#include "dateManager.h"
#include "dateChoose.h"

DateManager::DateManager(QWidget *parent) : QWidget(parent) {
    selectedDate = getCurrentDate();
}

QDate DateManager::getCurrentDate() {
    return QDate::currentDate();
}

QString DateManager::getSelectedDateText() const {
    QString DateText = selectedDate.toString("yyyy-MM-dd");
    return DateText;
}

QDate DateManager::getSelectedDate() const {
    return selectedDate;
}

void DateManager::newCalendar(QPoint pos) {
    dateChoose* calendarToChoose = new dateChoose(this);

    connect(calendarToChoose, &dateChoose::dateConfirmed, this, [=](QDate s_Date) {
        if( s_Date.isValid() ) {
            selectedDate = s_Date;
        }
        emit dateSent(selectedDate);

        calendarToChoose->deleteLater();
    });

    calendarToChoose->newCalendarToSelect(pos, 400, selectedDate);
}
