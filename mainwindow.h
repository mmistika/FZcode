#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <windows.h>
#include <windowsx.h>

#include <QMainWindow>
#include <QClipboard>
#include <QTranslator>

#include "settings.h"
#include "tablesdata.h"
#include "custom/tooltip/customtooltip.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    HWND hwnd;
    Ui::MainWindow *ui;
    Settings *settings;
    TablesData *tablesData;
    QClipboard *clipboard;
    QTranslator *translator;

    int currentDayIndex; // [0 - Monday ... 6 - Sunday]

    const int m_minWidth{526};
    const int m_minWidthNoTable{280};

    const int m_homeBtnsContainerMaxHeight{65};
    const int m_homeBtnsContainerMaxHeightOneColumn{125};

    QList<CustomToolTip*> m_toolTips;

public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:
    void settingsChanged();
    void tablesChanged();

public slots:
    void applySettings();
    void applyTablesData();

private slots:
    void saveSettings();
    void changeOnTopRule(bool state);
    void saveTables();
    void changeScheduleTableContent(int dayIndex);

private:
    void setupToolTips();
    void fillHomeComboBox();
    void fillHomeScheduleTable();

    enum class CopyType { Id, Password };
    void copy(CopyType type);
    void logCopy(CopyType type, QString toCopyText);

    enum class TableType { Meetings, Schedules };
    bool isTablesValid(TableType table);
    void tablesWarnMsg(TableType table);

    void changeLang();
    QString getSelectedLangCode();
    void setRadioButtons();

    enum class LayoutType { OneColumn, TwoColumns };
    void changeHomeButtonsLayout(LayoutType type);

    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
};

#endif // MAINWINDOW_H
