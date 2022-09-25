#ifndef CUSTOMCOMBOBOX_H
#define CUSTOMCOMBOBOX_H

#include <QComboBox>

#include "delegate/popupitemdelegate.h"

class CustomComboBox : public QComboBox
{
    Q_OBJECT
public:
    CustomComboBox(QWidget *parent=nullptr);

    void showPopup() override;
    void hidePopup() override;
};

#endif // CUSTOMCOMBOBOX_H
