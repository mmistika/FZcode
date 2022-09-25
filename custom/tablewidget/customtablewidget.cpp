#include "customtablewidget.h"

#include <QContextMenuEvent>
#include <QWidgetAction>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>

CustomTableWidget::CustomTableWidget(QWidget *parent): QTableWidget(parent)
{
    initMenu();
    resizeHeaders();
}

CustomTableWidget::~CustomTableWidget(){}

QAction* createAction(QWidget *parent, QString text, QString iconResPath="")
{
    QAction *action = new QAction(text, parent);
    action->setIcon(QIcon(iconResPath));
    return action;
}

bool CustomTableWidget::isItemEmpty(int row, int column)
{
    QTableWidgetItem *item = this->item(row, column);
    if(item->text().isEmpty() or item->text().simplified().isEmpty()) return true;
    else return false;
}

void CustomTableWidget::initMenu()
{   
    contextMenu = new QMenu(this);

    QWidgetAction* title = new QWidgetAction(contextMenu);
    menuTitle = new QLabel("", contextMenu);
    menuTitle->setAlignment(Qt::AlignCenter);
    title->setDefaultWidget(menuTitle);
    contextMenu->addAction(title);

    contextMenu->addSeparator();

    QAction *action = createAction(this, tr("Insert after current"), "://icons/add.png");
    connect(action, &QAction::triggered, this, &CustomTableWidget::addRow);
    contextMenu->addAction(action);

    action = createAction(this, tr("Delete current"), "://icons/delete.png");
    connect(action, &QAction::triggered, this, &CustomTableWidget::deleteRow);
    contextMenu->addAction(action);

    contextMenu->addSeparator();

    action = createAction(this, tr("Move Up"), "://icons/upArrow.png");
    connect(action, &QAction::triggered, this, [this](){moveRow(true);});
    contextMenu->addAction(action);

    action = createAction(this, tr("Move Down"), "://icons/downArrow.png");
    connect(action, &QAction::triggered, this, [this](){moveRow(false);});
    contextMenu->addAction(action);

    contextMenu->addSeparator();

    action = createAction(this, tr("Delete all"));
    connect(action, &QAction::triggered, this, [this](){setRowCount(0);});
    contextMenu->addAction(action);
}

QPushButton* createBtn(QWidget *parent, QString iconResPath,
                       QSize iconSize=QSize(13, 13), QSize btnSize=QSize(25, 25))
{
    QPushButton *btn = new QPushButton(parent);
    btn->setMinimumSize(btnSize);
    btn->setIcon(QIcon(iconResPath));
    btn->setIconSize(iconSize);
    return btn;
}

void CustomTableWidget::addRow()
{
    int rowIndex = 0;
    if(!sender() or sender()->objectName().contains("plusBtn"))
    {
        if (!rowCount()) insertRow(rowIndex);
        else
        {
            rowIndex = rowCount();
            insertRow(rowCount());
        }
    }
    else if (!strcmp(sender()->metaObject()->className(), "QAction"))
    {
        rowIndex = currentRow()+1;
        insertRow(rowIndex);
    }
    else
    {
        QPoint cellWgtPos = qobject_cast<QWidget*>(sender()->parent())->pos();
        rowIndex = indexAt(cellWgtPos).row()+1;
        insertRow(rowIndex);
    }

    QWidget *wgt = new QWidget();
    QHBoxLayout* layout_ = new QHBoxLayout(wgt);
    layout_->setSpacing(2);
    layout_->setContentsMargins(3, 0, 3, 0);

    QPushButton *btn = createBtn(wgt, "://icons/add.png");
    connect(btn, &QPushButton::clicked, this, &CustomTableWidget::addRow);
    layout_->addWidget(btn);

    btn = createBtn(wgt, "://icons/upArrow.png");
    connect(btn, &QPushButton::clicked, this, [this](){moveRow(true);});
    layout_->addWidget(btn);

    btn = createBtn(wgt, "://icons/downArrow.png");
    connect(btn, &QPushButton::clicked, this, [this](){moveRow(false);});
    layout_->addWidget(btn);

    btn = createBtn(wgt, "://icons/delete.png");
    connect(btn, &QPushButton::clicked, this, &CustomTableWidget::deleteRow);
    layout_->addWidget(btn);

    setItem(rowIndex, 0, new QTableWidgetItem());
    setItem(rowIndex, 1, new QTableWidgetItem());
    setItem(rowIndex, 2, new QTableWidgetItem());
    setCellWidget(rowIndex, 3, wgt);
}


void CustomTableWidget::deleteRow()
{
    if (!strcmp(sender()->metaObject()->className(), "QAction"))
    {
        removeRow(currentRow());
    }
    else
    {
        QPoint cellWgtPos = qobject_cast<QWidget*>(sender()->parent())->pos();
        removeRow(indexAt(cellWgtPos).row());
        clearSelection();
    }
}


void CustomTableWidget::moveRow(bool up)
{
    int currRow;
    if (!strcmp(sender()->metaObject()->className(), "QAction")) currRow = currentRow();
    else
    {
        QPoint cellWgtPos = qobject_cast<QWidget*>(sender()->parent())->pos();
        currRow = indexAt(cellWgtPos).row();
    }

    // return if the first line is moving up or the last line is moving down
    if ((up and !currRow) or (!up and currRow == rowCount()-1)) return;

    int src = currRow;
    int dest = currRow + (up ? -1 : 1);

    for (int columnIndex = 0; columnIndex < 3; ++columnIndex)
    {
        QTableWidgetItem *buffItem = takeItem(src, columnIndex);
        setItem(src, columnIndex, takeItem(dest, columnIndex));
        setItem(dest, columnIndex, buffItem);
    }
}

void CustomTableWidget::resizeHeaders()
{
    // Fix too big headers at large scale
    this->horizontalHeader()->setDefaultSectionSize(this->horizontalHeader()->defaultSectionSize());
    this->verticalHeader()->setDefaultSectionSize(this->verticalHeader()->defaultSectionSize());
}

bool CustomTableWidget::event(QEvent *event)
{
    if(event->type() == QEvent::ContextMenu and rowCount())
    {
        menuTitle->setText(tr("Current row: ") + QString(currentRow() != -1 ? QString::number(currentRow()+1) : "-"));
        contextMenu->popup(static_cast<QContextMenuEvent*>(event)->globalPos());
    }
    return QTableWidget::event(event);
}

void CustomTableWidget::rowsInserted(const QModelIndex&, int, int)
{
    // ResizeMode for last section with control buttons
    if(!resizeModeSetted) this->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
}
