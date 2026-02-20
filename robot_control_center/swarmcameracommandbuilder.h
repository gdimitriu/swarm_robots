#ifndef SWARMCAMERACOMMANDBUILDER_H
#define SWARMCAMERACOMMANDBUILDER_H

#include "cameracommandbuilder.h"

class SwarmCameraCommandBuilder : public CameraCommandBuilder
{
public:
    SwarmCameraCommandBuilder();
    virtual ~SwarmCameraCommandBuilder();
    virtual QString moveLeft(QString value);
    virtual QString moveRight(QString value);
    virtual QString moveUp(QString value);
    virtual QString moveDown(QString value);
    virtual QString moveCenter();
    virtual QString startStreaming();
    virtual QString stopStraming();
};

#endif // SWARMCAMERACOMMANDBUILDER_H
