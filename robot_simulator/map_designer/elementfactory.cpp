#include "elementfactory.h"

MapElement *ElementFactory::create(QString type, QObject *parent)
{
    if ( type.compare(QString("WallElement")) == 0 )
    {
        return new WallElement(parent);
    }
    else if ( type.compare(QString("DoorElement")) == 0 )
    {
        return new DoorElement(parent);
    }
    return nullptr;
}
