#ifndef DATACONNECT_H
#define DATACONNECT_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QSqlRecord>

bool connectToDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("task_database.db");

    if (!db.open()) {
        return false;
    } else {
        return true;
    }
}

void createTable() {
    QSqlQuery query;
    QString createTaskTable = R"(
    CREATE TABLE IF NOT EXISTS tasks (
        taskId INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT not null,
        description TEXT,
        deadline TEXT,
        setTime TEXT not null,
        repeatPeriod INTEGER,
        category INTEGER not null check(category between 0 and 3),
        isCompleted INTEGER not null check(isCompleted in (1,0))
    )
)";
    if (!query.exec(createTaskTable)) {
        qDebug() << "Error creating table:" << query.lastError();
    } else {
        qDebug() << "Tasks Table created successfully!";
    }

    QString createProcessTable = R"(
        CREATE TABLE IF NOT EXISTS process (
            taskId INTEGER,
            editTime TEXT,
            content TEXT,
            PRIMARY KEY (taskId, editTime),
            FOREIGN KEY (taskId) REFERENCES tasks(taskId) ON DELETE CASCADE
        )
    )";
    if (!query.exec(createProcessTable)) {
        qDebug() << "Error creating table:" << query.lastError();
    } else {
        qDebug() << "Tasks Table created successfully!";
    }
}

int insertTask(const QString &name, const QString &description, const QDateTime &deadline,
                const QDate &setTime, const int &repeatPeriod, const int &category,
                const int &isCompleted){
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO tasks (name, description, deadline, setTime, repeatPeriod, category, isCompleted)
        VALUES (:name, :description, :deadline, :setTime, :repeatPeriod, :category, :isCompleted)
    )");
    query.bindValue(":name", name);
    if (description.isEmpty()) {
        query.bindValue(":description", QVariant());
    } else {
        query.bindValue(":description", description);
    }

    if(!deadline.isValid()){
        query.bindValue(":deadline", QVariant());
    }
    else{
        query.bindValue(":deadline", deadline.toString("yyyy-MM-dd HH:mm"));
    }

    query.bindValue(":setTime", setTime.toString("yyyy-MM-dd"));
    query.bindValue(":repeatPeriod", repeatPeriod);
    query.bindValue(":category", category);
    query.bindValue(":isCompleted", isCompleted);
    if (!query.exec()) {
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool insertProcess(const int &taskID,const QDate &editTime,const QString &content) {
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO process(taskId,editTime,content)
        VALUES (:taskId,:editTime,:content)
    )");
    query.bindValue(":taskId",taskID);
    query.bindValue(":editTime",editTime);
    query.bindValue(":content",content);
    if(!query.exec()){
        return false;
    }
    return true;
}

bool updateTask(const int &taskId,const QString &name=QString(), const QString &description=QString(),
                const QDateTime &deadline = QDateTime(),bool setDeadlineToNull = false,
                const QDate &setTime = QDate(), const int* repeatPeriod=nullptr,
                const int *category=nullptr, const int *isCompleted=nullptr){
    QSqlQuery query;
    QString updateTaskString=("update tasks set ");

    QStringList updates;
    if(!name.isEmpty())updates<<"name=:name";
    if(!description.isEmpty())updates<<"description=:description";
    if(deadline.isValid())updates<<"deadline=:deadline";
    else if(setDeadlineToNull)updates<<"deadline=NULL";
    if(setTime.isValid())updates<<"setTime=:setTime";
    if(repeatPeriod!=nullptr)updates<<"repeatPeriod=:repeatPeriod";
    if(category!=nullptr)updates<<"category=:category";
    if(isCompleted!=nullptr)updates<<"isCompleted=:isCompleted";

    if(updates.isEmpty())return true;
    updateTaskString+=updates.join(", ");
    updateTaskString+=" where taskId=:taskId";

    query.prepare(updateTaskString);
    //qDebug() << "SQL Query: " << updateTaskString;

    if(!name.isEmpty())query.bindValue(":name", name);
    if(!description.isEmpty())query.bindValue(":description", description);
    if(deadline.isValid())query.bindValue(":deadline", deadline.toString("yyyy-MM-dd HH:mm"));
    if(setTime.isValid())query.bindValue(":setTime", setTime.toString("yyyy-MM-dd"));
    if(repeatPeriod!=nullptr)query.bindValue(":repeatPeriod", *repeatPeriod);
    if(category!=nullptr)query.bindValue(":category", *category);
    if(isCompleted!=nullptr)query.bindValue(":isCompleted", *isCompleted);

    query.bindValue(":taskId", taskId);

    if(!query.exec()){
        qDebug() << "SQL Error occurs in updating task:" << query.lastError().text();
        return false;
    }
    return true;
}

