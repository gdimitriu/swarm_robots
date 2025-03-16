"""
 moving robot raspberry pico and wifi

 configuration (this hold the configuration and main settings of the robot)

 Copyright 2025 Gabriel Dimitriu
  This file is part of swarm_robots project.

 swarm_robots is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 swarm_robots is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with swarm_robots; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
"""
from machine import Pin, PWM
SSID = 'YOUR-SSID'
PASSWORD = 'YOUR_PASSWORD'
PORT = 4242
ABSOLUTE_MAX_POWER = 65025
MAX_POWER = 65025
MIN_POWER = 30000
RESOLUTION_ENCODER = 20
WHEEL_RADIUS = 3.25
COUNT_ROTATE_90_LEFT = 16
COUNT_ROTATE_90_RIGHT = 16
# left front engine
leftFrontMotorPin1 = PWM(Pin(16))
leftFrontMotorPin2 = PWM(Pin(17))
left_front_motor_encoder = Pin(15, Pin.IN, Pin.PULL_UP)

# right front engine
rightFrontMotorPin1 = PWM(Pin(10))
rightFrontMotorPin2 = PWM(Pin(11))
right_front_motor_encoder = Pin(13, Pin.IN, Pin.PULL_UP)

# left rear engine
leftRearMotorPin1 = PWM(Pin(18))
leftRearMotorPin2 = PWM(Pin(19))
left_rear_motor_encoder = Pin(1, Pin.IN, Pin.PULL_UP)

# right rear engine
rightRearMotorPin1 = PWM(Pin(8))
rightRearMotorPin2 = PWM(Pin(9))
right_rear_motor_encoder = Pin(2, Pin.IN, Pin.PULL_UP)

STOP_DISTANCE = 5
LOW_POWER_DISTANCE = 15
IOE_SR05_DISABLE = True
# servo
servoPin = PWM(Pin(22))
# IOE-SR05
PIO_RX_PIN = Pin(5, Pin.IN, Pin.PULL_UP)
SR05EN = Pin(4, Pin.OUT)
SR05EN.high()
# IOE-SR05
MAX_RANGE_SENSOR = 200

DEBUG_MODE = True

PWM_FREQUENCY = 4000  # 4800000 4.8Mhz

# sensors
left_front_sensor = Pin(14, Pin.IN, Pin.PULL_UP)
right_front_sensor = Pin(12, Pin.IN, Pin.PULL_UP)
left_rear_sensor = Pin(0, Pin.IN, Pin.PULL_UP)
right_rear_sensor = Pin(6, Pin.IN, Pin.PULL_UP)
