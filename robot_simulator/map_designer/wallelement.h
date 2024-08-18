#ifndef WALLELEMENT_H
#define WALLELEMENT_H

#include "mapelement.h"
#include <QtCore>
#include <QDataStream>

class WallElement : public MapElement
{
   Q_OBJECT
public:
    WallElement(QObject *parent = nullptr);
    void draw(QPainter * painter);
    bool isInside(QPoint point);
    void addStartPoint(QPoint point);
    void addEndPoint(QPoint point);
    virtual QString getType()
    {
        return QString("WallElement");
    }
    virtual void read(QDataStream &in);
    virtual void write(QDataStream &out);
    friend QDataStream &operator<<(QDataStream &, const WallElement &);
    friend QDataStream &operator>>(QDataStream &, WallElement &);
protected:
    virtual Qt::PenStyle getPenStyle();
    QLine element;
    QLine vectorDim;
};

QDataStream &operator<<(QDataStream &, const WallElement &);
QDataStream &operator>>(QDataStream &, WallElement &);

#endif // WALLELEMENT_H
