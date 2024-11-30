#include "taskManager.h"
#include "ui_mainwindow.h"

#include "dataconnect.h"
#include "edittask.h"
#include "taskitem.h"

taskManager::taskManager(QWidget* parent,Ui::MainWindow* parentUI):QWidget(parent),parentUi(parentUI) {
    //连接数据库
    if(connectToDatabase()){
        createTable();
    }

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    categoryWidgets[0] = parentUi->task1_mainwidget;
    categoryWidgets[1] = parentUi->task2_mainwidget;
    categoryWidgets[2] = parentUi->task3_mainwidget;
    categoryWidgets[3] = parentUi->task4_mainwidget;

    for(int i=0;i<4;++i){
        layout[i]=new QVBoxLayout(categoryWidgets[i]);
        layout[i]->setAlignment(Qt::AlignTop);
    }

    timelinewidget=new TimelineWidget(parentUi->uselesswidget);
    timelinewidget->setObjectName("timelinewidget");

    taskWidgetNum=0;
}

#define taskItemWidth 310
void taskManager::addTaskItem(int taskId,QDate selectedDate,int category,QString taskName,int isCompleted,QString taskDescription){
    TaskItem* newTaskItem=new TaskItem(taskId,taskName,isCompleted,taskDescription,categoryWidgets[category]);

    //更新任务完成状态
    connect(newTaskItem,&TaskItem::taskStateChange,this,[=](Qt::CheckState state){
        if(state==Qt::Checked){
            int Completed=1;
            updateTask(taskId,QString(),QString(),QDateTime(),false,QDate(),nullptr,nullptr, &Completed);
            layout[category]->removeWidget(newTaskItem);
            layout[category]->addWidget(newTaskItem);
            timelinewidget->updateCompletation(taskId,1);
        }
        else if(state==Qt::Unchecked){
            int Completed=0;
            updateTask(taskId,QString(),QString(),QDateTime(),false,QDate(),nullptr,nullptr, &Completed);
            timelinewidget->updateCompletation(taskId,0);
        }
    });

    connect(newTaskItem,&TaskItem::editButtonClicked,this,[=]{
        updateTaskItem(taskId,selectedDate);
    });

    layout[category]->insertWidget(0,newTaskItem);
}

void taskManager::refreshTask(QDate selectedDate){
    timelinewidget->refreshTimeline();
    for(int i=0;i<4;++i){
        while (QLayoutItem* item = layout[i]->takeAt(0)) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
        }
        QVariantList taskList=queryTaskbyDate(selectedDate,i);

        //两次循环，先加已经完成了的任务，再加没有完成的任务（因为每次插入都是在顶部）
        //copy-and-paste了，或许可以重构
        for (const QVariant& taskVariant : taskList) {
            QVariantMap task = taskVariant.toMap();
            int isCompleted=task["isCompleted"].toInt();
            if(isCompleted==0)continue;
            int taskId=task["taskId"].toInt();
            QString taskName=task["name"].toString();
            QString taskDescription=task["description"].toString();
            addTaskItem(taskId,selectedDate,i,taskName,isCompleted,taskDescription);

            QDateTime ddl=task["deadline"].toDateTime();
            if(ddl.isValid()){
                if(ddl.date()==selectedDate){
                    timelinewidget->addTimelineItem(ddl,taskName,taskId,isCompleted);
                }
            }
        }
        for (const QVariant& taskVariant : taskList) {
            QVariantMap task = taskVariant.toMap();
            int isCompleted=task["isCompleted"].toInt();
            if(isCompleted==1)continue;
            int taskId=task["taskId"].toInt();
            QString taskName=task["name"].toString();
            QString taskDescription=task["description"].toString();
            addTaskItem(taskId,selectedDate,i,taskName,isCompleted,taskDescription);

            QDateTime ddl=task["deadline"].toDateTime();
            if(ddl.isValid()){
                if(ddl.date()==selectedDate){
                    timelinewidget->addTimelineItem(ddl,taskName,taskId,isCompleted);
                }
            }
        }
    }
}

void taskManager::newEditTask(QDate newEditDate){
    if(taskWidgetNum==0){
        editTask* taskWidget=new editTask(0);

        connect(taskWidget,&editTask::taskConfirmed,this,[=](QVariantList taskContent){
            taskWidgetNum--;

            QString name=taskContent[0].toString();
            int category=taskContent[1].toInt();
            QDateTime ddl=taskContent[2].toDateTime();
            int repeatPeriod=taskContent[3].toInt();
            QString description=taskContent[4].toString();
            QString process=taskContent[5].toString();

            int taskID=insertTask(name,description,ddl,newEditDate, repeatPeriod, category,0);
            addTaskItem(taskID,newEditDate,category,name,0,description);

            if(ddl.isValid()){
                if(ddl.date()==newEditDate)
                timelinewidget->addTimelineItem(ddl,name,taskID,0);
            }

            if(!process.isEmpty()){
                insertProcess(taskID,newEditDate,process);
            }

            taskWidget->deleteLater();
        });

        connect(taskWidget,&editTask::cancelEdit,this,[=]{
            taskWidgetNum--;

            taskWidget->deleteLater();
        });

        taskWidget->show();
        taskWidgetNum++;
    }
}

