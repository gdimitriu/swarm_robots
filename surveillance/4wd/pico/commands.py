"""
 moving robot raspberry pico and wifi from ESP32-CAM

 commands (the actual driver of the commands received from serial from ESP32-CAM wifi)

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
from engines import break_engines, go, set_human_control, move_with_distance
from engines import rotate_90_left, rotate_90_right, rotate_degree
from engines import init_encoders, disable_encoders
from pantiltservos import PanTiltServos
import configuration


class Command:
    def __init__(self, sock):
        self.current_power = configuration.MAX_POWER
        self.min_power = configuration.MIN_POWER
        self.max_power = configuration.MAX_POWER
        self.stop_distance = configuration.STOP_DISTANCE
        self.low_power_distance = configuration.LOW_POWER_DISTANCE
        self.sock = sock
        self.path_navigation = None
        self.pan_tilt_camera = PanTiltServos()

    def set_path_navigation(self, path_navigation):
        self.path_navigation = path_navigation

    def move_data(self, request):
        """
        move the robot according to the request
        :param request: socket
        """
        if configuration.DEBUG_MODE:
            print("MoveData(%s)" % request)
        if len(request) == 1:
            self.command_without_data(request)
        else:
            self.command_with_data(request)

    def command_without_data(self, request):
        if configuration.DEBUG_MODE:
            print("CommandWithoutData(%s)" % request)
        if request == 'v':
            self.sock.send("%g\r\n" % self.min_power)
        elif request == 'V':
            self.sock.send("%g\r\n" % self.max_power)
        elif request == 'c':
            self.sock.send("%g\r\n" % self.current_power)
        elif request == 'd':
            self.sock.send("%g\r\n" % self.low_power_distance)
        elif request == 's':
            self.sock.send("%g\r\n" % self.stop_distance)
        elif request == 'b':
            break_engines()
        elif request == 'D':
            self.path_navigation.move_direct()
            self.sock.send("OK\r\n")
        elif request == 'B':
            self.path_navigation.move_reverse()
            self.sock.send("OK\r\n")
        elif request == 'n':
            self.path_navigation.clear_navigation()
            self.sock.send("OK\r\n")

    def command_with_data(self, request, is_auto=False):
        if configuration.DEBUG_MODE:
            print("CommandWithData(%s)" % request)
        if request[0] == 'V':
            value = int(request[1:])
            if value <= configuration.ABSOLUTE_MAX_POWER:
                self.max_power = value
            self.sock.send("OK\r\n")
        elif request[0] == 'v':
            self.min_power = int(request[1:])
            self.sock.send("OK\r\n")
        elif request[0] == 'c':
            if self.max_power >= int(request[1:]) >= self.min_power:
                self.current_power = int(request[1:])
                self.sock.send("OK\r\n")
            else:
                self.sock.send("OK\r\n")
        elif request[0] == 'd':
            if int(request[1:]) <= configuration.MAX_RANGE_SENSOR:
                self.low_power_distance = int(request[1:])
            self.sock.send("OK\r\n")
        elif request[0] == 's':
            if int(request[1:]) <= configuration.MAX_RANGE_SENSOR:
                self.stop_distance = int(request[1:])
            self.sock.send("OK\r\n")
        elif request[0] == 'M':
            set_human_control(True)
            values = request[1:].split(",")
            if len(values) == 2:
                rotate_data = int(values[1])
                move_data = int(values[0])
                if move_data == 0 and rotate_data == 0:
                    go(0, 0)
                elif move_data == 0:
                    if rotate_data < 0:
                        go(-self.current_power, self.current_power)
                    else:
                        go(self.current_power, -self.current_power)
                elif rotate_data == 0:
                    if move_data > 0:
                        go(self.current_power, self.current_power)
                    else:
                        go(-self.current_power, -self.current_power)
        elif request[0] == 'm':
            set_human_control(False)
            init_encoders()
            values = request[1:].split(",")
            if len(values) == 2:
                rotate_data = int(values[1])
                move_data = float(values[0])
                if abs(move_data) <= 0.01 and rotate_data == 0:
                    go(0, 0)
                elif rotate_data == 0:
                    run_distance = {}
                    move_with_distance(move_data, self.current_power, run_distance)
                    if not is_auto:
                        self.sock.send("I%f,%f,%f,%f\r\n" % (run_distance[0], run_distance[1], run_distance[2], run_distance[3]))
                else:
                    if rotate_data == -90:  # left
                        rotate_90_left(self.current_power)
                    elif rotate_data == 90:  # right
                        rotate_90_right(self.current_power)
                    else:
                        rotate_degree(rotate_data, self.current_power)
            disable_encoders()
        elif request[0] == 'N':
            if request[1] == 'D':
                self.path_navigation.move_direct_with_file(request[2:])
            elif request[1] == 'B':
                self.path_navigation.move_reverse_with_file(request[2:])
            elif request[1] == 'f':
                if request[2] == 's':
                    self.path_navigation.save_path_file(request[3:])
                elif request[2] == 'l':
                    self.path_navigation.load_path_file(request[3:])
                elif request[2] == 'r':
                    self.path_navigation.remove_path_file(request[3:])
                elif request[2] == 'b':
                    self.path_navigation.start_write_path(request[3:])
                elif request[2] == 'e':
                    self.path_navigation.end_write_path()
            else:
                self.path_navigation.add_path_command(request[1:])
            self.sock.send("OK\r\n")
        elif request[0] == 'T':
            if request[1] == 'x':
                if configuration.DEBUG_MODE:
                    print(request)
                self.pan_tilt_camera.move_horizontally(int(request[2:]))
            elif request[1] == 'y':
                self.pan_tilt_camera.move_vertically(int(request[2:]))
        else:  # unsupported command
            self.sock.send("OK\r\n")
