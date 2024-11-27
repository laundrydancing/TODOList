#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPoint>

class DragWidgetFilter : public QObject{
public:
    DragWidgetFilter(QObject* parent):QObject(parent){}
    bool eventFilter(QObject *watched, QEvent *event) {
        auto w=dynamic_cast<QWidget*>(watched);
        if(event->type()==QEvent::MouseButtonPress){
            QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
            if(mouseEvent->button()==Qt::LeftButton){
                isDragging=true;
                pos=mouseEvent->pos();
                return true;
            }
        }
        else if(event->type()==QEvent::MouseMove){
            if(isDragging){
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                w->move(mouseEvent->globalPosition().toPoint() - pos);
                return true;
            }
        }
        else if(event->type()==QEvent::MouseButtonRelease){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                isDragging = false;
                return true;
            }
        }
        return QObject::eventFilter(watched,event);
    }

private:
    bool isDragging;
    QPoint pos;
};

#endif // DRAGWIDGET_H
