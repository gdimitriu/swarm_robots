# SWARM of ROBOTS ( defence or surveillance )

## FATCS:

- all robots are connecting only to external WIFI

- some robots could have BLE but is not mandatory

- all robots have same commands [commands](docs/protocol_commands.md)


## ROBOTS:

### Surveillance robot

#### esp32-cam

This is based on ESP32-CAM [surveillance](surveillance/README.md)

It is only controlled by human using [robot_control_center](robot_control_center/README.md)

### Raspberry Pi Zero W

This is based on Raspberry Pi Zero W using RT linux [raspberrypiZeroDroid](https://github.com/gdimitriu/raspberrypiZeroDroid/tree/e1e24bd15d75077b71034d399c53751fbf6440d0/README.md)

### Raspberry pico

This holds two robots (one with round chassy and one using zumo) [raspberryPicoDroids](https://github.com/gdimitriu/raspberryPicoDroids/tree/9816ddd46bdb44ec63efd156b53c3f2ca5d9aaee/README.md)

Latest versions are based on raspberry pico W microcontroller using wifi, encoders and some type of sensors.

### Waiter Home Robot

This is an unfinished robot (work in progress) using multiple microcontrollers [waiterHomeRobot](https://github.com/gdimitriu/waiterHomeRobot/tree/354a178dd8df6931f4911bf5b2bf32de430d1c00/README.md).

It is a big robot using special chassy and engines.

It has until now:

- engine stage which contains one arduino nano for operating engiens and base sensors.

- stage2 which contains one arduino mega to control LCD, SD, sound module, RFID reader (in the future also ultrasonics).

- all microcontrollers comunicate using UART as stage2 as dispatcher.
