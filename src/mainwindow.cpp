#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dragwidget.h"
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    this->installEventFilter(new DragWidgetFilter(this));

    //给listView加阴影
    setShadowEffect(ui->task1);
    setShadowEffect(ui->task2);
    setShadowEffect(ui->task3);
    setShadowEffect(ui->task4);
    setShadowEffect(ui->timeline);

    //时间系统管理
    datemanager=new DateManager(this);
    updateDate();
    connect(ui->calendar,&QPushButton::clicked,this,[this]() {
        ui->calendar->setStyleSheet("background-color:rgb(203, 203, 203);");
        QPoint calendarPos=ui->calendar->mapToGlobal(ui->calendar->pos());
        calendarPos.rx()-=55;
        calendarPos.ry()+=35;
        datemanager->newCalendar(calendarPos);
    }); //再点击一次calendar关掉日历，如何实现
    connect(datemanager,&DateManager::dateSent,this,&MainWindow::onDateExchange);

    //任务管理
    taskmanager=new taskManager(this,ui);
    taskmanager->refreshTask(datemanager->getSelectedDate());
    connect(ui->plus,&QPushButton::clicked,this, [=]() {
        taskmanager->newEditTask(datemanager->getSelectedDate());
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete datemanager;
    delete taskmanager;
}

void MainWindow::on_exit_Button_clicked()
{
    QApplication::quit();
}

void MainWindow::on_minus_Button_clicked()
{
    this->showMinimized();
}

void MainWindow::setShadowEffect(QWidget *widget) {
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(5);
    shadowEffect->setOffset(2, 2);
    shadowEffect->setColor(QColor(0, 0, 0, 10));
    widget->setGraphicsEffect(shadowEffect);
}

QDate MainWindow::updateDate(){
    QString newDateString=datemanager->getSelectedDateText();
    ui->date->setText(newDateString);
    QDate newDate=QDate::fromString(newDateString, "yyyy-MM-dd");
    return newDate;
}

void MainWindow::onDateExchange(){
    QDate newDate= updateDate();
    ui->calendar->setStyleSheet(
        "QPushButton { background-color: white; }"
        "QPushButton:hover { background-color: rgb(203, 203, 203); }"
        );

    taskmanager->refreshTask(newDate);
    //TODO: timeline的更新
}
