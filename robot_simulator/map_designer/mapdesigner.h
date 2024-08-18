#ifndef MAPDESIGNER_H
#define MAPDESIGNER_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QLinkedList>
#include <QToolBar>
#include "mapelement.h"

namespace Ui {
class MapDesigner;
}

class MapDesigner : public QMainWindow
{
    Q_OBJECT
public slots:
    void paintEvent(QPaintEvent * event);
private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    bool exportAs();
    void addWall();
    void addObject();
    void addDoor();
    void editElement();
    void deleteElement();
public:
    explicit MapDesigner(QWidget *parent = 0);
    ~MapDesigner();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    void createActions();
    void createMenus();

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exportAsAction;
    QToolBar *toolBar;
    QMenu *fileMenu;

    QAction *addWallAction;
    QAction *addObjectAction;
    QAction *addDoorAction;
    QAction *addEditAction;
    QAction *deleteAction;

    bool isTracking;
    enum class Operation { NONE, WALL, DOOR, OBJECT, EDIT , DELETE} currentOperation;

    Ui::MapDesigner *ui;

    Qt::MouseButton DrawingButton;
    QLinkedList<MapElement *> *mapElements;
    MapElement *currentElement;
    QString fileName;
};

#endif // MAPDESIGNER_H
