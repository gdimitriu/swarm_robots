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

This is based on Raspberry Pi Zero W using RT linux [raspberrypiZeroDroid](raspberrypiZeroDroid/README.md)

### Raspberry pico

This holds two robots (one with round chassy and one using zumo) [raspberryPicoDroids](raspberryPicoDroids/README.md)

Latest versions are based on raspberry pico W microcontroller using wifi, encoders and some type of sensors.

### Waiter Home Robot

This is an unfinished robot (work in progress) using multiple microcontrollers [waiterHomeRobot](waiterHomeRobot/README.md).

It is a big robot using special chassy and engines.

It has until now:

- engine stage which contains one arduino nano for operating engiens and base sensors.

- stage2 which contains one arduino mega to control LCD, SD, sound module, RFID reader (in the future also ultrasonics).

- all microcontrollers comunicate using UART as stage2 as dispatcher.
