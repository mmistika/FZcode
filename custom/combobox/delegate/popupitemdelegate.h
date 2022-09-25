#ifndef POPUPITEMDELEGATE_H
#define POPUPITEMDELEGATE_H

#include <QStyledItemDelegate>

class PopupItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    PopupItemDelegate(QWidget *parent=nullptr);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // POPUPITEMDELEGATE_H
