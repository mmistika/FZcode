#ifndef TABLESDATA_H
#define TABLESDATA_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>
#include <QVector>
#include <QString>

struct Meeting
{
    QString name;
    QString id;
    QString password;
};

struct Schedule
{
    QString name;
    QString start;
    QString duration;
};

class TablesData : public QObject
{
    Q_OBJECT

    QFile jsonFile;
    QJsonDocument doc;

public:
    TablesData(QString jsonFileName, QObject *parent=nullptr);
    void load();

    QVector<Meeting> meetings;
    QVector<QVector<Schedule>> schedule;

signals:
    void dataLoaded();

public slots:
    void saveData();

private:
    void read();
    void write();
    void create();
    void jsonToVectors(QJsonObject mainObj);
    void vectorsToJson(QJsonObject mainObj);
};

#endif // TABLESDATA_H
