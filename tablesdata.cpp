#include "tablesdata.h"

#include <QDir>

TablesData::TablesData(QString jsonFileName, QObject *parent)
    : QObject(parent),
      jsonFile(QDir::currentPath() + "/" + jsonFileName)
{
}

void TablesData::load()
{
    if(!jsonFile.exists()) create();
    read();
}

void TablesData::create()
{
    QString emptyJson = R"({"meetings":[], "schedule":[[],[],[],[],[],[],[]]})";
    jsonFile.open(QIODevice::WriteOnly);
    jsonFile.write(emptyJson.toStdString().c_str());
    jsonFile.close();
}

void TablesData::read()
{
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    doc = QJsonDocument::fromJson(QString(jsonFile.readAll()).toUtf8());
    jsonToVectors(doc.object());
    jsonFile.close();

    emit dataLoaded();
}

void TablesData::jsonToVectors(QJsonObject mainObj)
{
    if(mainObj.contains("meetings") and mainObj["meetings"].isArray())
    {
        QJsonArray obj = mainObj["meetings"].toArray();
        if(!obj.isEmpty())
        {
            for(int i = 0; i < obj.count(); ++i)
            {
                QJsonObject meetingObj = obj[i].toObject();
                meetings.append(Meeting{meetingObj["name"].toString(),
                                        meetingObj["id"].toString(),
                                        meetingObj["password"].toString()});
            }
        }
    }

    if(mainObj.contains("schedule") and mainObj["schedule"].isArray())
    {
        QJsonArray week = mainObj["schedule"].toArray();
        if(!week.isEmpty())
        {
            for(int i = 0; i < 7; ++i)
            {
                QJsonArray day = week[i].toArray();
                QVector<Schedule> dayVector;
                for(int j = 0; j < day.count(); ++j)
                {
                    QJsonObject scheduleItemObj = day[j].toObject();
                    dayVector.append(Schedule{scheduleItemObj["name"].toString(),
                                         scheduleItemObj["start"].toString(),
                                         scheduleItemObj["duration"].toString()});
                }

                schedule.append(dayVector);
            }
        }
    }
}

void TablesData::write()
{
    jsonFile.open(QIODevice::WriteOnly);
    vectorsToJson(doc.object());
    jsonFile.write(doc.toJson());
    jsonFile.close();
}

void TablesData::vectorsToJson(QJsonObject mainObj)
{
    QJsonArray meetingsArray;
    foreach(Meeting meetingStruct, meetings)
    {
        QJsonObject meetingObj
        {
            {"name", meetingStruct.name},
            {"id", meetingStruct.id},
            {"password", meetingStruct.password}
        };
        meetingsArray.append(meetingObj);
    }
    mainObj["meetings"] = meetingsArray;

    QJsonArray week;
    for(int i = 0; i < 7; ++i)
    {
        QJsonArray day;
        foreach(Schedule scheduleStruct, schedule[i])
        {
            QJsonObject scheduleObj
            {
                {"name", scheduleStruct.name},
                {"start", scheduleStruct.start},
                {"duration", scheduleStruct.duration}
            };
            day.append(scheduleObj);
        }
        week.append(day);
    }
    mainObj["schedule"] = week;

    doc.setObject(mainObj);
}

// SLOTS

void TablesData::saveData()
{
    write();
}
