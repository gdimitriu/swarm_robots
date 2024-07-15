#include "robotcontroldashboard.h"
#include "ui_robotcontroldashboard.h"

#include <QLabel>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

RobotControlDashboard::RobotControlDashboard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RobotControlDashboard)
{
    ui->setupUi(this);

    createMenus();
    populateUi();
}

RobotControlDashboard::~RobotControlDashboard()
{
    delete ui;
}

void RobotControlDashboard::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    loadRobotsAction = new QAction(tr("&Load Robots"), this);
    loadRobotsAction->setShortcut(tr("Ctrl+L"));
    loadRobotsAction->setStatusTip(tr("Load a file which contains the robots definitions"));
    connect(loadRobotsAction, SIGNAL(triggered(bool)), this, SLOT(loadRobots()));
    fileMenu->addAction(loadRobotsAction);
    saveRobotsAction = new QAction(tr("&Save Robots File"), this);
    saveRobotsAction->setShortcut(tr("Ctrl+S"));
    saveRobotsAction->setStatusTip(tr("Save a file which contains the robots definitions"));
    connect(saveRobotsAction, SIGNAL(triggered(bool)), this, SLOT(saveRobots()));
    fileMenu->addAction(saveRobotsAction);
    saveAsRobotsAction = new QAction(tr("Save As Robots File"), this);
    saveAsRobotsAction->setShortcut(tr("Ctrl+A"));
    saveAsRobotsAction->setStatusTip(tr("Save a new file which contains the robot path"));
    connect(saveAsRobotsAction, SIGNAL(triggered(bool)), this, SLOT(saveAsRobots()));
    fileMenu->addAction(saveAsRobotsAction);
}

void RobotControlDashboard::populateUi()
{
    ui->streamingType->addItem("http","http");
    ui->streamingType->addItem("vlc", "vlc");
    QLabel *label = new QLabel(tr("Current path file: "), this);
    statusBar()->addWidget(label);
    currentFile = new QLineEdit(this);
    currentFile->setReadOnly(true);
    statusBar()->addWidget(currentFile);
    connect(ui->launchButton, SIGNAL(clicked(bool)), this, SLOT(launchRobot()));
    connect(ui->addRobotButton, SIGNAL(clicked(bool)), this, SLOT(addRobot()));
    connect(ui->robotList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(displayrobotItem(QListWidgetItem*)));

}

void RobotControlDashboard::launchRobot()
{
    if ( !ui->robotNameValue->text().isEmpty() )
    {
        RobotControl *robot = new RobotControl(this);
        launchedRobots.insert(ui->robotNameValue->text(), robot);
        robot->setIpValue(ui->ipValue->text());
        robot->setPortValue(ui->portValue->text());
        robot->setWindowTitle(ui->robotNameValue->text());
        if ( ui->hasCamera->isChecked() )
        {
            robot->setCameraPort(ui->cameraPortValue->text());
            robot->setCameraProtocol(ui->streamingType->currentText());
            if ( ui->hasCommands->isChecked() )
            {
                robot->setCameraCommands("Yes");
            }
            else
            {
                robot->setCameraCommands("No");
            }
        }
        else
        {
            robot->setCameraCommands("No");
        }
        robot->show();
    }
    ui->launchButton->clearFocus();
}

void RobotControlDashboard::addRobot()
{
    ui->addRobotButton->clearFocus();
}

void RobotControlDashboard::loadRobots()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load file with robots"), ".", tr("json files (*.json)"));
    if ( !fileName.isEmpty() ) {
        ui->robotList->clear();
        robots.clear();
        QFile loadFile(fileName);
        if (!loadFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open save file.");
            return;
        }
        QByteArray saveData = loadFile.readAll();

        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        read(loadDoc.object());
        currentFile->clear();
        currentFile->setText(fileName);
    }
}

void RobotControlDashboard::read(const QJsonObject &json)
{
    QJsonArray robotsArray = json["robots"].toArray();

    for (int robotsIndex = 0; robotsIndex < robotsArray.size(); ++robotsIndex) {
        QJsonObject levelObject = robotsArray[robotsIndex].toObject();
        JsonRobot *robot = new JsonRobot(this);
        robot->read(levelObject);
        robots.insert(robot->getName(), robot);
        ui->robotList->addItem(robot->getName());
    }
}

void RobotControlDashboard::saveRobots()
{
    if ( currentFile->text().isEmpty() )
    {
        saveAsRobots();
        return;
    }
    QFile saveFile(currentFile->text());
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }
    QJsonObject object;
    write(object);
    QJsonDocument saveDoc(object);
    saveFile.write(saveDoc.toJson());
}

void RobotControlDashboard::saveAsRobots()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file with robots"), ".", tr("json files (*.json)"));
    if ( !fileName.isEmpty() ) {
        QFile saveFile(fileName);
        if (!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open save file.");
            return;
        }
        QJsonObject object;
        write(object);
        QJsonDocument saveDoc(object);
        saveFile.write(saveDoc.toJson());
        currentFile->insert(fileName);
    }
}

void RobotControlDashboard::displayrobotItem(QListWidgetItem *item)
{
    QMap<QString, JsonRobot *>::ConstIterator iterator = robots.find(item->text());
    JsonRobot *robot = *iterator;
    ui->robotNameValue->setText(robot->getName());
    ui->ipValue->setText(robot->getIp());
    ui->portValue->setText(robot->getPort());
    if ( robot->hasCamera().compare("yes",Qt::CaseInsensitive) == 0 )
    {
        ui->hasCamera->setChecked(true);
        ui->noCamera->setChecked(false);
        ui->cameraPortValue->setText(robot->cameraPort());
        ui->streamingType->setCurrentIndex(ui->streamingType->findText(robot->cameraProtocol()));
        if ( robot->hasStartStop().compare("yes", Qt::CaseInsensitive) == 0 )
        {
            ui->hasCommands->setChecked(true);
            ui->noCommands->setChecked(false);
        }
    }
    else
    {
        ui->hasCamera->setChecked(false);
        ui->noCamera->setChecked(true);
        ui->cameraPortValue->clear();
        ui->hasCommands->setChecked(false);
        ui->noCommands->setChecked(true);
    }
}

void RobotControlDashboard::write(QJsonObject &json)
{
    QJsonArray robotsArray;
    foreach (const JsonRobot *robot, robots) {
        QJsonObject robotObject;
        robot->write(robotObject);
        robotsArray.append(robotObject);
    }
    json["robots"] = robotsArray;
}

void RobotControlDashboard::childIsClosed(RobotControl *child)
{
    launchedRobots.erase(launchedRobots.find(child->windowTitle()));
    delete child;
}
