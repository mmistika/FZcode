#include "settings.h"

Settings::Settings(QString organizationName, QString appName, QObject *parent)
    : QObject(parent),
      organizationName(organizationName),
      appName(appName) {}

void Settings::load(){ loadSettings(); }

void Settings::loadSettings()
{
    QSettings settings(organizationName, appName);

    // UI
    maxVisibleItems = settings.value("UI/maxVisibleItems", maxVisibleItems).toInt();
    scheduleTableVisible = settings.value("UI/scheduleTableVisible", scheduleTableVisible).toBool();
    langCode = settings.value("UI/langCode", langCode).toString();

    // Other
    alwaysOnTop = settings.value("Other/alwaysOnTop", alwaysOnTop).toBool();
    saveLastWindowState = settings.value("Other/saveLastWindowState", saveLastWindowState).toBool();
    if(saveLastWindowState) lastWindowRect = settings.value("Other/lastWindowRect", lastWindowRect).toByteArray();

    emit settingsLoaded();
}

// SLOTS

void Settings::saveSettings()
{
    QSettings settings(organizationName, appName);

    settings.beginGroup("UI");
    settings.setValue("maxVisibleItems", maxVisibleItems);
    settings.setValue("scheduleTableVisible", scheduleTableVisible);
    settings.setValue("langCode", langCode);
    settings.endGroup();

    settings.beginGroup("Other");
    settings.setValue("alwaysOnTop", alwaysOnTop);
    settings.setValue("saveLastWindowState", saveLastWindowState);
    if (saveLastWindowState) settings.setValue("lastWindowRect", lastWindowRect);
    settings.endGroup();
}
