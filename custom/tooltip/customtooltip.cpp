#include "customtooltip.h"

#include <QHBoxLayout>
#include <QHelpEvent>
#include <QTabBar>


bool isTabBar(QWidget *widget)
{
    return widget->metaObject()->className() == QString("QTabBar");
}

CustomToolTip::CustomToolTip(QWidget *target, QString toolTipObjName)
    : QWidget(nullptr)
    , contentLabel(new QLabel(this))
    , targetWidget(target)
{
    init(toolTipObjName);

    if (!isTabBar(target))
    {
        contentLabel->setText(target->toolTip());
    }
}

void CustomToolTip::init(QString name)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setVisible(false);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(contentLabel);

    contentLabel->setAlignment(Qt::AlignCenter);

    QString objName = (name.isEmpty()) ? targetWidget->objectName()+QString("_toolTip") : name;
    setObjectName(objName);

    targetWidget->installEventFilter(this);
    targetWidget->setMouseTracking(true);
    connect(targetWidget, &QObject::destroyed, this, &QObject::deleteLater);
}

CustomToolTip::~CustomToolTip()
{
   delete contentLabel;
}

void CustomToolTip::showTip()
{
    if (firstShow)
    {
        firstShow = 0;
        return;
    }
    if (isTabBar(targetWidget))
    {
        QTabBar *bar = qobject_cast<QTabBar*>(targetWidget);

        int buttonIndex = bar->tabAt(bar->mapFromGlobal(QCursor::pos()));
        contentLabel->setText(bar->tabToolTip(buttonIndex));
    }
    if (!contentLabel->text().isEmpty()) showTip(QCursor::pos());
}

void CustomToolTip::showTip(QPoint position)
{
    move(position.x(), position.y()-this->height());
    show();
}

bool CustomToolTip::eventFilter(QObject *target, QEvent *event)
{
    if(target==targetWidget){
        switch (event->type()) {
        case QEvent::ToolTip:
            showTip();
            return true;
        case QEvent::Leave:
            hide();
            break;
        default:
            break;
        }
    }
    return QObject::eventFilter(target,event);
}

void CustomToolTip::resizeEvent(QResizeEvent *event)
{
    showTip();
    return QWidget::resizeEvent(event);
}