void taskManager::updateTaskItem(int taskID,QDate selectedDate){
    if(taskWidgetNum==0){
        QVariantMap previousTask=queryTaskbyID(taskID);
        QString preProcess=queryProcess(taskID,selectedDate);
        editTask* taskWidget=new editTask(1,&previousTask,preProcess);

        connect(taskWidget,&editTask::taskConfirmed,this,[=](QVariantList taskContent){
            taskWidgetNum--;

            int Completed=previousTask["isCompleted"].toInt();
            QString name=taskContent[0].toString();
            int category=taskContent[1].toInt();

            //感觉这串条件判断好蠢，，可以简化吗？
            QDateTime ddl=taskContent[2].toDateTime();
            bool setDDLNull=false;
            if(!ddl.isValid()){
                setDDLNull=true;
                timelinewidget->deleteTimelineItem(taskID);
            }
            else if(previousTask["deadline"].toDateTime().date()==selectedDate && ddl.date()!=selectedDate){
                timelinewidget->deleteTimelineItem(taskID);
            }
            else if(previousTask["deadline"].toDateTime().date()!=selectedDate && ddl.date()==selectedDate){
                timelinewidget->addTimelineItem(ddl,name,taskID,Completed);
            }
            else if(ddl!=previousTask["deadline"].toDateTime()){
                timelinewidget->updateTimeline(ddl,name,taskID);
            }
            else if(name!=previousTask["name"].toString()){
                timelinewidget->updateTimeline(ddl,name,taskID);
            }

            int repeatPeriod=taskContent[3].toInt();
            QString description=taskContent[4].toString();
            QString process=taskContent[5].toString();

            updateTask(taskID,name,description,ddl,setDDLNull,QDate(),&repeatPeriod,&category, &Completed);

            int preCategory=previousTask["category"].toInt();
            if(preCategory!=category){
                QString taskItemName=QString::number(taskID);
                for (int i = 0; i < layout[preCategory]->count(); ++i){
                    QLayoutItem* item =layout[preCategory]->itemAt(i);
                    if (item) {
                        QWidget* widget = item->widget();
                        if(widget->objectName()==taskItemName){
                            widget->deleteLater();
                            break;
                        }
                    }
                }
                addTaskItem(taskID,selectedDate,category,name,Completed,description);
            }
            else{
                QString taskItemName=QString::number(taskID);
                for (int i = 0; i < layout[preCategory]->count(); ++i){
                    QLayoutItem* item =layout[preCategory]->itemAt(i);
                    if (item) {
                        QWidget* widget = item->widget();
                        if(widget->objectName()==taskItemName){
                            QCheckBox* taskButton = widget->findChild<QCheckBox*>(taskItemName+"name");
                            taskButton->setText(name);
                            QLabel* descriptionLabel=widget->findChild<QLabel*>(taskItemName+"description");
                            descriptionLabel->setText(description);
                            break;
                        }
                    }
                }
            }

            if(preProcess.isEmpty()){
                insertProcess(taskID,selectedDate,process);
            }
            else if(process.isEmpty()){
                deleteProcess(taskID,selectedDate);
            }
            else if(preProcess!=process){
                updateProcess(taskID,selectedDate,process);
            }
        });

        connect(taskWidget,&editTask::cancelEdit,this,[=]{
            taskWidgetNum--;
        });

        connect(taskWidget,&editTask::deleteComfired,this,[=]{
            taskWidgetNum--;
            deleteTask(taskID);

            QString taskItemName=QString::number(taskID);
            int preCategory=previousTask["category"].toInt();
            for (int i = 0; i < layout[preCategory]->count(); ++i){
                QLayoutItem* item =layout[preCategory]->itemAt(i);
                if (item) {
                    QWidget* widget = item->widget();
                    if(widget->objectName()==taskItemName){
                        widget->deleteLater();
                        break;
                    }
                }
            }

            timelinewidget->deleteTimelineItem(taskID);
        });

        taskWidgetNum++;
        taskWidget->show();
    }
}

//TODO:1.一键删除某日全部任务。
