QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    custom/combobox/customcombobox.cpp \
    custom/combobox/delegate/popupitemdelegate.cpp \
    custom/tablewidget/customtablewidget.cpp \
    custom/tooltip/customtooltip.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    tablesdata.cpp

HEADERS += \
    custom/combobox/customcombobox.h \
    custom/combobox/delegate/popupitemdelegate.h \
    custom/tablewidget/customtablewidget.h \
    custom/tooltip/customtooltip.h \
    mainwindow.h \
    settings.h \
    tablesdata.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    translations\FZcode_en_US.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

RC_ICONS = icon.ico
