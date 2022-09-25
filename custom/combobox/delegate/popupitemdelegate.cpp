#include "popupitemdelegate.h"

PopupItemDelegate::PopupItemDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{}

QSize PopupItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize sz(QStyledItemDelegate::sizeHint(option, index));
    sz.setHeight(25);

    return sz;
}
