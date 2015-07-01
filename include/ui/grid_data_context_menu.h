#ifndef GRID_DATA_CONTEXT_MENU_H
#define GRID_DATA_CONTEXT_MENU_H

#include <QWidget>
#include <QMenu>

class GridDataContextMenu : public QMenu {
    Q_OBJECT
public:
    explicit GridDataContextMenu(QWidget *parent = 0) {
        addAction("Edit selected cells"); // QIcon
    }
};

#endif // GRID_DATA_CONTEXT_MENU_H
