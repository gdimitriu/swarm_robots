"""
 moving robot raspberry pico and wifi from ESP32-CAM

oantiltservos (moving servos for camera)

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
from servo_lib import Servo
import configuration


class PanTiltServos:
    min_angle = 0
    max_angle = 180

    def __init__(self):
        self.horizontal_servo = Servo(configuration.HORIZONTAL_SERVO_PIN)
        self.vertical_servo = Servo(configuration.VERTICAL_SERVO_PIN)
        self.horizontal_servo.move(90)
        self.vertical_servo.move(90)

    def move_horizontally(self, value):
        new_angle = self.horizontal_servo.current_angle + value
        if new_angle > self.max_angle or new_angle < self.min_angle:
            return
        self.horizontal_servo.move(new_angle)

    def move_vertically(self, value):
        new_angle = self.vertical_servo.current_angle + value
        if new_angle > self.max_angle or new_angle < self.min_angle:
            return
        self.vertical_servo.move(new_angle)
