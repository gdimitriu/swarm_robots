"""
 moving robot raspberry pico and wifi from ESP32-CAM

 main entry point of the robot

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
import engines
import serial_commands

engines.init_engines()
serial_commands.receive_commands()
