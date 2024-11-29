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

    void addTimelineItem(QDateTime Deadline,QString taskName,int taskId){
        QWidget* timelineItem=new QWidget();
        timelineItem->setFixedSize(230,50);
        timelineItem->setObjectName(QString::number(taskId));
        timelineItem->setStyleSheet("background-color:rgba(0,0,0,0);");

        QTime ddlTime=Deadline.time();
        QString ddltimeStr=ddlTime.toString();
        QLabel* timetext=new QLabel(ddltimeStr,timelineItem);
        timetext->setFixedSize(60,50);
        timetext->move(8,0);
        QLabel* nametext=new QLabel(taskName,timelineItem);
        nametext->setFixedSize(110,50);
        nametext->move(83,0);
        QLabel* taskPoint=new QLabel("",timelineItem);
        taskPoint->setFixedSize(15,15);
        taskPoint->move(64,16);
        QString labelStyleStr=R"(
            QLabel{
                background-color:rgba(0,0,0,0);
                border:none;
                font: 10pt "微软雅黑";
            }
        )";
        nametext->setStyleSheet(labelStyleStr);
        timetext->setStyleSheet(labelStyleStr);
        taskPoint->setStyleSheet(labelStyleStr);
        taskPoint->setStyleSheet("image:url(:/img/point.svg);");

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

    //TODO: 1.修改ddl时间的时候同步更新 2.插入timelineItem的时候按时间排序 3.已经完成了的任务变灰显示（如果修改了任务的完成情况也要同步更新）

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
