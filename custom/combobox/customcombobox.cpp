#include "customcombobox.h"

#include <QListView>

CustomComboBox::CustomComboBox(QWidget *parent): QComboBox(parent)
{
    view()->setItemDelegate(new PopupItemDelegate(this));
    view()->parentWidget()->setWindowFlag(Qt::FramelessWindowHint);
    view()->parentWidget()->setAttribute(Qt::WA_TranslucentBackground);
}


void CustomComboBox::showPopup()
{
    if(count())
    {
        setStyleSheet("#homeComboBox{border-bottom:none;border-bottom-right-radius:0;border-bottom-left-radius:0;}");
        QComboBox::showPopup();
    }
}

void CustomComboBox::hidePopup()
{
    setStyleSheet("");
    QComboBox::hidePopup();
}
