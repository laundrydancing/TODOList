#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "taskManager.h"
#include "dateManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setShadowEffect(QWidget *widget);

    QDate updateDate();

    void loadCategoryText();

private slots:
    void on_exit_Button_clicked();

    void on_minus_Button_clicked();

    void onDateExchange();

    void updateCategoryText(int label,QPoint pos,QLabel* label_task);


private:
    Ui::MainWindow *ui;
    DateManager* datemanager;
    taskManager* taskmanager;
    int calendarNum=0;
};

#endif // MAINWINDOW_H
