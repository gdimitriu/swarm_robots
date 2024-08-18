#include "mapelement.h"

MapElement::MapElement(QObject *parent) : QObject(parent), length(0.0)
{

}

MapElement::~MapElement()
{

}

void MapElement::setLength(double value)
{
    length = value;
}

double MapElement::getLength()
{
    return length;
}

QDataStream &operator<<(QDataStream &out, const MapElement &data)
{
    const_cast<MapElement &>(data).write(out);
    return out;
}
QDataStream &operator>>(QDataStream &in, MapElement &data)
{
    data.read(in);
    return in;
}

void MapElement::read(QDataStream &in)
{
    in>>length;
}
void MapElement::write(QDataStream& out)
{
    out<<length;
}
