#include "taskManager.h"
#include "ui_mainwindow.h"

#include "dataconnect.h"
#include "edittask.h"

#include <QFile>

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
    QWidget* newTaskItem=new QWidget(categoryWidgets[category]);
    newTaskItem->setFixedSize(taskItemWidth,50);
    QString taskItemName=QString::number(taskId);
    newTaskItem->setObjectName(taskItemName);
    QFile widgetsStyle(":/style_src/task_style.css");
    if(widgetsStyle.open(QFile::ReadOnly)){
        QString widgetsStyleString=widgetsStyle.readAll();
        newTaskItem->setStyleSheet(widgetsStyleString);
    }

    QVBoxLayout* taskItemLayout=new QVBoxLayout(newTaskItem);
    taskItemLayout->setContentsMargins(5, 5, 5, 5);
    taskItemLayout->setSpacing(2);
    taskItemLayout->setAlignment(Qt::AlignLeft);

    //任务名
    QCheckBox* taskButton=new QCheckBox(newTaskItem);
    taskButton->setText(taskName);
    taskButton->setFixedSize(taskItemWidth,30);
    taskItemLayout->addWidget(taskButton);
    if(isCompleted==1)
    taskButton->setCheckState(Qt::Checked);

    //任务描述
    QLabel* descriptionLabel=nullptr;
    if(!taskDescription.isEmpty()){
        descriptionLabel=new QLabel(taskDescription,newTaskItem);
        descriptionLabel->resize(taskItemWidth,20);
        descriptionLabel->setWordWrap(true);
        taskItemLayout->addWidget(descriptionLabel);
    }

    //更新任务完成状态
    connect(taskButton,&QCheckBox::checkStateChanged,this,[=](Qt::CheckState state){
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
        editTask* taskWidget=new editTask();

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

void taskManager::updateTaskItem(int taskID,QCheckBox* nameBox,QLabel* descriptionLabel){
    if(descriptionLabel==nullptr){
        //TODO:没想好
    }
    editTask* taskWidget=new editTask();
    taskWidget->show();
}

//TODO:1.编辑任务板块的名字。2. 弹出任务窗口，显示已有的任务内容，并支持编辑任务和更新任务。
