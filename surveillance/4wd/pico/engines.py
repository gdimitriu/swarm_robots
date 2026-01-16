"""
 moving robot raspberry pico and wifi from ESP32-CAM

 engines (actual driving the engines)

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
from machine import Pin
import configuration
import math
import ioe_sr05_pio
from servoSR05 import movecenter, move90left, move90right

direction = 0
_current_left_front_encoder = 0
_current_right_front_encoder = 0
_left_front_current_distance = 0.0
_right_front_current_distance = 0.0
_current_left_rear_encoder = 0
_current_right_rear_encoder = 0
_left_rear_current_distance = 0.0
_right_rear_current_distance = 0.0
_PPI = 0.0
_human_control = True
_stop_front_left = False
_stop_front_right = False
_stop_rear_left = False
_stop_rear_right = False


def set_human_control(value):
    global _human_control
    _human_control = value


def callback(pin):
    """callback for encoders"""
    global direction
    global _current_left_front_encoder
    global _current_right_front_encoder
    global _current_left_rear_encoder
    global _current_right_rear_encoder
    if configuration.DEBUG_MODE:
        print("Callback(%s)" % pin)
    if pin is configuration.left_front_motor_encoder:
        _current_left_front_encoder += 1
    elif pin is configuration.right_front_motor_encoder:
        _current_right_front_encoder += 1
    if pin is configuration.left_rear_motor_encoder:
        _current_left_rear_encoder += 1
    elif pin is configuration.right_rear_motor_encoder:
        _current_right_rear_encoder += 1
    elif pin is configuration.left_front_sensor or pin is configuration.right_front_sensor:
        if direction == 1 and _human_control:
            go(0, 0)
    elif pin is configuration.left_rear_sensor or pin is configuration.right_rear_sensor:
        if direction == -1 and _human_control:
            go(0, 0)


def init_encoders():
    """
    initialize the encoders
    this should be called every time the move wth distance is called
    or when it is with path navigation
    """
    global _current_right_front_encoder
    global _current_left_front_encoder
    global _current_right_rear_encoder
    global _current_left_rear_encoder
    if configuration.DEBUG_MODE:
        print("InitEncoders")
    _current_left_front_encoder = 0
    _current_right_front_encoder = 0
    configuration.left_front_motor_encoder.irq(trigger=Pin.IRQ_RISING, handler=callback)
    configuration.right_front_motor_encoder.irq(trigger=Pin.IRQ_RISING, handler=callback)
    _current_left_rear_encoder = 0
    _current_right_rear_encoder = 0
    configuration.left_rear_motor_encoder.irq(trigger=Pin.IRQ_RISING, handler=callback)
    configuration.right_rear_motor_encoder.irq(trigger=Pin.IRQ_RISING, handler=callback)


def disable_encoders():
    """
    disable the encoders
    :return:
    """
    if configuration.DEBUG_MODE:
        print("DisableEncoders")
    configuration.left_front_motor_encoder.irq(handler=None)
    configuration.right_front_motor_encoder.irq(handler=None)
    configuration.left_rear_motor_encoder.irq(handler=None)
    configuration.right_rear_motor_encoder.irq(handler=None)


def init_engines():
    """
    initialize the engines and sensors
    this should be called only once
    """
    global _PPI
    if configuration.DEBUG_MODE:
        print("InitEngines")
    _PPI = configuration.RESOLUTION_ENCODER / (2 * math.pi * configuration.WHEEL_RADIUS)
    configuration.leftFrontMotorPin1.freq(configuration.PWM_FREQUENCY)
    configuration.leftFrontMotorPin2.freq(configuration.PWM_FREQUENCY)
    configuration.rightFrontMotorPin1.freq(configuration.PWM_FREQUENCY)
    configuration.rightFrontMotorPin2.freq(configuration.PWM_FREQUENCY)
    configuration.leftRearMotorPin1.freq(configuration.PWM_FREQUENCY)
    configuration.leftRearMotorPin2.freq(configuration.PWM_FREQUENCY)
    configuration.rightRearMotorPin1.freq(configuration.PWM_FREQUENCY)
    configuration.rightRearMotorPin2.freq(configuration.PWM_FREQUENCY)
    configuration.left_front_sensor.irq(trigger=Pin.IRQ_FALLING, handler=callback)
    configuration.right_front_sensor.irq(trigger=Pin.IRQ_FALLING, handler=callback)
    configuration.left_rear_sensor.irq(trigger=Pin.IRQ_FALLING, handler=callback)
    configuration.right_rear_sensor.irq(trigger=Pin.IRQ_FALLING, handler=callback)
    movecenter()


def go(left_speed=0, right_speed=0):
    global direction
    if configuration.DEBUG_MODE:
        print("Go(%g,%g)" % (left_speed, right_speed))

    if left_speed == 0 and right_speed == 0:
        configuration.leftFrontMotorPin1.duty_u16(0)
        configuration.leftFrontMotorPin2.duty_u16(0)
        configuration.rightFrontMotorPin1.duty_u16(0)
        configuration.rightFrontMotorPin2.duty_u16(0)
        configuration.leftRearMotorPin1.duty_u16(0)
        configuration.leftRearMotorPin2.duty_u16(0)
        configuration.rightRearMotorPin1.duty_u16(0)
        configuration.rightRearMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("coasting")
        direction = 0
        return

    if left_speed > 0 and right_speed > 0 and (ioe_sr05_pio.get_distance() <= configuration.STOP_DISTANCE
            or configuration.left_front_sensor.value() == 0 or configuration.right_front_sensor.value() == 0):
        configuration.leftFrontMotorPin1.duty_u16(0)
        configuration.leftFrontMotorPin2.duty_u16(0)
        configuration.rightFrontMotorPin1.duty_u16(0)
        configuration.rightFrontMotorPin2.duty_u16(0)
        configuration.leftRearMotorPin1.duty_u16(0)
        configuration.leftRearMotorPin2.duty_u16(0)
        configuration.rightRearMotorPin1.duty_u16(0)
        configuration.rightRearMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("All on zero because of front collision %g" % ioe_sr05_pio.get_distance())
        return

    if left_speed < 0 and right_speed < 0 \
            and (configuration.left_rear_sensor.value() == 0 or configuration.right_rear_sensor.value() == 0):
        configuration.leftFrontMotorPin1.duty_u16(0)
        configuration.leftFrontMotorPin2.duty_u16(0)
        configuration.rightFrontMotorPin1.duty_u16(0)
        configuration.rightFrontMotorPin2.duty_u16(0)
        configuration.leftRearMotorPin1.duty_u16(0)
        configuration.leftRearMotorPin2.duty_u16(0)
        configuration.rightRearMotorPin1.duty_u16(0)
        configuration.rightRearMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("All on zero because of rear collision %g" % ioe_sr05_pio.get_distance())
        return

    if left_speed < 0 and right_speed < 0:
        direction = -1
    elif right_speed > 0 and left_speed > 0:
        direction = 1
    if left_speed > 0:
        configuration.leftFrontMotorPin1.duty_u16(left_speed)
        configuration.leftFrontMotorPin2.duty_u16(0)
        configuration.leftRearMotorPin1.duty_u16(left_speed)
        configuration.leftRearMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("Left %g:0" % left_speed)
    else:
        configuration.leftFrontMotorPin1.duty_u16(0)
        configuration.leftFrontMotorPin2.duty_u16(-left_speed)
        configuration.leftRearMotorPin1.duty_u16(0)
        configuration.leftRearMotorPin2.duty_u16(-left_speed)
        if configuration.DEBUG_MODE:
            print("Left 0:%g" % -left_speed)
    if right_speed > 0:
        configuration.rightFrontMotorPin1.duty_u16(right_speed)
        configuration.rightFrontMotorPin2.duty_u16(0)
        configuration.rightRearMotorPin1.duty_u16(right_speed)
        configuration.rightRearMotorPin2.duty_u16(0)
        if configuration.DEBUG_MODE:
            print("Right %g:0" % right_speed)
    else:
        configuration.rightFrontMotorPin1.duty_u16(0)
        configuration.rightFrontMotorPin2.duty_u16(-right_speed)
        configuration.rightRearMotorPin1.duty_u16(0)
        configuration.rightRearMotorPin2.duty_u16(-right_speed)
        if configuration.DEBUG_MODE:
            print("Right 0:%g" % -right_speed)


def break_engines():
    global _stop_front_left
    global _stop_front_right
    global _stop_rear_left
    global _stop_rear_right
    if configuration.DEBUG_MODE:
        print("BreakEngines")
    configuration.leftFrontMotorPin1.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.leftFrontMotorPin2.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.rightFrontMotorPin1.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.rightFrontMotorPin2.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.leftRearMotorPin1.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.leftRearMotorPin2.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.rightRearMotorPin1.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    configuration.rightRearMotorPin2.duty_u16(configuration.ABSOLUTE_MAX_POWER)
    _stop_front_left = True
    _stop_front_right = True
    _stop_rear_left = True
    _stop_rear_right = True


def stop_left_front_engine():
    configuration.leftFrontMotorPin1.duty_u16(0)
    configuration.leftFrontMotorPin2.duty_u16(0)


def stop_right_front_engine():
    configuration.rightFrontMotorPin1.duty_u16(0)
    configuration.rightFrontMotorPin2.duty_u16(0)


def stop_left_rear_engine():
    configuration.leftRearMotorPin1.duty_u16(0)
    configuration.leftRearMotorPin2.duty_u16(0)


def stop_right_rear_engine():
    configuration.rightRearMotorPin1.duty_u16(0)
    configuration.rightRearMotorPin2.duty_u16(0)


def move_with_distance(move_data, current_power, run_distance):
    global _left_front_current_distance
    global _right_front_current_distance
    global _left_rear_current_distance
    global _right_rear_current_distance
    global _PPI
    global _current_left_front_encoder
    global _current_right_front_encoder
    global _current_left_rear_encoder
    global _current_right_rear_encoder
    global _stop_front_left
    global _stop_front_right
    global _stop_rear_left
    global _stop_rear_right
    if configuration.DEBUG_MODE:
        print("Moving with distance %g" % move_data)
    _left_front_current_distance = 0.0
    _right_front_current_distance = 0.0
    _left_rear_current_distance = 0.0
    _right_rear_current_distance = 0.0
    _stop_front_left = False
    _stop_front_right = False
    _stop_rear_left = False
    _stop_rear_right = False
    if move_data > 0:
        distance = move_data
        go(current_power, current_power)
    elif move_data < 0:
        distance = -move_data
        go(-current_power, -current_power)
    else:
        go(0, 0)
        return
    while not _stop_front_left or not _stop_front_right or not _stop_rear_left or not _stop_rear_right:
        if not _stop_front_left:
            _left_front_current_distance = _current_left_front_encoder / _PPI
            if (distance - _left_front_current_distance) <= 0.02:
                stop_left_front_engine()
                _stop_front_left = True
        if not _stop_front_right:
            _right_front_current_distance = _current_right_front_encoder / _PPI
            if (distance - _right_front_current_distance) <= 0.02:
                stop_right_front_engine()
                _stop_front_right = True
        if not _stop_rear_left:
            _left_rear_current_distance = _current_left_rear_encoder / _PPI
            if (distance - _left_rear_current_distance) <= 0.02:
                stop_left_rear_engine()
                _stop_rear_left = True
        if not _stop_rear_right:
            _right_rear_current_distance = _current_right_rear_encoder / _PPI
            if (distance - _right_rear_current_distance) <= 0.02:
                stop_right_rear_engine()
                _stop_rear_right = True
    go(0, 0)
    run_distance[0] = _left_front_current_distance
    run_distance[1] = _right_front_current_distance
    run_distance[2] = _left_rear_current_distance
    run_distance[3] = _right_rear_current_distance


def rotate_90_left(current_power):
    global _stop_front_left
    global _stop_front_right
    global _stop_rear_left
    global _stop_rear_right
    global _current_left_front_encoder
    global _current_right_front_encoder
    global _current_left_rear_encoder
    global _current_right_rear_encoder
    _stop_front_left = False
    _stop_front_right = False
    _stop_rear_left = False
    _stop_rear_right = False
    if configuration.DEBUG_MODE:
        print("Rotate 90 left with left=%g and right=%g" % (configuration.COUNT_ROTATE_90_LEFT,
                                                            configuration.COUNT_ROTATE_90_RIGHT))
    go(-current_power, current_power)
    while not _stop_front_left or not _stop_front_right or not _stop_rear_left or not _stop_rear_right:
        if not _stop_front_left:
            if _current_left_front_encoder >= configuration.COUNT_ROTATE_90_LEFT:
                stop_left_front_engine()
                _stop_front_left = True
        if not _stop_front_right:
            if _current_right_front_encoder >= configuration.COUNT_ROTATE_90_RIGHT:
                stop_right_front_engine()
                _stop_front_right = True
        if not _stop_rear_left:
            if _current_left_rear_encoder >= configuration.COUNT_ROTATE_90_LEFT:
                stop_left_rear_engine()
                _stop_rear_left = True
        if not _stop_rear_right:
            if _current_right_rear_encoder >= configuration.COUNT_ROTATE_90_RIGHT:
                stop_right_rear_engine()
                _stop_rear_right = True
    go(0, 0)


def rotate_90_right(current_power):
    global _stop_front_left
    global _stop_front_right
    global _stop_rear_left
    global _stop_rear_right
    global _current_left_front_encoder
    global _current_right_front_encoder
    global _current_left_rear_encoder
    global _current_right_rear_encoder
    _stop_front_left = False
    _stop_front_right = False
    _stop_rear_left = False
    _stop_rear_right = False
    if configuration.DEBUG_MODE:
        print("Rotate 90 right with left=%g and right=%g" % (configuration.COUNT_ROTATE_90_LEFT,
                                                             configuration.COUNT_ROTATE_90_RIGHT))
    go(current_power, -current_power)
    while not _stop_front_left or not _stop_front_right or not _stop_rear_left or not _stop_rear_right:
        if not _stop_front_left:
            if _current_left_front_encoder >= configuration.COUNT_ROTATE_90_LEFT:
                stop_left_front_engine()
                _stop_front_left = True
        if not _stop_front_right:
            if _current_right_front_encoder >= configuration.COUNT_ROTATE_90_RIGHT:
                stop_right_front_engine()
                _stop_front_right = True
        if not _stop_rear_left:
            if _current_left_rear_encoder >= configuration.COUNT_ROTATE_90_LEFT:
                stop_left_rear_engine()
                _stop_rear_left = True
        if not _stop_rear_right:
            if _current_right_rear_encoder >= configuration.COUNT_ROTATE_90_RIGHT:
                stop_right_rear_engine()
                _stop_rear_right = True
    go(0, 0)


def rotate_degree(rotate_data, current_power):
    global _stop_front_left
    global _stop_front_right
    global _stop_rear_left
    global _stop_rear_right
    global _current_left_front_encoder
    global _current_right_front_encoder
    global _current_left_rear_encoder
    global _current_right_rear_encoder
    _stop_front_left = False
    _stop_front_right = False
    _stop_rear_left = False
    _stop_rear_right = False
    if rotate_data < 0:
        _left_front_target_counts = configuration.COUNT_ROTATE_90_LEFT * (-rotate_data) / 90
        _right_front_target_counts = configuration.COUNT_ROTATE_90_RIGHT * (-rotate_data) / 90
        _left_rear_target_counts = configuration.COUNT_ROTATE_90_LEFT * (-rotate_data) / 90
        _right_rear_target_counts = configuration.COUNT_ROTATE_90_RIGHT * (-rotate_data) / 90
        if configuration.DEBUG_MODE:
            print("Rotate %g left with left front=%g and right front=%g left rear=%g right rear=%g" %
                  (rotate_data, _left_front_target_counts, _right_front_target_counts,
                   _left_rear_target_counts, _right_rear_target_counts))
    else:
        _left_front_target_counts = configuration.COUNT_ROTATE_90_LEFT * rotate_data / 90
        _right_front_target_counts = configuration.COUNT_ROTATE_90_RIGHT * rotate_data / 90
        _left_rear_target_counts = configuration.COUNT_ROTATE_90_LEFT * rotate_data / 90
        _right_rear_target_counts = configuration.COUNT_ROTATE_90_RIGHT * rotate_data / 90
        if configuration.DEBUG_MODE:
            print("Rotate %g right with left front=%g and right front=%g left rear=%g right rear=%g" %
                  (rotate_data, _left_front_target_counts, _right_front_target_counts,
                   _left_rear_target_counts, _right_rear_target_counts))
    if rotate_data < 0:
        go(-current_power, current_power)
    else:
        go(current_power, -current_power)
    while not _stop_front_left or not _stop_front_right or not _stop_rear_left or not _stop_rear_right:
        if not _stop_front_left:
            if _current_left_front_encoder >= _left_front_target_counts:
                stop_left_front_engine()
                _stop_front_left = True
        if not _stop_front_right:
            if _current_right_front_encoder >= _right_front_target_counts:
                stop_right_front_engine()
                _stop_front_right = True
        if not _stop_rear_left:
            if _current_left_rear_encoder >= _left_rear_target_counts:
                stop_left_rear_engine()
                _stop_rear_left = True
        if not _stop_rear_right:
            if _current_right_rear_encoder >= _right_rear_target_counts:
                stop_right_rear_engine()
                _stop_rear_right = True
    go(0, 0)
