#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDate>
#include <QCloseEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      hwnd(HWND(winId())),
      ui(new Ui::MainWindow),
      settings(new Settings("Mistika Software", "FZcode")),
      tablesData(new TablesData("tables.json")),
      clipboard(qApp->clipboard()),
      translator(new QTranslator()),
      currentDayIndex(QDate::currentDate().dayOfWeek()-1)

{
    // Needed for nativeEvent()
    ::SetWindowLongPtr(hwnd, GWL_STYLE, WS_CAPTION | WS_THICKFRAME);

    // Styles must be applied directly to the qApp,
    // otherwise the CustomToolTip styles won't work.
    QFile styleFile(QString(":/styles/defaultDark.qss"));
    styleFile.open(QFile::ReadOnly | QFile::Text);
    qApp->setStyleSheet(QString::fromLatin1(styleFile.readAll()));
    styleFile.close();

    ui->setupUi(this);
    ui->daysComboBox->setCurrentIndex(currentDayIndex);
    ui->homeTable->setColumnWidth(2, 85);
    ui->macrosBtn->setVisible(false);
    ui->killBtn->setVisible(false);
    ui->shortcutsGroup->setVisible(false);

    setupToolTips();

    // Home buttons connections
    connect(ui->idBtn, &QPushButton::clicked, this, [this](){copy(CopyType::Id);});
    connect(ui->passwdBtn, &QPushButton::clicked, this, [this](){copy(CopyType::Password);});

    // Tables connections
    connect(ui->plusBtn, &QPushButton::clicked, ui->meetingsTableWidget, &CustomTableWidget::addRow);
    connect(ui->plusBtn_2, &QPushButton::clicked, ui->scheduleTableWidget, &CustomTableWidget::addRow);
    connect(ui->saveBtn, &QPushButton::clicked, this, &MainWindow::saveTables);
    connect(ui->saveBtn_2, &QPushButton::clicked, this, &MainWindow::saveTables);
    connect(ui->daysComboBox, QOverload<int>::of(&CustomComboBox::currentIndexChanged), this, &MainWindow::changeScheduleTableContent);
    connect(tablesData, &TablesData::dataLoaded, this, &MainWindow::applyTablesData);
    connect(this, &MainWindow::tablesChanged, tablesData, &TablesData::saveData);

    // Settings conections
    connect(settings, &Settings::settingsLoaded, this, &MainWindow::applySettings);
    connect(this, &MainWindow::settingsChanged, settings, &Settings::saveSettings);

    /* Settings tab connections */
    // --> MaxVisibleComboItems
    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [this](int value)
    {
        ui->homeComboBox->setMaxVisibleItems(value);
        if(!settings->onInit) saveSettings();
    });
    // --> Show schedule table
    connect(ui->showTableCheckBox, &QCheckBox::toggled, this,
            [this](bool state)
    {
        setMinimumWidth(state ? m_minWidth : m_minWidthNoTable);
        ui->homeTable->setVisible(state);
        if(!settings->onInit) saveSettings();
    });

    // --> Always on top
    connect(ui->alwaysOnTopCheckBox, &QCheckBox::toggled, this, &MainWindow::changeOnTopRule);
    // --> Save last window state
    connect(ui->saveStateCheckBox, &QCheckBox::toggled, this, [this](){if(!settings->onInit) saveSettings();});

    // --> Languages
    connect(ui->enRadioButton, &QRadioButton::clicked, this, &MainWindow::changeLang);
    connect(ui->uaRadioButton, &QRadioButton::clicked, this, &MainWindow::changeLang);
    connect(ui->ruRadioButton, &QRadioButton::clicked, this, &MainWindow::changeLang);

    settings->load();
    tablesData->load();
}

void MainWindow::setupToolTips()
{   
    QWidgetList list = {ui->tabWidget->tabBar(), ui->tablesTabWidget->tabBar(),
                        ui->idBtn, ui->passwdBtn, ui->macrosBtn,ui->killBtn,
                        ui->closeBtn, ui->minimizeBtn,
                        ui->plusBtn, ui->plusBtn_2};

    for (QWidget *w : list)
    {
        CustomToolTip *toolTip = new CustomToolTip(w);
        m_toolTips.append(toolTip);
    }
}

// SLOTS

