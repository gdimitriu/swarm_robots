/*
 * Robot Control Center
 *
 * Copyright 2024 Gabriel Dimitriu
 *
 * This file is part of swarm_of_robots project.

 * swarm_of_robots is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * swarm_of_robots is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with swarm_of_robots; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#include "robotcontrol.h"
#include "ui_robotcontrol.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <fstream>
#include "robotcontroldashboard.h"
#include <QWebEngineView>
#include "swarmmovecommandbuilder.h"
#include "swarmpathnavcommandbuilder.h"
#include "swarmcameracommandbuilder.h"
#include "socketoperations.h"

RobotControl::RobotControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RobotControl)
{
    ui->setupUi(this);
    socket = nullptr;    
    currentButton = nullptr;
    isCurrentPowerChanges = false;
    init();
    initUINavigation();
    isMinMaxPowerChanged = MIN_MAX_POWER_CHANGES::NONE;
    isDistanceChanged = DISTANCE_CHANGES::NONE;
    isDebugMode = false;
    streamImageOrig = ui->streamImage;
    cameraMoveTimer = nullptr;
    //until new type of commands are available
    moveCommandBuilder = new SwarmMoveCommandBuilder();
    pathNavCommandBuilder = new SwarmPathNavCommandBuilder();
    cameraCommandBuilder = new SwarmCameraCommandBuilder();
    socketOperations = nullptr;
}

RobotControl::~RobotControl()
{
    if ( socket != nullptr )
    {
        socket->close();
        delete socket;
        socket = nullptr;
    }
    delete ui;
    delete moveCommandBuilder;
    moveCommandBuilder = nullptr;
    delete pathNavCommandBuilder;
    pathNavCommandBuilder = nullptr;
    if (socketOperations != nullptr)
    {
        delete socketOperations;
        socketOperations = nullptr;
    }
}

void RobotControl::init()
{
    //socket communication
    connect(ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(connectTo()));
    connect(ui->disconnectButton, SIGNAL(clicked(bool)), this, SLOT(disconnectFrom()));
    //setup line termination
    ui->lineTermination->addItem("None", QVariant::fromValue(LINE_TERMINATION::NONE));
    ui->lineTermination->addItem("LF", QVariant::fromValue(LINE_TERMINATION::LF));
    ui->lineTermination->addItem("CR", QVariant::fromValue(LINE_TERMINATION::CR));
    ui->lineTermination->addItem("CR&LF", QVariant::fromValue(LINE_TERMINATION::CR_LF));

    // moving buttons
    connect(ui->forwardButton, SIGNAL(pressed()), this, SLOT(forward()));
    connect(ui->forwardButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->backwardButton, SIGNAL(pressed()), this, SLOT(backward()));
    connect(ui->backwardButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->leftButton, SIGNAL(pressed()), this, SLOT(left()));
    connect(ui->leftButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->rightButton, SIGNAL(pressed()), this, SLOT(right()));
    connect(ui->rightButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), this, SLOT(stop()));
    ui->robotRadioButton->setChecked(true);
    ui->cameraRadioButton->setChecked(false);
    ui->cameraStepValue->setText("1");
    ui->cameraTimerIntervalValue->setText("500");

    // power level
    ui->maximumPower->setText("4095");
    ui->minimumPower->setText("1000");
    ui->powerLevel->setRange(ui->minimumPower->text().toInt(),ui->maximumPower->text().toInt());
    connect(ui->powerLevel, SIGNAL(sliderMoved(int)), this, SLOT(powerChanged(int)));

    connect(ui->minimumPower, SIGNAL(editingFinished()), this, SLOT(minPowerChanged()));
    connect(ui->maximumPower, SIGNAL(editingFinished()), this, SLOT(maxPowerChanged()));
    connect(ui->lowPowerDistance, SIGNAL(editingFinished()), this, SLOT(lowPowerDistanceChanged()));
    connect(ui->stopDistance, SIGNAL(editingFinished()), this, SLOT(stopDistanceChanged()));

    //fetch config from Robot
    connect(ui->fetchButton, SIGNAL(clicked(bool)), this, SLOT(fetchFromRobot()));
    //deploy config to Robot
    connect(ui->deployButton, SIGNAL(clicked(bool)), this, SLOT(deployToRobot()));

    //add status bar
    QLabel *label = new QLabel(tr("Connected:"), this);
    statusBar()->addWidget(label);
    statusLine = new QLineEdit(this);
    statusLine->setText("Disconnected");
    statusLine->setMaxLength(12);
    statusLine->setReadOnly(true);
    statusBar()->addWidget(statusLine);
    label = new QLabel(tr("Current path file: "), this);
    statusBar()->addWidget(label);
    currentFile = new QLineEdit(this);
    currentFile->setReadOnly(true);
    statusBar()->addWidget(currentFile);

    //steaming
    connect(ui->connectStreamButton, SIGNAL(clicked(bool)), this , SLOT(connectStream()));
    connect(ui->disconnectStreamButton, SIGNAL(clicked(bool)), this, SLOT(disconnectStream()));
    ui->streamingType->addItem("http","http");
    ui->startStopCommands->addItem("Yes", "Yes");
    ui->startStopCommands->addItem("No", "No");

    //debug mode
    connect(ui->startDebugButton, SIGNAL(clicked(bool)), this, SLOT(enableDebugMode()));
    connect(ui->stopDebugButton, SIGNAL(clicked(bool)), this, SLOT(disableDebugMode()));
    connect(ui->clearDebugButton, SIGNAL(clicked(bool)), this, SLOT(clearDebugMode()));
}

void RobotControl::initUINavigation()
{
    //create navigation path
    ui->itemPosition->addItem("First", QVariant::fromValue(ITEM_POSITION::FIRST));
    ui->itemPosition->addItem("Before", QVariant::fromValue(ITEM_POSITION::BEFORE));
    ui->itemPosition->addItem("Current", QVariant::fromValue(ITEM_POSITION::CURRENT));
    ui->itemPosition->addItem("After", QVariant::fromValue(ITEM_POSITION::AFTER));
    ui->itemPosition->addItem("Last", QVariant::fromValue(ITEM_POSITION::LAST));

    connect(ui->movePathButton, SIGNAL(clicked(bool)), this, SLOT(movePathItem()));
    connect(ui->addPathButton, SIGNAL(clicked(bool)), this, SLOT(addPathItem()));
    connect(ui->deletePathButton, SIGNAL(clicked(bool)), this, SLOT(deletePathItem()));
    connect(ui->clearPathButton, SIGNAL(clicked(bool)), this, SLOT(clearPathItems()));
    connect(ui->replacePathButton, SIGNAL(clicked(bool)), this, SLOT(replacePathItem()));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(displayCommandPathItem(QListWidgetItem*)));

    //create Menu for path
    filePathMenu = menuBar()->addMenu(tr("&Navigation"));
    loadFilePathAction = new QAction(tr("&Load Robot Path File"), this);
    loadFilePathAction->setShortcut(tr("Ctrl+L"));
    loadFilePathAction->setStatusTip(tr("Load a file which contains the robot path"));
    connect(loadFilePathAction, SIGNAL(triggered(bool)), this, SLOT(loadFilePath()));
    filePathMenu->addAction(loadFilePathAction);
    saveFilePathAction = new QAction(tr("&Save Robot Path File"), this);
    saveFilePathAction->setShortcut(tr("Ctrl+S"));
    saveFilePathAction->setStatusTip(tr("Save a file which contains the robot path"));
    connect(saveFilePathAction, SIGNAL(triggered(bool)), this, SLOT(saveFilePath()));
    filePathMenu->addAction(saveFilePathAction);
    saveAsFilePathAction = new QAction(tr("Save As Robot Path File"), this);
    saveAsFilePathAction->setShortcut(tr("Ctrl+A"));
    saveAsFilePathAction->setStatusTip(tr("Save a new file which contains the robot path"));
    connect(saveAsFilePathAction, SIGNAL(triggered(bool)), this, SLOT(saveAsFilePath()));
    filePathMenu->addAction(saveAsFilePathAction);

    //deploy navigation paths
    filePathMenu->addSeparator();
    deployNavigationPathAction = new QAction(tr("&Deploy path"));
    deployNavigationPathAction->setShortcut(tr("Ctrl+D"));
    deployNavigationPathAction->setStatusTip(tr("Deploy path to the robot"));
    connect(deployNavigationPathAction, SIGNAL(triggered(bool)), this, SLOT(deployNavigationPath()));
    filePathMenu->addAction(deployNavigationPathAction);
    saveDeployedPathToFileAction = new QAction(tr("Save deployed path to file"));
    saveDeployedPathToFileAction->setStatusTip(tr("Save already deployed path to file"));
    connect(saveDeployedPathToFileAction, SIGNAL(triggered(bool)), this, SLOT(saveDeployedPathToFile()));
    filePathMenu->addAction(saveDeployedPathToFileAction);
    loadNavigationPathFromFileAction = new QAction(tr("Load path from file"));
    loadNavigationPathFromFileAction->setStatusTip(tr("Load path to memory from deployed file"));
    connect(loadNavigationPathFromFileAction, SIGNAL(triggered(bool)), this, SLOT(loadNavigationPathFromFile()));
    filePathMenu->addAction(loadNavigationPathFromFileAction);
    removeNavigationPathFileAction = new QAction(tr("Remove path file"));
    removeNavigationPathFileAction->setStatusTip(tr("Remove deployed path file from robot"));
    connect(removeNavigationPathFileAction, SIGNAL(triggered(bool)), this, SLOT(removeNavigationPathFile()));
    filePathMenu->addAction(removeNavigationPathFileAction);
    fetchNavigationPathAction = new QAction(tr("&Fetch path"));
    fetchNavigationPathAction->setShortcut(tr("Ctrl+F"));
    fetchNavigationPathAction->setStatusTip(tr("Fetch path from the robot"));
    connect(fetchNavigationPathAction, SIGNAL(triggered(bool)), this, SLOT(fetchNavigationPath()));
    filePathMenu->addAction(fetchNavigationPathAction);

    //run navigation paths
    filePathMenu->addSeparator();
    runFowardWithPathAction = new QAction(tr("Run forward"));
    runFowardWithPathAction->setStatusTip(tr("Run forward with the deployed navigation path"));
    connect(runFowardWithPathAction, SIGNAL(triggered(bool)), this, SLOT(runForwardWithPath()));
    filePathMenu->addAction(runFowardWithPathAction);
    runBackwardWithPathAction = new QAction(tr("Run backward"));
    runBackwardWithPathAction->setStatusTip(tr("Run backward with the deployed navigation path"));
    connect(runBackwardWithPathAction, SIGNAL(triggered(bool)), this, SLOT(runBackwardWithPath()));
    filePathMenu->addAction(runBackwardWithPathAction);

    ui->deployedType->addItem("Memory", QVariant::fromValue(DEPLOY_RUN_TYPE::MEMORY));
    ui->deployedType->addItem("File", QVariant::fromValue(DEPLOY_RUN_TYPE::FILE));
    ui->deployedType->addItem("EEPROM", QVariant::fromValue(DEPLOY_RUN_TYPE::EEPROM));
}

void RobotControl::displayCommandPathItem(QListWidgetItem *item)
{
    ui->newPathCommand->clear();
    ui->newPathCommand->setText(item->text());
}

void RobotControl::connectTo()
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(sockConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisconnected()));
    socket->connectToHost(ui->ipValue->text(), ui->portValue->text().toInt());
    ui->connectButton->clearFocus();
    if (socketOperations != nullptr)
    {
        delete socketOperations;
        socketOperations = new SocketOperations(socket);
    }
    else
    {
        socketOperations = new SocketOperations(socket);
    }
}

void RobotControl::disconnectFrom()
{
    if ( socket != nullptr )
    {
        sendOneWay("exit#", false);
        socket->disconnectFromHost();
        socket->close();
        delete socket;
        socket = nullptr;
        delete socketOperations;
        socketOperations = nullptr;
    }
    ui->disconnectButton->clearFocus();
}

void RobotControl::sockDisconnected()
{
    statusLine->setText("Disconnected");
    repaint();
    if (socketOperations != nullptr)
    {
        delete socketOperations;
        socketOperations = nullptr;
    }
    if (socket != nullptr)
    {
        delete socket;
        socket = nullptr;
    }
}

void RobotControl::sockConnected()
{
    statusLine->setText("Connected");
    repaint();
}

void RobotControl::forward()
{
    if ( !isConnected() )
    {
        ui->forwardButton->clearFocus();
        return;
    }
    if ( ui->robotRadioButton->isChecked() )
    {
        sendPowerToRobotIfModified();
        sendOneWay(moveCommandBuilder->moveOrRotate("1", "0"));
    }
    else if ( ui->cameraRadioButton->isChecked() )
    {
        sendOneWay(cameraCommandBuilder->moveUp(ui->cameraStepValue->text()));
        if (cameraMoveTimer == nullptr)
        {
            cameraDirection = CameraDirection::FORWARD;
            cameraMoveTimer = new QTimer(this);
            cameraMoveTimer->setInterval(ui->cameraTimerIntervalValue->text().toInt());
            connect(cameraMoveTimer, SIGNAL(timeout()), this, SLOT(forward()));
            cameraMoveTimer->start();
        }
    }
    currentButton = ui->forwardButton;
}

void RobotControl::backward()
{
    if ( !isConnected() )
    {
        ui->backwardButton->clearFocus();
        return;
    }
    if ( ui->robotRadioButton->isChecked() )
    {
        sendPowerToRobotIfModified();
        sendOneWay(moveCommandBuilder->moveOrRotate("-1", "0"));
    }
    else if ( ui->cameraRadioButton->isChecked() )
    {
        sendOneWay(cameraCommandBuilder->moveDown(ui->cameraStepValue->text()));
        if (cameraMoveTimer == nullptr)
        {
            cameraDirection = CameraDirection::BACKWARD;
            cameraMoveTimer = new QTimer(this);
            cameraMoveTimer->setInterval(ui->cameraTimerIntervalValue->text().toInt());
            connect(cameraMoveTimer, SIGNAL(timeout()), this, SLOT(backward()));
            cameraMoveTimer->start();
        }
    }
    currentButton = ui->backwardButton;
}

void RobotControl::left()
{
    if ( !isConnected() )
    {
        ui->leftButton->clearFocus();
        return;
    }
    if ( ui->robotRadioButton->isChecked() )
    {
        sendPowerToRobotIfModified();
        sendOneWay(moveCommandBuilder->moveOrRotate("0", "-1"));
    }
    else if ( ui->cameraRadioButton->isChecked() )
    {
        sendOneWay(cameraCommandBuilder->moveLeft(ui->cameraStepValue->text()));
        if (cameraMoveTimer == nullptr)
        {
            cameraDirection = CameraDirection::LEFT;
            cameraMoveTimer = new QTimer(this);
            cameraMoveTimer->setInterval(ui->cameraTimerIntervalValue->text().toInt());
            connect(cameraMoveTimer, SIGNAL(timeout()), this, SLOT(left()));
            cameraMoveTimer->start();
        }
    }
    currentButton = ui->leftButton;
}

void RobotControl::right()
{
    if ( !isConnected() )
    {
        ui->rightButton->clearFocus();
        return;
    }
    if ( ui->robotRadioButton->isChecked() )
    {
        sendPowerToRobotIfModified();
        sendOneWay(moveCommandBuilder->moveOrRotate("0", "1"));
    }
    else if ( ui->cameraRadioButton->isChecked() )
    {
        sendOneWay(cameraCommandBuilder->moveRight(ui->cameraStepValue->text()));
        if (cameraMoveTimer == nullptr)
        {
            cameraDirection = CameraDirection::RIGHT;
            cameraMoveTimer = new QTimer(this);
            cameraMoveTimer->setInterval(ui->cameraTimerIntervalValue->text().toInt());
            connect(cameraMoveTimer, SIGNAL(timeout()), this, SLOT(right()));
            cameraMoveTimer->start();
        }
    }
    currentButton = ui->rightButton;
}

void RobotControl::stop()
{
    if ( !isConnected() )
    {
        if ( currentButton != nullptr )
        {
            currentButton->clearFocus();
            currentButton = nullptr;
        }
        else
        {
            ui->stopButton->clearFocus();
        }
        return;
    }
    if ( ui->robotRadioButton->isChecked() )
    {
        sendOneWay(moveCommandBuilder->stop());
    }
    else if (ui->cameraRadioButton->isChecked())
    {
        if (cameraMoveTimer != nullptr)
        {
            switch(cameraDirection)
            {
            case CameraDirection::FORWARD:
                disconnect(cameraMoveTimer, SIGNAL(timeout()), this, SLOT(forward()));
                break;
            case CameraDirection::BACKWARD:
                disconnect(cameraMoveTimer, SIGNAL(timeout()), this, SLOT(backward()));
                break;
            case CameraDirection::LEFT:
                disconnect(cameraMoveTimer, SIGNAL(timeout()), this, SLOT(left()));
                break;
            case CameraDirection::RIGHT:
                disconnect(cameraMoveTimer, SIGNAL(timeout()), this, SLOT(right()));
                break;
            case CameraDirection::NONE:
                break;
            }
            cameraMoveTimer->stop();
            delete cameraMoveTimer;
            cameraMoveTimer = nullptr;
        }
    }
    if ( currentButton != nullptr )
    {
        currentButton->clearFocus();
        currentButton = nullptr;
    }
    else
    {
        ui->stopButton->clearFocus();
    }
}

void RobotControl::powerChanged(int value)
{
    ui->currentPowerLevel->setText(QString::number(value));
    repaint();
    isCurrentPowerChanges = true;
}

QString RobotControl::sendWithReply(QString message)
{
    if ( socket != nullptr )
    {
        switch ( ui->lineTermination->currentData().value<LINE_TERMINATION>() )
        {
        case LINE_TERMINATION::LF :
            message.append('\n');
            break;
        case LINE_TERMINATION::CR :
            message.append('\r');
            break;
        case LINE_TERMINATION::CR_LF :
            message.append("\r\n");
            break;
        default:
            break;
        }
        if ( isDebugMode )
        {
            ui->debugView->moveCursor(QTextCursor::End);
            ui->debugView->insertPlainText("Send data: ");
            ui->debugView->insertPlainText(message);
        }
        QString readData = socketOperations->sendWithReply(message);
        if ( isDebugMode )
        {
            ui->debugView->moveCursor(QTextCursor::End);
            ui->debugView->insertPlainText("Receiving data: ");
            ui->debugView->insertPlainText(readData);
        }
        return readData;
    }
    else
    {
        QMessageBox::critical(this, tr("First connect to robot."), tr("First connect to robot"), QMessageBox::Ok);
    }
    return "";
}

void RobotControl::sendOneWay(QString message, bool hasAck)
{
    if ( socket != nullptr )
    {
        switch ( (LINE_TERMINATION) ui->lineTermination->currentData().value<LINE_TERMINATION>() )
        {
        case LINE_TERMINATION::LF :
            message.append('\n');
            break;
        case LINE_TERMINATION::CR :
            message.append('\r');
            break;
        case LINE_TERMINATION::CR_LF :
            message.append("\r\n");
            break;
        default:
            break;
        }
        if ( isDebugMode )
        {
            ui->debugView->moveCursor(QTextCursor::End);
            ui->debugView->insertPlainText("Send data: ");
            ui->debugView->insertPlainText(message);
        }
        QString ackMessage = socketOperations->sendOneWay(message, hasAck);
        if ( hasAck )
        {
            if ( isDebugMode )
            {
                ui->debugView->moveCursor(QTextCursor::End);
                ui->debugView->insertPlainText("Receiving ack data: ");
                ui->debugView->insertPlainText(ackMessage);
            }
        }
    }
    else
    {
        QMessageBox::critical(this, tr("First connect to robot."), tr("First connect to robot"), QMessageBox::Ok);
    }
}

void RobotControl::deployToRobot()
{
    if ( !isConnected() )
    {
        ui->deployButton->clearFocus();
        return;
    }
    sendOneWay(moveCommandBuilder->setMaxPower(ui->maximumPower->text()), true);
    sendOneWay(moveCommandBuilder->setMinPower(ui->minimumPower->text()), true);
    sendOneWay(moveCommandBuilder->setCurrentPower(ui->currentPowerLevel->text()), true);
    sendOneWay(moveCommandBuilder->setStopDistance(ui->stopDistance->text()), true);
    sendOneWay(moveCommandBuilder->setLowPowerDistance(ui->lowPowerDistance->text()), true);
    ui->deployButton->clearFocus();
}

void RobotControl::fetchFromRobot()
{
    if ( !isConnected() )
    {
        ui->fetchButton->clearFocus();
        return;
    }
    QString value = sendWithReply(moveCommandBuilder->getMaxPower());
    ui->maximumPower->setText(value.trimmed());
    value = sendWithReply(moveCommandBuilder->getMinPower());
    ui->minimumPower->setText(value.trimmed());
    value = sendWithReply(moveCommandBuilder->getCurrentPower());
    ui->currentPowerLevel->setText(value.trimmed());
    ui->powerLevel->setRange(ui->minimumPower->text().toInt(),ui->maximumPower->text().toInt());
    ui->powerLevel->setValue(ui->minimumPower->text().toInt());
    isMinMaxPowerChanged = MIN_MAX_POWER_CHANGES::NONE;
    value = sendWithReply(moveCommandBuilder->getStopDistance());
    ui->stopDistance->setText(value.trimmed());
    value = sendWithReply(moveCommandBuilder->getLowPowerDistance());
    ui->lowPowerDistance->setText(value.trimmed());
    ui->fetchButton->clearFocus();
}

void RobotControl::minPowerChanged()
{
    ui->powerLevel->setMinimum(ui->minimumPower->text().toInt());
    if ( ui->powerLevel->minimum() > ui->currentPowerLevel->text().toInt() )
    {
        ui->currentPowerLevel->setText( QString::number(ui->powerLevel->minimum()));
        isCurrentPowerChanges = true;
    }
    if ( isMinMaxPowerChanged == MIN_MAX_POWER_CHANGES::NONE )
        isMinMaxPowerChanged = MIN_MAX_POWER_CHANGES::MIN;
    else
        isMinMaxPowerChanged = MIN_MAX_POWER_CHANGES::MINMAX;
}

void RobotControl::maxPowerChanged()
{
    ui->powerLevel->setMaximum(ui->maximumPower->text().toInt());
    if ( ui->powerLevel->maximum() < ui->currentPowerLevel->text().toInt() )
    {
        ui->currentPowerLevel->setText(QString::number(ui->powerLevel->maximum()));
        isCurrentPowerChanges = true;
    }
    if ( isMinMaxPowerChanged == MIN_MAX_POWER_CHANGES::NONE )
        isMinMaxPowerChanged = MIN_MAX_POWER_CHANGES::MAX;
    else
        isMinMaxPowerChanged = MIN_MAX_POWER_CHANGES::MINMAX;
}

void RobotControl::stopDistanceChanged()
{
    if ( isDistanceChanged == DISTANCE_CHANGES::LOW_POWER )
        isDistanceChanged =  DISTANCE_CHANGES::BOTH;
    else
        isDistanceChanged = DISTANCE_CHANGES::STOP;
}

void RobotControl::lowPowerDistanceChanged()
{
    if ( isDistanceChanged == DISTANCE_CHANGES::STOP )
        isDistanceChanged = DISTANCE_CHANGES::BOTH;
    else
        isDistanceChanged = DISTANCE_CHANGES::LOW_POWER;
}

void RobotControl::sendPowerToRobotIfModified()
{
    switch( isMinMaxPowerChanged )
    {
    case MIN_MAX_POWER_CHANGES::MAX:
        sendOneWay(moveCommandBuilder->setMaxPower(ui->maximumPower->text()), true);
        break;
    case MIN_MAX_POWER_CHANGES::MIN:
        sendOneWay(moveCommandBuilder->setMinPower(ui->minimumPower->text()), true);
        break;
    case MIN_MAX_POWER_CHANGES::MINMAX:
        sendOneWay(moveCommandBuilder->setMaxPower(ui->maximumPower->text()), true);
        sendOneWay(moveCommandBuilder->setMinPower(ui->minimumPower->text()), true);
        break;
    default:
        break;
    }
    isMinMaxPowerChanged = MIN_MAX_POWER_CHANGES::NONE;
    if ( isCurrentPowerChanges )
    {
        sendOneWay(moveCommandBuilder->setCurrentPower(ui->currentPowerLevel->text()), true);
        isCurrentPowerChanges = false;
    }

    switch( isDistanceChanged )
    {
    case DISTANCE_CHANGES::STOP :
        sendOneWay(moveCommandBuilder->setStopDistance(ui->stopDistance->text()), true);
        break;
    case DISTANCE_CHANGES::LOW_POWER:
        sendOneWay(moveCommandBuilder->setLowPowerDistance(ui->lowPowerDistance->text()), true);
        break;
    case DISTANCE_CHANGES::BOTH :
        sendOneWay(moveCommandBuilder->setStopDistance(ui->stopDistance->text()), true);
        sendOneWay(moveCommandBuilder->setLowPowerDistance(ui->lowPowerDistance->text()), true);
        break;
    default:
        break;
    }
    isDistanceChanged = DISTANCE_CHANGES::NONE;
}

void RobotControl::loadFilePath()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Load Path File"), ".", tr("Text files (*.txt)"));
    if ( filePath == nullptr || filePath.isEmpty() )
        return;
    currentFile->setText(filePath);
    ui->listWidget->clear();
    QFile inFile(currentFile->text());
    if ( !inFile.open(QIODevice::ReadOnly | QIODevice::Text) )
        return;
    while ( !inFile.atEnd() )
    {
        QByteArray line = inFile.readLine();
        ui->listWidget->addItem(QString::fromStdString(line.toStdString()).trimmed());
    }
    inFile.close();
}

void RobotControl::saveFilePath()
{
    if ( currentFile->text().isEmpty() )
    {
        QMessageBox::critical(this, tr("First select a file."), tr("First select a file."), QMessageBox::Ok);
        return;
    }
    std::ofstream outFile;
    outFile.open(currentFile->text().toStdString().c_str(), std::ios::trunc);
    for ( int i = 0; i < ui->listWidget->count(); ++i )
    {
        outFile<<ui->listWidget->item(i)->text().toStdString()<<std::endl;
    }
    outFile.close();
}

void RobotControl::saveAsFilePath()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save As Path File"), ".", tr("Text files (*.txt)"));
    if ( filePath == nullptr || filePath.isEmpty() )
        return;
    currentFile->setText(filePath);
    std::ofstream outFile;
    outFile.open(currentFile->text().toStdString().c_str(), std::ios::trunc);
    for ( int i = 0; i < ui->listWidget->count(); ++i )
    {
        outFile<<ui->listWidget->item(i)->text().toStdString()<<std::endl;
    }
    outFile.close();
}

void RobotControl::addPathItem()
{
    int currentPos = ui->listWidget->currentRow();
    switch((ITEM_POSITION) ui->itemPosition->currentData().value<ITEM_POSITION>() )
    {
    case ITEM_POSITION::FIRST :
        ui->listWidget->insertItem(0, ui->newPathCommand->text());
        break;
    case ITEM_POSITION::BEFORE :
        if ( currentPos == -1 )
        {
            QMessageBox::critical(this, tr("First select an item."), tr("First select an item"), QMessageBox::Ok);
             ui->addPathButton->clearFocus();
            return;
        }
        if ( currentPos == 0 )
            currentPos = 1;
        ui->listWidget->insertItem(currentPos - 1, ui->newPathCommand->text());
        break;
    case ITEM_POSITION::CURRENT :
        if ( currentPos == -1 )
        {
            QMessageBox::critical(this, tr("First select an item."), tr("First select an item"), QMessageBox::Ok);
             ui->addPathButton->clearFocus();
            return;
        }
        if ( currentPos == 0 )
            currentPos = 1;
        ui->listWidget->insertItem(currentPos, ui->newPathCommand->text());
        break;
    case ITEM_POSITION::AFTER :
        if ( currentPos == -1 )
        {
            QMessageBox::critical(this, tr("First select an item."), tr("First select an item"), QMessageBox::Ok);
             ui->addPathButton->clearFocus();
            return;
        }
        ui->listWidget->insertItem(currentPos + 1, ui->newPathCommand->text());
        break;
    case ITEM_POSITION::LAST :
        ui->listWidget->addItem(ui->newPathCommand->text());
        break;
    }
    ui->addPathButton->clearFocus();
}

void RobotControl::deletePathItem()
{
    ui->listWidget->takeItem(ui->listWidget->currentRow());
    ui->deletePathButton->clearFocus();
}

void RobotControl::movePathItem()
{
    int currentPos = ui->listWidget->currentRow();

    QListWidgetItem *element = ui->listWidget->takeItem(currentPos);

    switch((ITEM_POSITION) ui->itemPosition->currentData().value<ITEM_POSITION>() )
    {
    case ITEM_POSITION::FIRST :
        ui->listWidget->insertItem(0, element);
        break;
    case ITEM_POSITION::BEFORE :
        if ( currentPos == -1 )
        {
            QMessageBox::critical(this, tr("First select an item."), tr("First select an item"), QMessageBox::Ok);
             ui->movePathButton->clearFocus();
            return;
        }
        if ( currentPos == 0 )
            currentPos = 1;
        ui->listWidget->insertItem(currentPos - 1, element);
        break;
    case ITEM_POSITION::CURRENT :
        if ( currentPos == -1 )
        {
            QMessageBox::critical(this, tr("First select an item."), tr("First select an item"), QMessageBox::Ok);
             ui->movePathButton->clearFocus();
            return;
        }
        break;
    case ITEM_POSITION::AFTER :
        if ( currentPos == -1 )
        {
            QMessageBox::critical(this, tr("First select an item."), tr("First select an item"), QMessageBox::Ok);
             ui->movePathButton->clearFocus();
            return;
        }
        ui->listWidget->insertItem(currentPos + 1, element);
        break;
    case ITEM_POSITION::LAST :
        ui->listWidget->addItem(element);
        break;
    }
    ui->movePathButton->clearFocus();
}

void RobotControl::replacePathItem()
{
    int currentPos = ui->listWidget->currentRow();
    if ( currentPos == -1 )
    {
        ui->replacePathButton->clearFocus();
        return;
    }
    QListWidgetItem *element = ui->listWidget->currentItem();
    element->setText(ui->newPathCommand->text());
    ui->replacePathButton->clearFocus();
}

void RobotControl::clearPathItems()
{
    ui->listWidget->clear();
    ui->clearPathButton->clearFocus();
}

void RobotControl::deployNavigationPath()
{
    bool isFile = false;
    if ( isConnected() )
    {
        if ( ui->listWidget->count() == 0 )
            return;
        sendOneWay(pathNavCommandBuilder->startDeploy(), true);
        switch((DEPLOY_RUN_TYPE) ui->deployedType->currentData().value<DEPLOY_RUN_TYPE>() )
        {
        case DEPLOY_RUN_TYPE::MEMORY :
            break;
        case DEPLOY_RUN_TYPE::FILE :
        {
            QString fileToDeploy = ui->deployedFile->text();
            if ( fileToDeploy.isEmpty() )
                return;
            sendOneWay(pathNavCommandBuilder->deployFile(fileToDeploy), true);
            isFile = true;
            break;
        }
        case DEPLOY_RUN_TYPE::EEPROM :
            return;
        }
        for ( int i = 0; i < ui->listWidget->count(); ++i )
        {
            sendOneWay(pathNavCommandBuilder->deployCommand(ui->listWidget->item(i)->text()), true);
        }
        if(isFile == true)
        {
            sendOneWay("Nfe#", true);
        }
    }
}

void RobotControl::saveDeployedPathToFile()
{
    if ( !isConnected() )
    {
        return;
    }
    QString fileToDeploy = ui->deployedFile->text();
    if ( fileToDeploy.isEmpty() )
        return;
    sendOneWay(pathNavCommandBuilder->saveToFile(fileToDeploy), true);
}

void RobotControl::loadNavigationPathFromFile()
{
    if ( !isConnected() )
    {
        return;
    }
    QString fileToDeploy = ui->deployedFile->text();
    if ( fileToDeploy.isEmpty() )
        return;
    sendOneWay(pathNavCommandBuilder->loadFromFile(fileToDeploy), true);
}

void RobotControl::removeNavigationPathFile()
{
    if ( !isConnected() )
    {
        return;
    }
    QString fileToDeploy = ui->deployedFile->text();
    if ( fileToDeploy.isEmpty() )
        return;
    sendOneWay(pathNavCommandBuilder->removeFile(fileToDeploy), true);
}

void RobotControl::fetchNavigationPath()
{
    if ( !isConnected() )
    {

        return;
    }
}

void RobotControl::runForwardWithPath()
{
    if ( isConnected() )
    {
        switch((DEPLOY_RUN_TYPE) ui->deployedType->currentData().value<DEPLOY_RUN_TYPE>() )
        {
        case DEPLOY_RUN_TYPE::MEMORY :
            sendOneWay(pathNavCommandBuilder->runForward(), true);
            return;
        case DEPLOY_RUN_TYPE::FILE :
        {
            QString fileToDeploy = ui->deployedFile->text();
            if ( fileToDeploy.isEmpty() )
                return;
            sendOneWay(pathNavCommandBuilder->runForward(fileToDeploy), true);
            return;
        }
        case DEPLOY_RUN_TYPE::EEPROM :
            return;
        }
    }
}

void RobotControl::runBackwardWithPath()
{
    if ( isConnected() )
    {
        QString message;
        switch((DEPLOY_RUN_TYPE) ui->deployedType->currentData().value<DEPLOY_RUN_TYPE>() )
        {
        case DEPLOY_RUN_TYPE::MEMORY :
            sendOneWay(pathNavCommandBuilder->runBackward(), true);
            return;
        case DEPLOY_RUN_TYPE::FILE :
        {
            QString fileToDeploy = ui->deployedFile->text();
            if ( fileToDeploy.isEmpty() )
                return;
            sendOneWay(pathNavCommandBuilder->runBackward(fileToDeploy), true);
            return;
        }
        case DEPLOY_RUN_TYPE::EEPROM :
            return;
        }
    }
}

bool RobotControl::isConnected()
{
    if ( socket != nullptr && socket->isOpen() )
    {
        return true;
    }
    QMessageBox::critical(this, tr("First connect to robot."), tr("First connect to robot"), QMessageBox::Ok);
    return false;
}

void RobotControl::connectStream()
{
    if ( ui->startStopCommands->currentText().compare("yes",Qt::CaseInsensitive) == 0 )
    {
        sendOneWay(cameraCommandBuilder->startStreaming());
    }
    QByteArray geometry = ui->streamImage->saveGeometry();
    QSize size = ui->streamImage->size();
    if ( ui->streamingType->currentText() == QString("http") )
    {
        QUrl url("http://" + ui->ipValue->text() + ":" + ui->cameraPort->text());
        QWebEngineView *webengine = new QWebEngineView(this);
        ui->streamImage = webengine;
        ((QWebEngineView *)ui->streamImage)->load(url);
    }
    ui->streamImage->restoreGeometry(geometry);
    ui->streamImage->setEnabled(true);
    ui->streamImage->setFixedSize(size);
    ui->streamImage->show();
    this->repaint();
    ui->connectStreamButton->clearFocus();
}

void RobotControl::disconnectStream()
{
    if ( ui->startStopCommands->currentText().compare("yes",Qt::CaseInsensitive) == 0 )
    {
        sendOneWay(cameraCommandBuilder->stopStraming());
    }
    if ( ui->streamingType->currentText() == QString("http") )
    {
        ((QWebEngineView *)ui->streamImage)->close();
    }
    delete ui->streamImage;
    ui->streamImage = streamImageOrig;
    this->repaint();
    ui->disconnectStreamButton->clearFocus();
}

void RobotControl::enableDebugMode()
{
    isDebugMode = true;
    ui->startDebugButton->clearFocus();
}

void RobotControl::disableDebugMode()
{
    isDebugMode = false;
    ui->stopDebugButton->clearFocus();
}

void RobotControl::clearDebugMode()
{
    ui->debugView->clear();
    ui->clearDebugButton->clearFocus();
}

void RobotControl::setIpValue(QString value)
{
    ui->ipValue->clear();
    ui->ipValue->setText(value);
}

void RobotControl::setPortValue(QString value)
{
    ui->portValue->clear();
    ui->portValue->setText(value);
}

void RobotControl::setCameraPort(QString value)
{
    ui->cameraPort->setText(value);
}

void RobotControl::setCameraProtocol(QString value)
{
    ui->streamingType->setCurrentIndex(ui->streamingType->findText(value));
}

void RobotControl::closeEvent(QCloseEvent *event)
{
    RobotControlDashboard * caller = static_cast<RobotControlDashboard *>(parent());
    event->accept();
    caller->childIsClosed(this);
}

void RobotControl::setCameraCommands(QString value)
{
    ui->startStopCommands->setCurrentIndex(ui->startStopCommands->findText(value));
}

void RobotControl::setMovingCamera(bool value)
{
    if (!value)
    {
        ui->cameraTimerIntervalValue->clear();
        ui->cameraTimerIntervalValue->setDisabled(true);
        ui->cameraStepValue->clear();
        ui->cameraStepValue->setDisabled(true);
        ui->cameraRadioButton->setDisabled(true);
    }
}
