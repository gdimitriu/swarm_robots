#include "doorelement.h"

DoorElement::DoorElement(QObject *parent) : WallElement(parent)
{

}

Qt::PenStyle DoorElement::getPenStyle()
{
    return Qt::DotLine;
}


QDataStream &operator<<(QDataStream &out, const DoorElement &data)
{
    out<<data.length;
    out<<data.element;
    out<<data.vectorDim;
}

QDataStream &operator>>(QDataStream &in, DoorElement &data)
{
    in>>data.length;
    in>>data.element;
    in>>data.vectorDim;
}
