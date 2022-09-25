#ifndef CUSTOMTABLEWIDGET_H
#define CUSTOMTABLEWIDGET_H

#include <QTableWidget>
#include <QLabel>
#include <QEvent>
#include <QMenu>

class CustomTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    CustomTableWidget(QWidget *parent=nullptr);
    ~CustomTableWidget();

    bool isItemEmpty(int row, int column);

public slots:
    void addRow();
    void deleteRow();
    void moveRow(bool up);

protected:
    bool event(QEvent *event) override;
    void rowsInserted(const QModelIndex & parent, int start, int end) override;

private:
    void resizeHeaders();
    void initMenu();

    bool resizeModeSetted{0};
    QPoint menuPos;

    QMenu *contextMenu;
    QLabel *menuTitle;
};

#endif // CUSTOMTABLEWIDGET_H
