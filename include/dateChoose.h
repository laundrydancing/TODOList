#ifndef DATECHOOSE_H
#define DATECHOOSE_H

#include <QWidget>
#include <QCalendarWidget>
#include <QDateTime>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include <QFile>
#include <QToolButton>
#include <QMenu>

class dateChoose:public QWidget{
    Q_OBJECT;

public:
    dateChoose(QWidget *parent) : QWidget(parent){}
    void newCalendarToSelect(QPoint pos,int size,QDate date_){
        s_Date=date_;

        //新建对话窗口
        QDialog *calendarDialog = new QDialog(this,Qt::FramelessWindowHint | Qt::Dialog);
        calendarDialog->setAttribute(Qt::WA_TranslucentBackground);
        calendarDialog->setAttribute(Qt::WA_DeleteOnClose);
        QVBoxLayout *layout = new QVBoxLayout(calendarDialog);

        QWidget *containerWidget = new QWidget(calendarDialog);
        containerWidget->setFixedSize(size, size);
        QVBoxLayout *containerLayout = new QVBoxLayout(containerWidget);

        //新建日历控件
        QCalendarWidget *calendar= new QCalendarWidget(containerWidget);
        calendar->setSelectedDate(s_Date);
        calendar->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
        calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
        containerLayout->addWidget(calendar);

        //新建按键
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *calendarCancelButton = new QPushButton("Cancel", containerWidget);
        calendarCancelButton->setObjectName("calendarCancelButton");
        QPushButton *okButton = new QPushButton("OK", containerWidget);
        okButton->setObjectName("okButton");
        buttonLayout->addWidget(calendarCancelButton);
        buttonLayout->addWidget(okButton);
        containerLayout->addLayout(buttonLayout);
        layout->addWidget(containerWidget, 0, Qt::AlignCenter);

        //更改样式
        QFile styleFile(":/style_src/calendar_style.css");
        if (styleFile.open(QFile::ReadOnly)) {
            QString styleSheet = QTextStream(&styleFile).readAll();
            containerWidget->setStyleSheet(styleSheet);
        } else {
            qDebug() << "Unable to load calendar style file.";
        }

        //隐藏选择前/后月的箭头
        QToolButton *prevButton = calendar->findChild<QToolButton *>("qt_calendar_prevmonth");
        if (prevButton) {
            prevButton->hide();
        }
        QToolButton *nextButton = calendar->findChild<QToolButton *>("qt_calendar_nextmonth");
        if (nextButton) {
            nextButton->hide();
        }

        //连接按键信号与主窗口控制槽
        connect(okButton, &QPushButton::clicked, this,[this, calendarDialog]() {
            s_Date = calendarDialog->findChild<QCalendarWidget *>()->selectedDate();
            emit dateConfirmed(s_Date);
            calendarDialog->accept();
        });
        connect(calendarCancelButton, &QPushButton::clicked, this,[this, calendarDialog]() {
            emit dateConfirmed(QDate());
            calendarDialog->reject();
        });

        calendarDialog->move(pos);
        calendarDialog->exec();
    }

signals:
    void dateConfirmed(QDate s_Date);

private:
    QDate s_Date;

};


#endif // DATECHOOSE_H
