#include "mapdesigner.h"
#include "ui_mapdesigner.h"
#include <QFileDialog>
#include <QPainter>
#include <QRect>
#include <QSignalMapper>
#include <QFile>
#include <QDataStream>
#include "wallelement.h"
#include "doorelement.h"
#include "editproperty.h"
#include "elementfactory.h"


MapDesigner::MapDesigner(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MapDesigner)
{
    ui->setupUi(this);
    createActions();
    createMenus();
    setAttribute(Qt::WA_DeleteOnClose);
    isTracking = false;
    mapElements = new QLinkedList<MapElement *>();
    currentElement = nullptr;
    currentOperation = Operation::NONE;
    DrawingButton = Qt::RightButton;
}

MapDesigner::~MapDesigner()
{
    delete ui;
    QLinkedList<MapElement *>::iterator it;
    for ( it = mapElements->begin(); it != mapElements->end(); it++ )
    {
        delete *it;
    }
    delete mapElements;
}

void MapDesigner::createActions()
{
    newAction = new QAction(tr("&New"), this);
    newAction->setShortcut(tr("Ctrl+N"));
    newAction->setStatusTip(tr("Create a new map"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openAction = new QAction(tr("&Open"), this);
    openAction->setShortcut(tr("Ctrl+O"));
    openAction->setStatusTip(tr("Open a map"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(tr("Ctrl+S"));
    saveAction->setStatusTip(tr("Save map"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction(tr("Save &as"), this);
    saveAsAction->setStatusTip(tr("Save as map"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    exportAsAction = new QAction(tr("&Export"), this);
    exportAsAction->setStatusTip(tr("Export for use in robot simulator"));
    connect(exportAsAction, SIGNAL(triggered()), this, SLOT(exportAs()));

    addWallAction = new QAction(tr("&Wall"), this);
    addWallAction->setStatusTip(tr("Add Wall to map"));
    connect(addWallAction, SIGNAL(triggered()), this, SLOT(addWall()));

    addObjectAction = new QAction(tr("&Object"), this);
    addObjectAction->setStatusTip(tr("Add Object to map"));
    connect(addObjectAction, SIGNAL(triggered()), this, SLOT(addObject()));

    addDoorAction = new QAction(tr("&Door"), this);
    addDoorAction->setStatusTip(tr("Add Door to map"));
    connect(addDoorAction, SIGNAL(triggered()), this, SLOT(addDoor()));

    addEditAction = new QAction(tr("&Edit"), this);
    addDoorAction->setStatusTip(tr("Edit an element from map"));
    connect(addEditAction, SIGNAL(triggered()), this, SLOT(editElement()));

    deleteAction = new QAction(tr("Dele&te"), this);
    deleteAction->setStatusTip(tr("Delete an element from map"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteElement()));

}

void MapDesigner::createMenus()
{
    ui->toolBar->addAction(addWallAction);
    ui->toolBar->addAction(addObjectAction);
    ui->toolBar->addAction(addDoorAction);
    ui->toolBar->addAction(addEditAction);
    ui->toolBar->addAction(deleteAction);
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(exportAsAction);
}

void MapDesigner::newFile()
{
    QLinkedList<MapElement *>::iterator it;
    for ( it = mapElements->begin(); it != mapElements->end(); it++ )
    {
        delete *it;
    }
    mapElements->clear();
    currentElement = nullptr;
    isTracking = false;
    currentOperation = Operation::NONE;
    update();
}

void MapDesigner::open()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open Map"), ".", tr("map files (*.map)"));
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    mapElements->clear();
    int sz;
    in>>sz;
    for ( int i = 0; i < sz; ++i )
    {
        QString type;
        in>>type;
        MapElement *el = ElementFactory::create(type, this);
        in>>*el;
        mapElements->append(el);
    }
    update();
}

bool MapDesigner::save()
{
    if ( fileName.isEmpty() )
        return saveAs();
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out<<mapElements->size();
    QLinkedList<MapElement *>::iterator it;
    for ( it = mapElements->begin(); it != mapElements->end(); ++it )
    {
        out<<(*it)->getType();
        out<<**it;
    }
    return true;
}

bool MapDesigner::saveAs()
{
    fileName = QFileDialog::getSaveFileName(this, tr("Save as ...  Map"), ".", tr("map files (*.map)"));
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out<<mapElements->size();
    QLinkedList<MapElement *>::iterator it;
    for ( it = mapElements->begin(); it != mapElements->end(); ++it )
    {
        out<<(*it)->getType();
        out<<**it;
    }
    return true;
}


bool MapDesigner::exportAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export as ... "), ".", tr("dat files (*.dat)"));
}

void MapDesigner::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    QRect rect = event->rect();
    painter.eraseRect(rect);

    QLinkedList<MapElement *>::iterator itElements;
    for ( itElements = mapElements->begin(); itElements != mapElements->end(); itElements++ )
    {
        (*itElements)->draw(&painter);
    }
}

void MapDesigner::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() != DrawingButton )
    {
        isTracking = false;
        return;
    }
    QLinkedList<MapElement *>::iterator it;
    switch ( currentOperation )
    {
    case  Operation::WALL:
        isTracking = true;
        currentElement = new WallElement(this);
        dynamic_cast<WallElement *>(currentElement)->addStartPoint(event->pos());
        mapElements->append(currentElement);
        event->accept();
        break;
    case  Operation::DOOR:
        isTracking = true;
        currentElement = new DoorElement(this);
        dynamic_cast<DoorElement *>(currentElement)->addStartPoint(event->pos());
        mapElements->append(currentElement);
        event->accept();
        break;
    case Operation::EDIT:
        for ( it = mapElements->begin(); it != mapElements->end(); ++it )
        {
            if ( (*it)->isInside(event->pos()) )
            {
                bool ok;
                do
                {
                    EditProperty size("Edit Lenght", "Length", this);
                    size.setToEditProperty(QString::number((*it)->getLength()));
                    size.exec();
                    if ( size.isOk() )
                    {
                        double sz = size.getEditedProperty().toDouble(&ok);
                        if ( ok )
                            (*it)->setLength(sz);
                    }
                } while ( !ok );
                break;
            }
        }
        currentOperation = Operation::NONE;
        event->accept();
        break;
    case Operation::DELETE:
        for ( it = mapElements->begin(); it != mapElements->end(); ++it )
        {
            if ( (*it)->isInside(event->pos()) )
            {
                mapElements->erase(it);
                break;
            }
        }
        currentOperation = Operation::NONE;
        event->accept();
        break;
    }
    update();
}

void MapDesigner::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->button() == DrawingButton && isTracking )
    {
        isTracking = false;
        if ( currentElement != nullptr) {
            switch (currentOperation)
            {
            case Operation::DOOR:
            case Operation::WALL:
                WallElement *element = dynamic_cast<WallElement *>(currentElement);
                element->addEndPoint(event->pos());
                currentElement = nullptr;
                bool ok;
                do
                {
                    EditProperty size("Add Lenght", "Length", this);
                    size.exec();
                    if ( size.isOk() )
                    {
                        double sz = size.getEditedProperty().toDouble(&ok);
                        if ( ok )
                            element->setLength(sz);
                    }
                } while ( !ok );
                break;
            }
        }
        currentOperation = Operation::NONE;
        update();
    }
}

void MapDesigner::mouseMoveEvent(QMouseEvent *event)
{
    if ( isTracking )
    {
        if ( currentElement != nullptr) {
            switch (currentOperation)
            {
            case Operation::DOOR:
            case Operation::WALL:
                WallElement *element = dynamic_cast<WallElement *>(currentElement);
                element->addEndPoint(event->pos());
                break;
            }
        }
        update();
    }
}

void MapDesigner::addWall()
{
    currentOperation = Operation::WALL;
}

void MapDesigner::addObject()
{
    currentOperation = Operation::OBJECT;
}

void MapDesigner::addDoor()
{
    currentOperation = Operation::DOOR;
}

void MapDesigner::editElement()
{
    currentOperation = Operation::EDIT;
}

void MapDesigner::deleteElement()
{
    currentOperation = Operation::DELETE;
}