void MainWindow::copy(CopyType type)
{
    int index = ui->homeComboBox->currentIndex();
    if(index >= 0 and tablesData->meetings.count() >= index)
    {
        Meeting *meetingStruct = &tablesData->meetings[index];
        QString toCopyText = type == CopyType::Id ? meetingStruct->id : meetingStruct->password;
        clipboard->setText(toCopyText);
        logCopy(type, toCopyText);
    }
}

void MainWindow::logCopy(CopyType type, QString toCopyText)
{
    QString copyType = type == CopyType::Id ? "ID" : tr("Password");
    QString logText = copyType + tr(" copied:\n") + toCopyText;
    ui->logLabel->setText(logText);
}

void MainWindow::saveSettings()
{
    // UI
    settings->maxVisibleItems = ui->spinBox->value();
    settings->scheduleTableVisible = ui->showTableCheckBox->isChecked();
    settings->langCode = getSelectedLangCode();

    // Other
    settings->alwaysOnTop = ui->alwaysOnTopCheckBox->isChecked();
    settings->saveLastWindowState = ui->saveStateCheckBox->isChecked();
    if(settings->saveLastWindowState) settings->lastWindowRect = saveGeometry();

    emit settingsChanged();
}

void MainWindow::applySettings()
{
    // UI
    ui->spinBox->setValue(settings->maxVisibleItems);
    emit ui->spinBox->valueChanged(settings->maxVisibleItems);
    ui->showTableCheckBox->setChecked(settings->scheduleTableVisible);
    emit ui->showTableCheckBox->toggled(settings->scheduleTableVisible);
    setRadioButtons();
    changeLang();

    // Other
    ui->alwaysOnTopCheckBox->setChecked(settings->alwaysOnTop);
    emit ui->alwaysOnTopCheckBox->toggled(settings->alwaysOnTop);
    ui->saveStateCheckBox->setChecked(settings->saveLastWindowState);
    emit ui->saveStateCheckBox->toggled(settings->saveLastWindowState);
    if(settings->saveLastWindowState) restoreGeometry(settings->lastWindowRect);

    settings->onInit = false;
}

void MainWindow::setRadioButtons()
{
    if(settings->langCode == "en") ui->enRadioButton->setChecked(true);
    else if(settings->langCode == "ua") ui->uaRadioButton->setChecked(true);
    else if(settings->langCode == "ru") ui->ruRadioButton->setChecked(true);
}

