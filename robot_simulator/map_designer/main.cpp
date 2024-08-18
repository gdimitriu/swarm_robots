#include "mapdesigner.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MapDesigner w;
    w.show();

    return a.exec();
}
