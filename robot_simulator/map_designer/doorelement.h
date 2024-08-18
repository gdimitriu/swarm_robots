#ifndef DOORELEMENT_H
#define DOORELEMENT_H

#include "wallelement.h"

class DoorElement : public WallElement
{
    Q_OBJECT
public:
    DoorElement(QObject *parent = nullptr);
    virtual QString getType()
    {
        return QString("DoorElement");
    }
    friend QDataStream &operator<<(QDataStream &, const DoorElement &);
    friend QDataStream &operator>>(QDataStream &, DoorElement &);
protected:
    virtual Qt::PenStyle getPenStyle();
};

QDataStream &operator<<(QDataStream &, const DoorElement &);
QDataStream &operator>>(QDataStream &, DoorElement &);

#endif // DOORELEMENT_H
