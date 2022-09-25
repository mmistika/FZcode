#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT

    QString organizationName;
    QString appName;

public:
    explicit Settings(QString organizationName, QString appName, QObject *parent=nullptr);

    void load();

    // Flag
    bool onInit{true};

    // UI
    int maxVisibleItems{15};
    bool scheduleTableVisible{true};
    QString styleName{"default.qss"};
    int langId{1};

    // Shortcuts
    QString visibilityShortcut{"Ctrl+Shift+V"};
    QString macrosShortcut{"Ctrl+Shift+S"};
    QString killZoomShortcut{"Ctrl+Shift+Z"};

    // Other
    bool alwaysOnTop{true};
    bool showSplashScreen{true};
    bool autoZoomKillingOnExit{false};
    bool saveLastWindowState{true};
    QByteArray lastWindowRect;

signals:
    void settingsLoaded();

public slots:
    void saveSettings();

private:
    void loadSettings();
};

#endif // SETTINGS_H
