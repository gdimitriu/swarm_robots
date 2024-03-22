/*
 * Robot Controll Center
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
#include "robotcontroll.h"
#include "ui_robotcontroll.h"
#include <QMessageBox>
#include <QFileDialog>

RobotControll::RobotControll(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RobotControll)
{
    ui->setupUi(this);
    socket = nullptr;    
    currentButton = nullptr;
    isCurrentPowerChanges = false;
    init();
    initUINavigation();
    isMinMaxPowerChanged = MIN_MAX_POWER_CHANGES::NONE;
    isDistanceChanged = DISTANCE_CHANGES::NONE;
}

RobotControll::~RobotControll()
{
    if ( socket != nullptr ) {
        socket->close();
        delete socket;
    }
    delete ui;
}

void RobotControll::init()
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
    label = new QLabel(tr("Current file: "), this);
    statusBar()->addWidget(label);
    currentFile = new QLineEdit(this);
    currentFile->setReadOnly(true);
    statusBar()->addWidget(currentFile);

    //steaming
    connect(ui->connectStreamButton, SIGNAL(clicked(bool)), this , SLOT(connectStream()));
    connect(ui->disconnectStreamButton, SIGNAL(clicked(bool)), this, SLOT(disconnectStream()));
    ui->streamingType->addItem("http","http");
    ui->streamingType->addItem("vlc", "vlc");
}

void RobotControll::initUINavigation()
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

    //create Menu for path
    filePathMenu = menuBar()->addMenu(tr("&Navigation"));
    loadFilePathAction = new QAction(tr("&Load File Path"), this);
    loadFilePathAction->setShortcut(tr("Ctrl+L"));
    loadFilePathAction->setStatusTip(tr("Load a file which contains the robot path"));
    connect(loadFilePathAction, SIGNAL(triggered(bool)), this, SLOT(loadFilePath()));
    filePathMenu->addAction(loadFilePathAction);
    saveFilePathAction = new QAction(tr("&Save File Path"), this);
    saveFilePathAction->setShortcut(tr("Ctrl+S"));
    saveFilePathAction->setStatusTip(tr("Save a file which contains the robot path"));
    connect(saveFilePathAction, SIGNAL(triggered(bool)), this, SLOT(saveFilePath()));
    filePathMenu->addAction(saveFilePathAction);
    saveAsFilePathAction = new QAction(tr("Save As File Path"), this);
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
    fetchNavigationPathAction = new QAction(tr("&Fetch path"));
    fetchNavigationPathAction->setShortcut(tr("Ctrl+F"));
    fetchNavigationPathAction->setStatusTip(tr("Fetch path from the robot"));
    connect(fetchNavigationPathAction, SIGNAL(triggered(bool)), this, SLOT(fetchNavigationPath()));
    filePathMenu->addAction(fetchNavigationPathAction);

    //run navigation paths
    filePathMenu->addSeparator();
    runFowardWithPathAction = new QAction(tr("Run forward"));
    runFowardWithPathAction->setStatusTip(tr("Run forward with the deployed navigation path"));
    filePathMenu->addAction(runFowardWithPathAction);
    runBackwardWithPathAction = new QAction(tr("Run backward"));
    runBackwardWithPathAction->setStatusTip(tr("Run backward with the deployed navigation path"));
    filePathMenu->addAction(runBackwardWithPathAction);

    ui->deployedType->addItem("Memory", QVariant::fromValue(DEPLOY_RUN_TYPE::MEMORY));
    ui->deployedType->addItem("File", QVariant::fromValue(DEPLOY_RUN_TYPE::FILE));
    ui->deployedType->addItem("EEPROM", QVariant::fromValue(DEPLOY_RUN_TYPE::EEPROM));
}

void RobotControll::connectTo()
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(sockConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisconnected()));
    socket->connectToHost(ui->ipValue->text(), ui->portValue->text().toInt());
    ui->connectButton->clearFocus();
}

void RobotControll::disconnectFrom()
{
    if ( socket != nullptr )
    {
        socket->disconnectFromHost();
        socket->close();
        delete socket;
        socket = nullptr;
    }
    ui->disconnectButton->clearFocus();
}

void RobotControll::sockDisconnected()
{
    statusLine->setText("Disconnected");
    repaint();
}

void RobotControll::sockConnected()
{
    statusLine->setText("Connected");
    repaint();
}

void RobotControll::forward()
{
    if ( !isConnected() )
    {
        ui->forwardButton->clearFocus();
        return;
    }
    if ( ui->robotRadioButton->isChecked() )
    {
        sendPowerToRobotIfModified();
        sendOneWay("M1,0#");
    }
    currentButton = ui->forwardButton;
}

void RobotControll::backward()
{
    if ( !isConnected() )
    {
        ui->backwardButton->clearFocus();
        return;
    }
    if ( ui->robotRadioButton->isChecked() )
    {
        sendPowerToRobotIfModified();
        sendOneWay("M-1,0#");
    }
    currentButton = ui->backwardButton;
}

void RobotControll::left()
{
    if ( !isConnected() )
    {
        ui->leftButton->clearFocus();
        return;
    }
    if ( ui->robotRadioButton->isChecked() )
    {
        sendPowerToRobotIfModified();
        sendOneWay("M0,-1#");
    }
    currentButton = ui->leftButton;
}

void RobotControll::right()
{
    if ( !isConnected() )
    {
        ui->rightButton->clearFocus();
        return;
    }
    if ( ui->robotRadioButton->isChecked() )
    {
        sendPowerToRobotIfModified();
        sendOneWay("M0,1#");
    }
    currentButton = ui->rightButton;
}

void RobotControll::stop()
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
        sendOneWay("b#");
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

void RobotControll::powerChanged(int value)
{
    ui->currentPowerLevel->setText(QString::number(value));
    repaint();
    isCurrentPowerChanges = true;
}

QString RobotControll::sendWithReply(QString message)
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
        socket->write(message.toLatin1());
        socket->flush();
        socket->waitForReadyRead();
        QByteArray readData = socket->readAll();
        while ( socket->bytesAvailable() > 0) {
            QByteArray localReadData = socket->readAll();
            if ( readData.isEmpty() )
                break;
            readData.append(localReadData);
        }
        return QString(readData);
    }
    else
    {
        QMessageBox::critical(this, tr("First connect to robot."), tr("First connect to robot"), QMessageBox::Ok);
    }
    return "";
}

void RobotControll::sendOneWay(QString message, bool hasAck)
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
        socket->write(message.toLatin1());
        socket->flush();
        if ( hasAck )
        {
            socket->waitForReadyRead();
            QByteArray readData = socket->readAll();
            QString str(readData);
            while ( !str.contains("OK\r\n") )
            {
                socket->waitForReadyRead();
                str.append(socket->readAll());
            }
        }
    }
    else
    {
        QMessageBox::critical(this, tr("First connect to robot."), tr("First connect to robot"), QMessageBox::Ok);
    }
}

void RobotControll::deployToRobot()
{
    if ( !isConnected() )
    {
        ui->deployButton->clearFocus();
        return;
    }
    QString value;
    value.append("V");
    value.append(ui->maximumPower->text());
    value.append('#');
    sendOneWay(value, true);
    value.clear();
    value.append("v");
    value.append(ui->minimumPower->text());
    value.append('#');
    sendOneWay(value, true);
    value.clear();
    value.append("c");
    value.append(ui->currentPowerLevel->text());
    value.append('#');
    sendOneWay(value, true);
    value.clear();
    value.append("s");
    value.append(ui->stopDistance->text());
    value.append('#');
    sendOneWay(value, true);
    value.clear();
    value.append("d");
    value.append(ui->lowPowerDistance->text());
    value.append('#');
    sendOneWay(value, true);
    ui->deployButton->clearFocus();
}

void RobotControll::fetchFromRobot()
{
    if ( !isConnected() )
    {
        ui->fetchButton->clearFocus();
        return;
    }
    QString value = sendWithReply("V#");
    ui->maximumPower->setText(value.trimmed());
    value = sendWithReply("v#");
    ui->minimumPower->setText(value.trimmed());
    value = sendWithReply("c#");
    ui->currentPowerLevel->setText(value.trimmed());
    ui->powerLevel->setRange(ui->minimumPower->text().toInt(),ui->maximumPower->text().toInt());
    ui->powerLevel->setValue(ui->minimumPower->text().toInt());
    isMinMaxPowerChanged = MIN_MAX_POWER_CHANGES::NONE;
    value = sendWithReply("s#");
    ui->stopDistance->setText(value.trimmed());
    value = sendWithReply("d#");
    ui->lowPowerDistance->setText(value.trimmed());
    ui->fetchButton->clearFocus();
}

void RobotControll::minPowerChanged()
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

void RobotControll::maxPowerChanged()
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

void RobotControll::stopDistanceChanged()
{
    if ( isDistanceChanged == DISTANCE_CHANGES::LOW_POWER )
        isDistanceChanged =  DISTANCE_CHANGES::BOTH;
    else
        isDistanceChanged = DISTANCE_CHANGES::STOP;
}

void RobotControll::lowPowerDistanceChanged()
{
    if ( isDistanceChanged == DISTANCE_CHANGES::STOP )
        isDistanceChanged = DISTANCE_CHANGES::BOTH;
    else
        isDistanceChanged = DISTANCE_CHANGES::LOW_POWER;
}

void RobotControll::sendPowerToRobotIfModified()
{
    QString value;
    switch( isMinMaxPowerChanged )
    {
    case MIN_MAX_POWER_CHANGES::MAX:
        value.clear();
        value.append("V");
        value.append(ui->maximumPower->text());
        value.append('#');
        sendOneWay(value, true);
        break;
    case MIN_MAX_POWER_CHANGES::MIN:
        value.clear();
        value.append("v");
        value.append(ui->minimumPower->text());
        value.append('#');
        sendOneWay(value, true);
        break;
    case MIN_MAX_POWER_CHANGES::MINMAX:
        value.clear();
        value.append("V");
        value.append(ui->maximumPower->text());
        value.append('#');
        sendOneWay(value, true);
        value.clear();
        value.append("v");
        value.append(ui->minimumPower->text());
        value.append('#');
        sendOneWay(value, true);
        break;
    default:
        break;
    }
    isMinMaxPowerChanged = MIN_MAX_POWER_CHANGES::NONE;
    if ( isCurrentPowerChanges )
    {
        value.clear();
        value.append("c");
        value.append(ui->currentPowerLevel->text());
        value.append('#');
        sendOneWay(value, true);
        isCurrentPowerChanges = false;
    }

    switch( isDistanceChanged )
    {
    case DISTANCE_CHANGES::STOP :
        value.clear();
        value.append("s");
        value.append(ui->stopDistance->text());
        value.append('#');
        sendOneWay(value, true);
        break;
    case DISTANCE_CHANGES::LOW_POWER:
        value.clear();
        value.append("d");
        value.append(ui->lowPowerDistance->text());
        value.append('#');
        sendOneWay(value, true);
        break;
    case DISTANCE_CHANGES::BOTH :
        value.clear();
        value.append("s");
        value.append(ui->stopDistance->text());
        value.append('#');
        sendOneWay(value, true);
        value.clear();
        value.append("d");
        value.append(ui->lowPowerDistance->text());
        value.append('#');
        sendOneWay(value, true);
        break;
    default:
        break;
    }
    isDistanceChanged = DISTANCE_CHANGES::NONE;
}

void RobotControll::loadFilePath()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Load Path File"), ".", tr("Text files (*.txt)"));
    if ( filePath != nullptr || filePath.isEmpty() )
        return;
    currentFile->setText(filePath);
}

void RobotControll::saveFilePath()
{
    if ( currentFile->text().isEmpty() )
    {
        QMessageBox::critical(this, tr("First select a file."), tr("First select a file."), QMessageBox::Ok);
    }
}

void RobotControll::saveAsFilePath()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save As Path File"), ".", tr("Text files (*.txt)"));
    if ( filePath != nullptr || filePath.isEmpty() )
        return;
    currentFile->setText(filePath);
}

void RobotControll::addPathItem()
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

void RobotControll::deletePathItem()
{
    ui->listWidget->takeItem(ui->listWidget->currentRow());
    ui->deletePathButton->clearFocus();
}

void RobotControll::movePathItem()
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

void RobotControll::replacePathItem()
{
    int currentPos = ui->listWidget->currentRow();
    if ( currentPos == -1 )
    {
        ui->replacePathButton->clearFocus();
        return;
    }
    QListWidgetItem *element = ui->listWidget->currentItem();
    element->setText(ui->newPathCommand->text());
}

void RobotControll::clearPathItems()
{
    ui->listWidget->clear();
    ui->clearPathButton->clearFocus();
}

void RobotControll::deployNavigationPath()
{
    if ( !isConnected() )
    {
        ui->deployButton->clearFocus();
        return;
    }
}

void RobotControll::fetchNavigationPath()
{
    if ( !isConnected() )
    {
        ui->fetchButton->clearFocus();
        return;
    }
}

void RobotControll::runForwardWithPath()
{
    if ( !isConnected() )
    {
        return;
    }
}

void RobotControll::runBackwardWithPath()
{
    if ( !isConnected() )
    {
        return;
    }
}

bool RobotControll::isConnected()
{
    if ( socket != nullptr && socket->isOpen() )
    {
        return true;
    }
    QMessageBox::critical(this, tr("First connect to robot."), tr("First connect to robot"), QMessageBox::Ok);
    return false;
}

void RobotControll::connectStream()
{
    if ( ui->streamingType->currentText() == QString("http") )
    {
        QUrl url("http://" + ui->ipValue->text() + ":" + ui->cameraPort->text());
        ui->streamImage->load(url);
    }
    ui->connectStreamButton->clearFocus();
}

void RobotControll::disconnectStream()
{
    ui->streamImage->close();
    ui->connectStreamButton->clearFocus();
}
