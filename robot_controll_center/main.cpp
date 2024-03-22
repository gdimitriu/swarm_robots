#include "robotcontroll.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RobotControll w;
    w.show();

    return a.exec();
}
