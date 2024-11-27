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
    explicit editTask(QWidget *parent = nullptr);
    ~editTask();

signals:
    //返回的变量列表顺序：name category ddl repeatPeriod description process
    void taskConfirmed(QVariantList taskContent);
    void cancelEdit();

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_Deadline_checkStateChanged(const Qt::CheckState &arg1);

    void on_repeat_checkStateChanged(const Qt::CheckState &arg1);

    void on_ddlCalendar_clicked();

private:
    Ui::editTask *ui;
};

#endif // EDITTASK_H
