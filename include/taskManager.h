#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QWidget>
#include <QVBoxLayout>
#include "timeline.h"

namespace Ui {
class MainWindow;
}

class taskManager: public QWidget{
    Q_OBJECT;

public:
    explicit taskManager(QWidget* parent,Ui::MainWindow* parentUI);
    void refreshTask(QDate selectedDate);
    void updateTaskItem(int taskId,QDate selectedDate);

public slots:
    void addTaskItem(int taskId,QDate selectedDate,int category,QString taskName,int isCompleted,QString taskDescription=QString());
    void newEditTask(QDate newEditDate);

private:
    Ui::MainWindow* parentUi;
    QWidget* categoryWidgets[4];
    QVBoxLayout *layout[4];
    int taskWidgetNum;
    TimelineWidget* timelinewidget;
};

#endif // TASKMANAGER_H
