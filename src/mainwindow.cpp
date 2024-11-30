#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dragwidget.h"

#include <QGraphicsDropShadowEffect>
#include <QFile>
#include <QSettings>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    this->installEventFilter(new DragWidgetFilter(this));

    //初始化标签
    loadCategoryText();
    connect(ui->task1_edit,&QPushButton::clicked,this,[=]{
        updateCategoryText(0,QPoint(100,100),ui->task1_label);
    });
    connect(ui->task2_edit,&QPushButton::clicked,this,[=]{
        updateCategoryText(1,QPoint(510,100),ui->task2_label);
    });
    connect(ui->task3_edit,&QPushButton::clicked,this,[=]{
        updateCategoryText(2,QPoint(100,400),ui->task3_label);
    });
    connect(ui->task4_edit,&QPushButton::clicked,this,[=]{
        updateCategoryText(3,QPoint(510,400),ui->task4_label);
    });

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
    }); //TODO: 再点击一次calendar关掉日历
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
}

void MainWindow::loadCategoryText(){
    QFile file("./settings.ini");

    if (!file.exists()) {
        QSettings settings("./settings.ini", QSettings::IniFormat);
        settings.setValue("label1", ui->task1_label->text());
        settings.setValue("label2", ui->task2_label->text());
        settings.setValue("label3", ui->task3_label->text());
        settings.setValue("label4", ui->task4_label->text());
    } else {
        QSettings settings("./settings.ini", QSettings::IniFormat);
        ui->task1_label->setText(settings.value("label1").toString());
        ui->task2_label->setText(settings.value("label2").toString());
        ui->task3_label->setText(settings.value("label3").toString());
        ui->task4_label->setText(settings.value("label4").toString());
    }
    file.close();
}

void MainWindow::updateCategoryText(int label,QPoint pos,QLabel* tasklabel){
    tasklabel->setVisible(false);
    QWidget* categoryEditWidget=new QWidget(this);
    categoryEditWidget->setFixedSize(300,50);
    categoryEditWidget->move(pos);
    QHBoxLayout* categoryEditLayout=new QHBoxLayout(categoryEditWidget);
    categoryEditLayout->setContentsMargins(5, 5, 5, 5);
    categoryEditLayout->setSpacing(2);

    QTextEdit* categoryedit=new QTextEdit();
    categoryedit->setFixedSize(250,38);
    categoryedit->setText(tasklabel->text());
    categoryedit->setStyleSheet("border:1px solid #a1a1a1;"
                                "border-radius:10px;"
                                "padding: 4px 0px;"
                                "font:11pt \"微软雅黑\";");
    categoryEditLayout->addWidget(categoryedit);

    QPushButton* comfiredButton=new QPushButton("");
    comfiredButton->setFixedSize(QSize(30,30));
    comfiredButton->setIcon(QIcon(":/img/Check.svg"));
    comfiredButton->setIconSize(QSize(20, 20));
    comfiredButton->setStyleSheet("QPushButton{"
                                  "border:none;"
                                  "}"
                                  "QPushButton:hover{"
                                  "background-color:#bebebe;"
                                  "}");
    categoryEditLayout->addWidget(comfiredButton);

    categoryEditWidget->setLayout(categoryEditLayout);

    connect(comfiredButton,&QPushButton::clicked,this,[=]{
        QString newname=categoryedit->toPlainText();
        QFile file("./settings.ini");
        QSettings settings("./settings.ini", QSettings::IniFormat);
        switch(label){
        case 0:settings.setValue("label1", newname);break;
        case 1:settings.setValue("label2", newname);break;
        case 2:settings.setValue("label3", newname);break;
        case 3:settings.setValue("label4", newname);break;
        }
        file.close();

        categoryEditWidget->deleteLater();
        tasklabel->setText(newname);
        tasklabel->setVisible(true);
    });

    categoryEditWidget->show();
}
