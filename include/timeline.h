#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>
#include <QPainter>
#include <QVBoxLayout>
#include <QDateTime>
#include <QLabel>

class TimelineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        setFixedSize(201, 531);
        timelineLayout=new QVBoxLayout(this);
        timelineLayout->setAlignment(Qt::AlignTop);
    }

    ~TimelineWidget(){
        delete timelineLayout;
    }

    void addTimelineItem(QDateTime Deadline,QString taskName,int taskId,int isCompleted){
        QString taskItemName=QString::number(taskId);

        QWidget* timelineItem=new QWidget();
        timelineItem->setFixedSize(230,50);
        timelineItem->setObjectName(taskItemName);
        timelineItem->setStyleSheet("background-color:rgba(0,0,0,0);");

        QTime ddlTime=Deadline.time();
        QString ddltimeStr=ddlTime.toString();
        QLabel* timetext=new QLabel(ddltimeStr,timelineItem);
        timetext->setObjectName(taskItemName+"time");
        timetext->setFixedSize(60,50);
        timetext->move(8,0);

        QLabel* nametext=new QLabel(taskName,timelineItem);
        nametext->setObjectName(taskItemName+"name");
        nametext->setFixedSize(110,50);
        nametext->move(83,0);

        QLabel* taskPoint=new QLabel("",timelineItem);
        taskPoint->setFixedSize(15,15);
        taskPoint->move(64,16);
        taskPoint->setStyleSheet("image:url(:/img/point.svg);");

        QString labelStyleStr;
        if(isCompleted){
            labelStyleStr=R"(
                QLabel{
                    background-color:rgba(0,0,0,0);
                    border:none;
                    font: 10pt "微软雅黑";
                    color:#bdbdbd;
                }
            )";
        }
        else{
            labelStyleStr=R"(
                QLabel{
                    background-color:rgba(0,0,0,0);
                    border:none;
                    font: 10pt "微软雅黑";
                }
            )";
        }
        nametext->setStyleSheet(labelStyleStr);
        timetext->setStyleSheet(labelStyleStr);

        timelineLayout->addWidget(timelineItem);
    }

    void deleteTimelineItem(int taskId){
        QString taskItemName=QString::number(taskId);
        for (int i = 0; i < timelineLayout->count(); ++i){
            QLayoutItem* item =timelineLayout->itemAt(i);
            if (item) {
                QWidget* widget = item->widget();
                if(widget->objectName()==taskItemName){
                    widget->deleteLater();
                    break;
                }
            }
        }
    }

    void refreshTimeline(){
        for (int i = 0; i < timelineLayout->count(); ++i){
            QLayoutItem* item =timelineLayout->itemAt(i);
            if (item) {
                QWidget* widget = item->widget();
                widget->deleteLater();
            }
        }
    }

    void updateTimeline(QDateTime Deadline,QString taskName,int taskId){
        QString taskItemName=QString::number(taskId);
        for (int i = 0; i < timelineLayout->count(); ++i){
            QLayoutItem* item =timelineLayout->itemAt(i);
            if (item) {
                QWidget* widget = item->widget();
                if(widget->objectName()==taskItemName){
                    QLabel* timetext=widget->findChild<QLabel*>(taskItemName+"time");
                    QTime ddlTime=Deadline.time();
                    QString ddltimeStr=ddlTime.toString();
                    timetext->setText(ddltimeStr);
                    QLabel* nametext=widget->findChild<QLabel*>(taskItemName+"name");
                    nametext->setText(taskName);
                    break;
                }
            }
        }
    }

    void updateCompletation(int taskId,int isCompleted){
        QString labelStyleStr;
        if(isCompleted){
            labelStyleStr=R"(
                QLabel{
                    background-color:rgba(0,0,0,0);
                    border:none;
                    font: 10pt "微软雅黑";
                    color:#bdbdbd;
                }
            )";
        }
        else{
            labelStyleStr=R"(
                QLabel{
                    background-color:rgba(0,0,0,0);
                    border:none;
                    font: 10pt "微软雅黑";
                }
            )";
        }
        QString taskItemName=QString::number(taskId);
        for (int i = 0; i < timelineLayout->count(); ++i){
            QLayoutItem* item =timelineLayout->itemAt(i);
            if (item) {
                QWidget* widget = item->widget();
                if(widget->objectName()==taskItemName){
                    QLabel* timetext=widget->findChild<QLabel*>(taskItemName+"time");
                    timetext->setStyleSheet(labelStyleStr);
                    QLabel* nametext=widget->findChild<QLabel*>(taskItemName+"name");
                    nametext->setStyleSheet(labelStyleStr);
                    break;
                }
            }
        }
    }

    //TODO: 1.插入timelineItem的时候按时间排序，更新时间的时候按时间顺序调整位置

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setPen(QPen(QColor(57,57,57,205), 3));

        QPoint startPoint(80, 10);
        QPoint endPoint(80, 520);

        painter.drawLine(startPoint, endPoint);
    }

private:
    QVBoxLayout* timelineLayout;
};



#endif // TIMELINE_H
