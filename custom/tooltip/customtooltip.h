#ifndef CUSTOMTOOLTIP_H
#define CUSTOMTOOLTIP_H

#include <QEvent>
#include <QLabel>

class CustomToolTip : QWidget
{
    Q_OBJECT

public:
    explicit CustomToolTip(QWidget *target, QString toolTipObjName=QString());
    virtual ~CustomToolTip();

    void showTip();
    void showTip(QPoint position);

private:
    bool eventFilter(QObject *target, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void init(QString name);

    QLabel *contentLabel;
    QWidget *targetWidget;

    bool firstShow = 1;
};

#endif // CUSTOMTOOLTIP_H
