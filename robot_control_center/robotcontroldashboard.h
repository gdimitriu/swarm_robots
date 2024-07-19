#ifndef ROBOTCONTROLDASHBOARD_H
#define ROBOTCONTROLDASHBOARD_H

#include "robotcontrol.h"
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QMap>
#include "jsonrobot.h"

namespace Ui {
class RobotControlDashboard;
}

class RobotControlDashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit RobotControlDashboard(QWidget *parent = 0);
    ~RobotControlDashboard();
    void childIsClosed(RobotControl *child);
private slots:
    void launchRobot();
    void addRobot();
    void deleteRobot();
    void loadRobots();
    void saveRobots();
    void saveAsRobots();
    void displayrobotItem(QListWidgetItem *item);
private:
    void createMenus();
    void populateUi();
    void read(const QJsonObject &json);
    void write(QJsonObject &json);

    Ui::RobotControlDashboard *ui;
    QMenu *fileMenu;
    QAction *loadRobotsAction;
    QAction *saveRobotsAction;
    QAction *saveAsRobotsAction;
    QLineEdit *currentFile;
    QMap<QString, RobotControl *> launchedRobots;
    QMap<QString, JsonRobot *> robots;
};

#endif // ROBOTCONTROLDASHBOARD_H
