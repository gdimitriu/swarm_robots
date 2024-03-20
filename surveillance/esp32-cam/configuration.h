/*
 * moving robot
 * Copyright 2024 Gabriel Dimitriu
 *
 * This file is part of swarm_robots project.

 * swarm_robots is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * swarm_robots is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with swarm_robots; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__
#include <WiFi.h>


#define SERIAL_BUFFER 50

#define ABSOLUTE_MAX_POWER 65550

#define SERVER_PORT 4242

/**********************************************
 * 
 * right motor pin 2
 * 
 **********************************************/
#define RIGHT_MOTOR_PIN2 12

/**********************************************
 * 
 * right motor pin 1
 * 
 **********************************************/
#define RIGHT_MOTOR_PIN1 13

/**********************************************
 * 
 * left motor pin 2
 * 
 **********************************************/
#define LEFT_MOTOR_PIN2 14

/**********************************************
 * 
 * left motor pin 1
 * 
 **********************************************/
#define LEFT_MOTOR_PIN1 15

/**********************************************
 * 
 * servo horizontally (x) pin
 * 
 **********************************************/
#define SERVO_X_PIN 16

/**********************************************
 * 
 * servo vertically (Y) pin
 * 
 **********************************************/
#define SERVO_Y_PIN 4

extern int currentPower;

extern int minPower;

extern int maxPower;

extern int indexReceive;

extern bool isValidInput;

void makeCleanup();

void sendOK();

#endif
