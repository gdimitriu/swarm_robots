# SWARM of ROBOTS ( defence or surveillance )

## FATCS:

- all robots are connecting only to external WIFI

- some robots could have BLE but is not mandatory

- all robots have same commands [commands](docs/protocol_commands.md)


## ROBOTS:

### 4wd_esp32_wroom

It uses ESP32 WROOM and it has proximity sensors.

It could be controlled only by human using WIFI.

### Surveillance robot

#### esp32-cam

[surveillance](surveillance/README.md)

They could be controlled by [robot_control_center](robot_control_center/README.md)

### Raspberry Pi Zero W

This is based on Raspberry Pi Zero W using RT linux [raspberrypiZeroDroid](https://github.com/gdimitriu/raspberrypiZeroDroid)

### Raspberry pico

This holds two robots (one with round chassy and one using zumo) [raspberryPicoDroids](https://github.com/gdimitriu/raspberryPicoDroids)

Latest versions are based on raspberry pico W microcontroller using wifi, encoders and some type of sensors.

### Waiter Home Robot

This is an unfinished robot (work in progress) using multiple microcontrollers [waiterHomeRobot](https://github.com/gdimitriu/waiterHomeRobot).

It is a big robot using special chassy and engines.

It has until now:

- engine stage which contains one arduino nano for operating engiens and base sensors.

- stage2 which contains one arduino mega to control LCD, SD, sound module, RFID reader (in the future also ultrasonics).

- all microcontrollers comunicate using UART as stage2 as dispatcher.
