#ifndef CAMERACOMMANDBUILDER_H
#define CAMERACOMMANDBUILDER_H

#include <QString>

class CameraCommandBuilder {
public:
    virtual ~CameraCommandBuilder() {}
    virtual QString moveLeft(QString value) = 0;
    virtual QString moveRight(QString value) = 0;
    virtual QString moveUp(QString value) = 0;
    virtual QString moveDown(QString value) = 0;
    virtual QString moveCenterHorinzonal() = 0;
    virtual QString moveCenterVertical() = 0;
    virtual QString startStreaming() = 0;
    virtual QString stopStraming() = 0;
};

#endif // CAMERACOMMANDBUILDER_H