void MainWindow::changeOnTopRule(bool state)
{
    HWND rule = state ? HWND_TOPMOST : HWND_NOTOPMOST;
    ::SetWindowPos(hwnd, rule, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    if(!settings->onInit) saveSettings();
}

void MainWindow::saveTables()
{
    int rowCount = ui->meetingsTableWidget->rowCount();
    if(rowCount)
    {
        if(!isTablesValid(TableType::Meetings))
        {
            tablesWarnMsg(TableType::Meetings);
            return;
        }
        tablesData->meetings.resize(rowCount);

        for(int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
        {
            Meeting meetingStruct{ui->meetingsTableWidget->item(rowIndex, 0)->text(),
                                  ui->meetingsTableWidget->item(rowIndex, 1)->text(),
                                  ui->meetingsTableWidget->item(rowIndex, 2)->text()};
            tablesData->meetings[rowIndex] = meetingStruct;
        }
    }
    else {tablesData->meetings.clear();}

    fillHomeComboBox();

    rowCount = ui->scheduleTableWidget->rowCount();
    int dayIndex = ui->daysComboBox->currentIndex();
    if(rowCount)
    {
        if(!isTablesValid(TableType::Schedules))
        {
            tablesWarnMsg(TableType::Schedules);
            return;
        }
        QVector<Schedule> day;
        for(int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
        {
            Schedule scheduleStruct{ui->scheduleTableWidget->item(rowIndex, 0)->text(),
                                   ui->scheduleTableWidget->item(rowIndex, 1)->text(),
                                   ui->scheduleTableWidget->item(rowIndex, 2)->text()};
            day.append(scheduleStruct);
        }

        tablesData->schedule[dayIndex] = day;
    }
    else {tablesData->schedule[dayIndex].clear();}

    fillHomeScheduleTable();

    emit tablesChanged();
}

void MainWindow::applyTablesData()
{
    ui->meetingsTableWidget->setRowCount(0);
    for(int index = 0; index < tablesData->meetings.count(); ++index)
    {
        Meeting *meetingStruct = &tablesData->meetings[index];
        ui->meetingsTableWidget->addRow();
        ui->meetingsTableWidget->item(index, 0)->setText(meetingStruct->name);
        ui->meetingsTableWidget->item(index, 1)->setText(meetingStruct->id);
        ui->meetingsTableWidget->item(index, 2)->setText(meetingStruct->password);
    }
    fillHomeComboBox();

    ui->daysComboBox->setCurrentIndex(currentDayIndex);
    changeScheduleTableContent(currentDayIndex);
    fillHomeScheduleTable();
}

bool MainWindow::isTablesValid(TableType table)
{
    if(table == TableType::Meetings)
    {
        for(int rowIndex = 0; rowIndex < ui->meetingsTableWidget->rowCount(); ++rowIndex)
        {
            if(!ui->meetingsTableWidget->isItemEmpty(rowIndex, 0) and
               !ui->meetingsTableWidget->isItemEmpty(rowIndex, 1) and
               !ui->meetingsTableWidget->isItemEmpty(rowIndex, 2))
            {
                QString id = ui->meetingsTableWidget->item(rowIndex, 1)->text();
                if(id.toLongLong() <= 0 or (id.length() < 9 or id.length() > 11)) return false;
            }
            else return false;
        }
    }
    else if(table == TableType::Schedules)
    {
        for(int rowIndex = 0; rowIndex < ui->scheduleTableWidget->rowCount(); ++rowIndex)
        {
            if(ui->scheduleTableWidget->isItemEmpty(rowIndex, 0) or
               ui->scheduleTableWidget->isItemEmpty(rowIndex, 1) or
               ui->scheduleTableWidget->isItemEmpty(rowIndex, 2)) return false;
        }
    }

    return true;
}

void MainWindow::changeScheduleTableContent(int dayIndex)
{
    ui->scheduleTableWidget->setRowCount(0);

    QVector<Schedule> *day = &tablesData->schedule[dayIndex];
    for(int index = 0; index < day->count(); ++index)
    {
        Schedule scheduleStruct = (*day)[index];
        ui->scheduleTableWidget->addRow();
        ui->scheduleTableWidget->item(index, 0)->setText(scheduleStruct.name);
        ui->scheduleTableWidget->item(index, 1)->setText(scheduleStruct.start);
        ui->scheduleTableWidget->item(index, 2)->setText(scheduleStruct.duration);
    }
}

void MainWindow::fillHomeComboBox()
{
    ui->homeComboBox->clear();
    foreach(Meeting meetingStruct, tablesData->meetings)
    {
        ui->homeComboBox->addItem(meetingStruct.name);
    }
}

void MainWindow::fillHomeScheduleTable()
{
    QVector<Schedule> *day = &tablesData->schedule[currentDayIndex];
    ui->homeTable->setRowCount(day->count());

    for(int index = 0; index < day->count(); ++index)
    {
        Schedule scheduleStruct = (*day)[index];
        ui->homeTable->setItem(index, 0, new QTableWidgetItem(scheduleStruct.name));
        ui->homeTable->setItem(index, 1, new QTableWidgetItem(scheduleStruct.start));
        ui->homeTable->setItem(index, 2, new QTableWidgetItem(scheduleStruct.duration));
    }
}

void MainWindow::tablesWarnMsg(TableType table)
{
    QStringList conditionList{tr(" - there are no empty cells\n"),
                              tr(" - only numbers are present in the id column\n"),
                              tr(" - conference ID is valid\n")};
    QString tableName = table == TableType::Meetings ? tr("meeting table!") : tr("schedule table!");
    QString title = tr("Invalid data in ") + tableName;
    QString conditions = table == TableType::Meetings ? conditionList[1] + conditionList[2] : "";
    QString msgText = title + tr("\n\nAn error occurred while reading data from the table,\n"
                      "make sure that all conditions are met:\n") + conditionList[0] + conditions;

    QMessageBox msgBox(QMessageBox::Warning,
                       title,
                       msgText, QMessageBox::NoButton,
                       this, Qt::Dialog | Qt::FramelessWindowHint);

    QPushButton *restoreBtn = msgBox.addButton(tr("Restore last save"), QMessageBox::ResetRole);
    msgBox.setEscapeButton(restoreBtn);
    QPushButton *continueBtn = msgBox.addButton(tr("Continue editing"), QMessageBox::AcceptRole);
    msgBox.setDefaultButton(continueBtn);
    msgBox.exec();

    if(msgBox.clickedButton() == restoreBtn)
    {
        applyTablesData();
    }
}

QString MainWindow::getSelectedLangCode()
{
    if(ui->enRadioButton->isChecked()) return "en";
    else if(ui->uaRadioButton->isChecked()) return "ua";
    else if(ui->ruRadioButton->isChecked()) return "ru";
    else return "en";
}

void setTranslator(QTranslator* translator, QString filename)
{
    qApp->removeTranslator(translator);
    QString path = QApplication::applicationDirPath();
    path.append("/translations/");
    if(translator->load(path + filename))
    qApp->installTranslator(translator);
}

void MainWindow::changeLang()
{
    QString lang;
    if(sender() and !settings->onInit)
    {
        lang = getSelectedLangCode();
        if(lang == settings->langCode) return;
        else saveSettings();
    }
    else lang = settings->langCode;

    setTranslator(translator, QString("FZcode_%1").arg(lang));
    changeHomeButtonsLayout(lang == "en" ? LayoutType::TwoColumns : LayoutType::OneColumn);
}

void MainWindow::changeHomeButtonsLayout(LayoutType type)
{
    QGridLayout *layout = qobject_cast<QGridLayout*>(ui->homeBtnsContainer->layout());

    QLayoutItem* btns[4];
    for(int btnIndex = 0; btnIndex < 4; ++btnIndex)
    {
        btns[btnIndex] = layout->takeAt(0);
    }

    if(type == LayoutType::OneColumn)
    {
        for(int btnIndex = 0; btnIndex < 4; ++btnIndex)
        {
            layout->addItem(btns[btnIndex], btnIndex, 0);
        }
    }
    else
    {
        layout->addItem(btns[0], 0, 0);
        layout->addItem(btns[1], 0, 1);
        layout->addItem(btns[2], 1, 0);
        layout->addItem(btns[3], 1, 1);
    }
}

// EVENTS

void MainWindow::closeEvent(QCloseEvent*)
{
    saveSettings();
    tablesData->saveData();
}

void MainWindow::changeEvent(QEvent* event)
{
    if(event->type() == QEvent::LanguageChange) ui->retranslateUi(this);
    else QMainWindow::changeEvent(event);
}

bool MainWindow::nativeEvent(const QByteArray&, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);

    switch (msg->message)
    {
    case WM_NCCALCSIZE:
    {
        //remove WS_THICKFRAME and WS_CAPTION
        if(msg->wParam)
        {
            *result = 0;
            return true;
        }
        return false;
    }
    case WM_NCHITTEST:
    {
        int borderWidth{4};

        QPoint globalPos(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));        
        QPoint localPos = ui->titleBar->mapFromGlobal(globalPos);

        RECT rect;
        GetWindowRect(hwnd, &rect);

        if(!(localPos.x() >= ui->minimizeBtn->pos().x() and
           localPos.y() <= ui->closeBtn->height()))
        {
            // Borders
            if (abs(rect.left - globalPos.x()) <= borderWidth) *result = HTLEFT;
            if (rect.right - globalPos.x() <= borderWidth) *result = HTRIGHT;
            if (abs(rect.top - globalPos.y()) <= borderWidth) *result = HTTOP;
            if (rect.bottom - globalPos.y() <= borderWidth) *result = HTBOTTOM;

            // Corners
            if (abs(rect.left - globalPos.x()) <= borderWidth and
                    rect.bottom - globalPos.y() <= borderWidth) *result = HTBOTTOMLEFT;
            if (rect.right - globalPos.x() <= borderWidth and
                    rect.bottom - globalPos.y() <= borderWidth) *result = HTBOTTOMRIGHT;
            if (abs(rect.left - globalPos.x()) <= borderWidth and
                    abs(rect.top - globalPos.y()) <= borderWidth) *result = HTTOPLEFT;
        }

        if (*result) return true;

        if (ui->titleBar->rect().contains(localPos) and
            localPos.x() < ui->minimizeBtn->pos().x())
        {
            *result = HTCAPTION;
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

