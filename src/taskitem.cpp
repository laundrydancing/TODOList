#include "taskitem.h"
#include <QFile>
#include <QEvent>
#include <QObject>

#define taskItemWidth 260
TaskItem::TaskItem(int taskId, QString taskName, int isCompleted, QString taskDescription,QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(taskItemWidth+50,50);
    this->installEventFilter(this);
    QString taskItemName=QString::number(taskId);
    this->setObjectName(taskItemName);
    QFile widgetsStyle(":/style_src/task_style.css");
    if(widgetsStyle.open(QFile::ReadOnly)){
        QString widgetsStyleString=widgetsStyle.readAll();
        this->setStyleSheet(widgetsStyleString);
    }

    wholeItem=new QHBoxLayout(this);
    wholeItem->setContentsMargins(0, 0, 0, 0);
    wholeItem->setSpacing(0);

    taskItemLayout=new QVBoxLayout();
    taskItemLayout->setContentsMargins(5, 5, 5, 5);
    taskItemLayout->setSpacing(2);
    taskItemLayout->setAlignment(Qt::AlignLeft);

    //任务名
    taskButton=new QCheckBox(this);
    taskButton->setText(taskName);
    taskButton->setFixedSize(taskItemWidth,30);
    taskItemLayout->addWidget(taskButton);
    if(isCompleted==1)taskButton->setCheckState(Qt::Checked);

    //任务描述
    descriptionLabel=nullptr;
    if(!taskDescription.isEmpty()){
        descriptionLabel=new QLabel(taskDescription,this);
        descriptionLabel->resize(taskItemWidth,20);
        descriptionLabel->setWordWrap(true);
        taskItemLayout->addWidget(descriptionLabel);
    }

    wholeItem->addLayout(taskItemLayout);

    editButton=new QPushButton("",this);
    editButton->setIcon(QIcon(":/img/Edit.svg"));
    editButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    editButton->setIconSize(QSize(20, 20));
    editButton->setMinimumHeight(40);
    editButton->setMinimumWidth(42);
    wholeItem->addWidget(editButton);

    wholeItem->setStretch(0, 3);
    wholeItem->setStretch(1, 1);

    connect(taskButton,&QCheckBox::checkStateChanged,this,[this](Qt::CheckState state){
        emit taskStateChange(state);
    });
    connect(editButton,&QPushButton::clicked,this,[this]{
        emit editButtonClicked();
        //TODO：加个限制，不能多开
    });
}

bool TaskItem::eventFilter(QObject *watched, QEvent *event) {
    if (watched == this) {
        if (event->type() == QEvent::Enter) {
            setAllChildrenBackground(true);
        } else if (event->type() == QEvent::Leave) {
            setAllChildrenBackground(false);
        }
    }
    return QWidget::eventFilter(watched, event);
}

void TaskItem::setAllChildrenBackground(bool isHovered) {
    QString backgroundColor = isHovered ? "background-color:#f5f5f5;" : "background-color:#ffffff;";

    foreach (QWidget *child, findChildren<QWidget*>()) {
        if (child !=editButton)
        child->setStyleSheet(backgroundColor);
    }
}
