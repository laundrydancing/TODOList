#ifndef TASKITEM_H
#define TASKITEM_H

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

class TaskItem : public QWidget
{
    Q_OBJECT
public:
    explicit TaskItem(int taskId, QString taskName, int isCompleted, QString taskDescription,QWidget *parent = nullptr);

signals:
    void taskStateChange(Qt::CheckState state);
    void editButtonClicked();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setAllChildrenBackground(bool isHovered);
    int taskId;
    QVBoxLayout* taskItemLayout;
    QCheckBox *taskButton;
    QLabel *descriptionLabel;
    QHBoxLayout* wholeItem;
    QPushButton* editButton;
};

#endif // TASKITEM_H
