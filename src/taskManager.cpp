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

    taskWidgetNum=0;
}

#define taskItemWidth 310
void taskManager::addTaskItem(int taskId,int category,QString taskName,int isCompleted,QString taskDescription){
    TaskItem* newTaskItem=new TaskItem(taskId,taskName,isCompleted,taskDescription,categoryWidgets[category]);

    //更新任务完成状态
    connect(newTaskItem,&TaskItem::taskStateChange,this,[=](Qt::CheckState state){
        if(state==Qt::Checked){
            int Completed=1;
            updateTask(taskId,QString(),QString(),QDateTime(),QDate(),nullptr,nullptr, &Completed);
            layout[category]->removeWidget(newTaskItem);
            layout[category]->addWidget(newTaskItem);
        }
        else if(state==Qt::Unchecked){
            int Completed=0;
            updateTask(taskId,QString(),QString(),QDateTime(),QDate(),nullptr,nullptr, &Completed);
        }
    });

    connect(newTaskItem,&TaskItem::editButtonClicked,this,[=]{
        updateTaskItem(taskId);
    });

    layout[category]->insertWidget(0,newTaskItem);
}

void taskManager::refreshTask(QDate selectedDate){
    for(int i=0;i<4;++i){
        while (QLayoutItem* item = layout[i]->takeAt(0)) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
        }
        QVariantList taskList=queryTaskbyDate(selectedDate,i);

        //两次循环，先加已经完成了的任务，再加没有完成的任务（因为每次插入都是在顶部）
        for (const QVariant& taskVariant : taskList) {
            QVariantMap task = taskVariant.toMap();
            int isCompleted=task["isCompleted"].toInt();
            if(isCompleted==0)continue;
            int taskId=task["taskId"].toInt();
            QString taskName=task["name"].toString();
            QString taskDescription=task["description"].toString();
            addTaskItem(taskId,i,taskName,isCompleted,taskDescription);
        }
        for (const QVariant& taskVariant : taskList) {
            QVariantMap task = taskVariant.toMap();
            int isCompleted=task["isCompleted"].toInt();
            if(isCompleted==1)continue;
            int taskId=task["taskId"].toInt();
            QString taskName=task["name"].toString();
            QString taskDescription=task["description"].toString();
            addTaskItem(taskId,i,taskName,isCompleted,taskDescription);
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
            addTaskItem(taskID,category,name,0,description);

            if(!process.isEmpty()){
                insertProgress(taskID,newEditDate,process);
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


void taskManager::updateTaskItem(int taskID){
    QVariantMap previousTask=queryTaskbyID(taskID);
    editTask* taskWidget=new editTask(1,&previousTask);//TODO：需要传日期查process
    connect(taskWidget,&editTask::taskConfirmed,this,[=](QVariantList taskContent){
        QString name=taskContent[0].toString();
        int category=taskContent[1].toInt();
        QDateTime ddl=taskContent[2].toDateTime();
        int repeatPeriod=taskContent[3].toInt();
        QString description=taskContent[4].toString();
        QString process=taskContent[5].toString();

        int Completed=previousTask["isCompleted"].toInt();
        updateTask(taskID,name,description,ddl,QDate(),&repeatPeriod,&category, &Completed);
        //TODO:取代原框信息
        //TODO:处理日期问题
        /*
        if(!process.isEmpty()){
            insertProgress(taskID,,process);
        }*/
    });
    taskWidget->show();
}

//TODO:1.编辑任务板块的名字。
