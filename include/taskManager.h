#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QWidget>
#include <QVBoxLayout>

namespace Ui {
class MainWindow;
}

class taskManager: public QWidget{
    Q_OBJECT;

public:
    explicit taskManager(QWidget* parent,Ui::MainWindow* parentUI);
    void refreshTask(QDate selectedDate);
    void updateTaskItem(int taskId);

public slots:
    void addTaskItem(int taskId,int category,QString taskName,int isCompleted,QString taskDescription=QString());
    void newEditTask(QDate newEditDate);

private:
    Ui::MainWindow* parentUi;
    QWidget* categoryWidgets[4];
    QVBoxLayout *layout[4];
    int taskWidgetNum;
};

#endif // TASKMANAGER_H
