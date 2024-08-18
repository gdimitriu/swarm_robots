#ifndef MAPELEMENT_H
#define MAPELEMENT_H

#include <Qt>
#include <QObject>
#include <QPainter>
#include <QPoint>
#include <QDataStream>

class MapElement : public QObject
{
    Q_OBJECT
public:
    MapElement(QObject *parent = nullptr);
    virtual ~MapElement();
    virtual void draw(QPainter * painter) = 0;
    virtual bool isInside(QPoint point) = 0;
    virtual void setLength(double value);
    virtual double getLength();
    virtual void read(QDataStream &in);
    virtual void write(QDataStream &out);
    virtual QString getType()
    {
        return QString("MapElement");
    }
    friend QDataStream &operator<<(QDataStream &, const MapElement &);
    friend QDataStream &operator>>(QDataStream &, MapElement &);
signals:
protected:
    double length;

};

QDataStream &operator<<(QDataStream &, const MapElement &);
QDataStream &operator>>(QDataStream &, MapElement &);


#endif // MAPELEMENT_H
