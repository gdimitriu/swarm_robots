#include "wallelement.h"

WallElement::WallElement(QObject *parent) : MapElement(parent)
{

}

Qt::PenStyle WallElement::getPenStyle()
{
    return Qt::SolidLine;
}

void WallElement::draw(QPainter * painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(Qt::black, 15, getPenStyle(), Qt::FlatCap));
    painter->drawLine(element);

    painter->setPen(QPen(Qt::black, 2, Qt::DashLine, Qt::RoundCap));
    painter->drawLine(vectorDim);

    if ( abs(element.dx()) < 10 && abs(element.dy()) > 10 )
    {
        painter->save();
        painter->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap));
        QPoint center = vectorDim.center();
        painter->translate(center);
        painter->rotate(-90);
        painter->drawText(0,0, QString::number(length));
        auto x = center.x();
        center.setX(-center.y());
        center.setY(-x);
        painter->translate(center);
        painter->restore();
    } else if ( abs(element.dx()) > 10 )
    {
        painter->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap));
        painter->drawText(vectorDim.center(),QString::number(length));
    }
}

bool WallElement::isInside(QPoint point)
{
    //horizontal element
    if ( element.y1() == element.y2() && element.x1() != element.x2() )
    {
        if ( element.x1() < element.x2() )
        {
            //from left to right
            if ( element.x1() > point.x() || element.x2() < point.x() )
                return false;
            else if ( (element.y1() + 7) < point.y() || (element.y1() - 7) > point.y() )
                return false;
            else if ( (element.y2() + 7) < point.y() || (element.y2() - 7) > point.y() )
               return false;
        } else
        {
            //from right to left
            if ( element.x1() < point.x() || element.x2() > point.x() )
                return false;
            else if ( (element.y1() + 7) < point.y() || (element.y1() - 7) > point.y() )
                return false;
            else if ( (element.y2() + 7) < point.y() || (element.y2() - 7) > point.y() )
               return false;
        }
    } else
    {
        //vertical element
        if ( element.x1() == element.x2() && element.y1() != element.y2() )
        {
            if ( element.y1() < element.y2() )
            {
                //from upper to bottom
                if ( element.y1() > point.y() || element.y2() < point.y() )
                    return false;
                else if ( (element.x1() + 7) < point.x() || (element.x1() - 7) > point.x() )
                    return false;
                else if ( (element.x2() + 7) < point.x() || (element.x2() - 7) > point.x() )
                   return false;
            } else
            {
                //from bottom to upper
                if ( element.y1() < point.y() || element.y2() > point.y() )
                    return false;
                else if ( (element.x1() + 7) < point.x() || (element.x1() - 7) > point.x() )
                    return false;
                else if ( (element.x2() + 7) < point.x() || (element.x2() - 7) > point.x() )
                   return false;
            }
        }
    }
    return true;
}

void WallElement::addStartPoint(QPoint point)
{
    element.setP1(point);
    element.setP2(point);
    vectorDim.setP1(point + QPoint(0,30));
    vectorDim.setP2(point + QPoint(0,30));
}

void WallElement::addEndPoint(QPoint point)
{
    element.setP2(point);
    if ( abs(element.dx()) > 10 )
    {
        vectorDim.setP1(element.p1() + QPoint(0,30));
        vectorDim.setP2(point + QPoint(0,30));
    } else
    {
        vectorDim.setP1(element.p1() + QPoint(30,0));
        vectorDim.setP2(point + QPoint(30,0));
    }
}

void WallElement::read(QDataStream &in)
{
    in>>length;
    in>>element;
    in>>vectorDim;
}

void WallElement::write(QDataStream& out)
{
    out<<length;
    out<<element;
    out<<vectorDim;
}

QDataStream &operator<<(QDataStream &out, const WallElement &data)
{
    const_cast<WallElement &>(data).write(out);
    return out;
}

QDataStream &operator>>(QDataStream &in, WallElement &data)
{
    data.read(in);
    return in;
}
