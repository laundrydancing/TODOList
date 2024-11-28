#include "edittask.h"
#include "ui_edittask.h"
#include "dragwidget.h"
#include "dateChoose.h"

#include <QDateTime>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

editTask::editTask(int s,QVariantMap* pretask,QString preProcess,QWidget *parent)
    : state(s),QWidget(parent)
    , ui(new Ui::editTask)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    this->installEventFilter(new DragWidgetFilter(this));

    ui->setupUi(this);
    ui->labelChoose->addItems({"Task1","Task2","Task3","Task4"});//TODO：用户自定义标签名字

    ui->Deadline->setLayoutDirection(Qt::RightToLeft);
    ui->repeat->setLayoutDirection(Qt::RightToLeft);

    QRegularExpression dateRegex("^\\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])$");
    QRegularExpressionValidator *dateValidator = new QRegularExpressionValidator(dateRegex, this);
    ui->ddlText->setValidator(dateValidator);
    ui->ddlText->setPlaceholderText("yyyy-mm-dd");

    QRegularExpression timeRegex("^([0-1][0-9]|2[0-3]):([0-5][0-9])$");
    QRegularExpressionValidator *timeValidator =new QRegularExpressionValidator(timeRegex,this);
    ui->ddlTimeText->setValidator(timeValidator);
    ui->ddlTimeText->setPlaceholderText("00:00-23:59");

    QRegularExpression repeatRegex("^[1-9]|([1-3][0-9])$");
    QRegularExpressionValidator *daysValidator=new QRegularExpressionValidator(repeatRegex,this);
    ui->repeatEdit->setValidator(daysValidator);
    ui->repeatEdit->setPlaceholderText("1-30");

    ui->descriptionEdit->setMaxLength(20);
    ui->descriptionEdit->setPlaceholderText("20 characters/letters only");

    ui->nameAlert->setVisible(false);
    ui->deleteButton->setIcon(QIcon(":/img/Trash"));
    ui->deleteButton->setIconSize(QSize(20, 20));

    if(state==0){
        ui->deleteButton->setVisible(false);
        ui->deleteText->setVisible(false);
    }

    if(state==1){
        ui->nameEdit->setText(pretask->value("name").toString());
        ui->labelChoose->setCurrentIndex(pretask->value("category").toInt());
        if(!pretask->value("deadline").isNull()){
            ui->Deadline->setCheckState(Qt::Checked);
            QString dateTime=pretask->value("deadline").toString();
            QStringList parts = dateTime.split(" ");
            ui->ddlText->setText(parts[0]);
            ui->ddlTimeText->setText(parts[1]);
        }
        if(pretask->value("repeatPeriod").toInt()!=0){
            ui->repeat->setCheckState(Qt::Checked);
            ui->repeatEdit->setText(pretask->value("repeatPeriod").toString());
        }
        if(!pretask->value("description").isNull()){
            ui->descriptionEdit->setText(pretask->value("description").toString());
        }
        if(!preProcess.isNull()){
            ui->processEdit->setText(preProcess);
        }
    }
}

editTask::~editTask()
{
    delete ui;
}

void editTask::on_okButton_clicked()
{
    QString nameStr=ui->nameEdit->toPlainText();
    if(nameStr.isEmpty()){
        ui->nameAlert->setVisible(true);
        return;
    }

    QVariantList taskContent;
    taskContent.append(nameStr);
    taskContent.append(ui->labelChoose->currentIndex());

    if(ui->Deadline->isChecked()){
        QString ddlDatetimeStr=ui->ddlText->text()+" "+ui->ddlTimeText->text();
        QDateTime ddlDatetime=QDateTime::fromString(ddlDatetimeStr,"yyyy-MM-dd HH:mm");
        taskContent.append(ddlDatetime);
    }
    else{
        taskContent.append(QDateTime());
    }

    if(ui->repeat->isChecked()){
        int repeatPeriod=ui->repeatEdit->text().toInt();
        taskContent.append(repeatPeriod);
    }
    else{
        taskContent.append(0);
    }

    taskContent.append(ui->descriptionEdit->text());
    taskContent.append(ui->processEdit->toPlainText());

    emit taskConfirmed(taskContent);
    this->close();
}

void editTask::on_cancelButton_clicked()
{
    emit cancelEdit();
    this->close();
}


void editTask::on_Deadline_checkStateChanged(const Qt::CheckState &arg1)
{
    bool enableState=false;
    if(arg1==Qt::Checked){
        enableState=true;
    }
    ui->ddlText->setEnabled(enableState);
    ui->ddlTimeText->setEnabled(enableState);
    ui->ddlCalendar->setEnabled(enableState);
}


void editTask::on_repeat_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1==Qt::Checked){
        ui->repeatEdit->setEnabled(true);
    }
    else{
        ui->repeatEdit->setEnabled(false);
    }
}


void editTask::on_ddlCalendar_clicked()
{
    dateChoose* calendarToChoose=new dateChoose(this);

    connect(calendarToChoose,&dateChoose::dateConfirmed,this,[=](QDate s_Date) {
        if(s_Date.isValid()){
            ui->ddlText->setText(s_Date.toString("yyyy-MM-dd"));
        }

        calendarToChoose->deleteLater();
    });

    QPoint calendarPos=ui->ddlCalendar->mapToGlobal(ui->ddlCalendar->pos());
    calendarPos.rx()-=165;
    calendarPos.ry()-=180;
    calendarToChoose->newCalendarToSelect(calendarPos,300,QDate::currentDate());
}


void editTask::on_deleteButton_clicked()
{
    QDialog* recomfired=new QDialog();
    recomfired->setObjectName("recomfired");
    recomfired->setWindowFlag(Qt::FramelessWindowHint);
    //recomfired->setAttribute(Qt::WA_TranslucentBackground);
    recomfired->resize(400,300);
    recomfired->installEventFilter(new DragWidgetFilter(recomfired));
    QFile dialogStyle(":/style_src/dialog_style.css");
    if(dialogStyle.open(QFile::ReadOnly)){
        QString dialogStyleStr=dialogStyle.readAll();
        recomfired->setStyleSheet(dialogStyleStr);
    }

    QVBoxLayout* mainlayout=new QVBoxLayout(recomfired);
    QLabel* labeltext=new QLabel("Are you sure you want to delete this task?\nThis action cannot be undone.");
    mainlayout->addWidget(labeltext);
    labeltext->setMinimumHeight(100);

    QHBoxLayout* buttonlayout=new QHBoxLayout();
    QPushButton* yesButton=new QPushButton("Yes, delete it.");
    yesButton->setMinimumHeight(40);
    buttonlayout->addWidget(yesButton);
    QPushButton* cancelButton=new QPushButton("No, keep it.");
    cancelButton->setMinimumHeight(40);
    buttonlayout->addWidget(cancelButton);

    mainlayout->addLayout(buttonlayout);

    mainlayout->setStretch(0, 4);
    mainlayout->setStretch(1, 1);

    connect(yesButton,&QPushButton::clicked,recomfired,[=]{
        emit deleteComfired();
        recomfired->accept();
        this->close();
    });

    connect(cancelButton,&QPushButton::clicked,recomfired,[=]{
        recomfired->reject();
    });

    recomfired->exec();
}

