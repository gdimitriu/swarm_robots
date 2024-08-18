#ifndef ELEMENTFACTORY_H
#define ELEMENTFACTORY_H

#include "mapelement.h"
#include "wallelement.h"
#include "doorelement.h"

class ElementFactory
{
private:
    ElementFactory() = delete;
public:
    static MapElement *create(QString type,QObject *parent);
};

#endif // ELEMENTFACTORY_H
