#ifndef DATEMANAGER_H
#define DATEMANAGER_H

#include <QWidget>
#include <QDateTime>

class DateManager : public QWidget {
    Q_OBJECT

public:
    explicit DateManager(QWidget *parent);

    QDate getCurrentDate();

    QString getSelectedDateText() const;

    QDate getSelectedDate()const;

public slots:
    void newCalendar(QPoint pos);

signals:
    void dateSent(QDate selectedDate);

private:
    QDate selectedDate;
};
#endif // DATEMANAGER_H