bool updateProcess(const int &taskID,const QDate &editTime,const QString &content=QString()){
    QSqlQuery query;
    QString updateProcessString=("update process set ");
    QStringList updates;

    if(!editTime.isValid()){
        qDebug()<<"there is a "<<editTime<<" when updating process\n";
        return false;
    }
    if(!content.isEmpty())updates<<"content=:content";

    if(updates.empty())return true;
    updateProcessString+=updates.join(", ");
    updateProcessString+=" where taskId=:taskId and editTime=:editTime";

    query.prepare(updateProcessString);

    query.bindValue(":taskId",taskID);
    query.bindValue(":editTime",editTime);
    if(!content.isEmpty())query.bindValue(":content",content);

    if(!query.exec()){
        qDebug() << "SQL Error occurs in updating process:" << query.lastError().text();
        return false;
    }
    return true;
}

bool deleteTask(const int& taskID){
    QSqlQuery query;
    query.prepare("delete from tasks where taskId=:taskId");
    query.bindValue(":taskId",taskID);
    if(!query.exec()){
        qDebug() << "SQL Error occurs in deleting task:" << query.lastError().text();
        return false;
    }
    return true;
}

bool deleteProcess(const int& taskID,const QDate &editTime){
    QSqlQuery query;
    query.prepare("delete from process where taskId=:taskID and editTime=:editTime");
    query.bindValue(":taskID",taskID);
    query.bindValue(":editTime",editTime);
    if(!query.exec()){
        qDebug() << "SQL Error occurs in deleting process:" << query.lastError().text();
        return false;
    }
    return true;
}

QVariantMap queryTaskbyID(const int& taskID){
    QSqlQuery query;
    query.prepare("select * from tasks where taskId=:taskId");
    query.bindValue(":taskId",taskID);

    QVariantMap task;
    if(query.exec()&&query.next()){
        QSqlRecord rec = query.record();
        for(int i=0;i<rec.count();++i){
            task.insert(rec.fieldName(i),query.value(i));
        }
    }
    return task;
}

QVariantList queryTaskbyDate(const QDate& selectedDate,const int& category){
    QSqlQuery query;
    query.prepare("select * from tasks "
                  "where category=:category "
                  "and ("
                  "(deadline IS NOT NULL AND deadline > DATE(:selectedDate, '-1 day') || ' 23:59' AND setTime <= :selectedDate) "
                  "or setTime = :selectedDate "
                  "or (repeatPeriod IS NOT NULL AND repeatPeriod != 0 AND (julianday(:selectedDate) - julianday(setTime)) % repeatPeriod = 0)"
                  ");"
                );
    query.bindValue(":selectedDate",selectedDate.toString("yyyy-MM-dd"));
    query.bindValue(":category",category);

    QVariantList tasks;
    if(query.exec()){
        while(query.next()){
            QVariantMap task;
            QSqlRecord rec = query.record();
            for(int i=0;i<rec.count();++i){
                task.insert(rec.fieldName(i),query.value(i));
            }
            tasks.append(task);
        }
    }
    return tasks;
}

QString queryProcess(const int &taskID,const QDate &editTime){
    QSqlQuery query;
    query.prepare("select content from process where taskId=:taskID and editTime=:editTime");
    query.bindValue(":taskID",taskID);
    query.bindValue(":editTime",editTime.toString("yyyy-MM-dd"));

    QString contentStr=QString();
    if(query.exec()&&query.next()){
        contentStr=query.value(0).toString();
    }
    else if(!query.exec()){
        qDebug() << "Query failed:" << query.lastError().text();
    }
    return contentStr;
}


#endif // DATACONNECT_H
