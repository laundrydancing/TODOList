#ifndef EDITTASK_H
#define EDITTASK_H

#include <QWidget>
#include <QDate>

namespace Ui {
class editTask;
}

class editTask : public QWidget
{
    Q_OBJECT

public:
    explicit editTask(int s,QVariantMap* pretask=nullptr,QString preProcess=QString(),QWidget *parent = nullptr);
    ~editTask();

signals:
    //变量列表顺序：name category ddl repeatPeriod description process
    void taskConfirmed(QVariantList taskContent);
    void cancelEdit();
    void deleteComfired();

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_Deadline_checkStateChanged(const Qt::CheckState &arg1);

    void on_repeat_checkStateChanged(const Qt::CheckState &arg1);

    void on_ddlCalendar_clicked();

    void on_deleteButton_clicked();

private:
    int state; //0=>new one; 1=>edit one
    Ui::editTask *ui;
};

#endif // EDITTASK_H
