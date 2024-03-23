/*
 * 2 Servo calibration this is useful to put servo at center when the data is attached
 * Copyright 2024 Gabriel Dimitriu
 *
 * This file is part of swarm_robots project
  
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
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define SERVO_X_PIN 16
#define SERVO_Y_PIN 4
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte indexReceive = 0; // indexReceive into array; where to store the character

static const int servoXChannel = 4;
static const int servoYChannel = 5;
static const int freq = 50;
static const int resolution = 12;

void menu() {
  Serial.println( "--------------------------------------------------" );
  Serial.println( "Calibration of two servos");
  Serial.println( "--------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "hxxx# put the horizontally servo at xxx degree");
  Serial.println( "vxxx# put the vertically servo at xxx degree");
  Serial.println( "c# center servo");
  Serial.println( "-----------------------------" );
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(9600);
  ledcSetup(servoXChannel, freq, resolution);
  ledcAttachPin(SERVO_X_PIN, servoXChannel);
  ledcSetup(servoYChannel, freq, resolution);
  ledcAttachPin(SERVO_Y_PIN, servoYChannel);
  ledcWrite(servoXChannel, 200);
  ledcWrite(servoYChannel, 200);
  menu();
}

static void makeCleanup() {
  for (indexReceive = 0; indexReceive < 20; indexReceive++) {
    inData[indexReceive] = '\0';
  }
  indexReceive = 0;
  inChar ='0';
}

static boolean isValidNumber(char *data)
{
  if (strlen(data) == 0 ) return false;
   if(!(data[0] == '+' || data[0] == '-' || isDigit(data[0]))) return false;

   for(byte i=1;i<strlen(data);i++) {
       if(!(isDigit(data[i]) || data[i] == '.')) return false;
   }
   return true;
}

void makeMove(char *data) {
  char *realData = data;
  if (strlen(realData) > 0) {
    realData[strlen(realData) -1] = '\0';
  } else {
    return;
  }
  if (strlen(realData) == 1) {
    if (realData[0] == 'c') {
      Serial.println("Center servo");
      ledcWrite(servoXChannel, 200);
      ledcWrite(servoYChannel, 200);
    }
  } else if (strlen(realData) > 1) {
      if (realData[0] == 'h') {
        realData++;
        if (!isValidNumber(realData)) {
          makeCleanup();
          return;
        }
        int value = atoi(realData);
        Serial.print("Move to ");Serial.println(value);
        ledcWrite(servoXChannel, value);
        return;
      }
      else if (realData[0] == 'v') {
        realData++;
        if (!isValidNumber(realData)) {
          makeCleanup();
          return;
        }
        int value = atoi(realData);
        Serial.print("Move to ");Serial.println(value);
        ledcWrite(servoYChannel, value);
        return;
      }
    }
}

void receiveCommand() {
  while(Serial.available() > 0) {
     if(indexReceive < 19) // One less than the size of the array
     {
        inChar = Serial.read(); // Read a character
        if (inChar == '\r' || inChar == '\n' || inChar == '\0' || inChar < 35 || inChar > 122) {
          continue;
        }
        //commands start with a letter capital or small
        if (indexReceive == 0 && !((inChar > 64 && inChar < 91) || (inChar > 96 && inChar < 123))) {
          continue;
        }
        inData[indexReceive++] = inChar; // Store it
        inData[indexReceive] = '\0'; // Null terminate the string
     } else {
        makeCleanup();
     }
  }
  if (inData[indexReceive-1] == '#') {
    makeMove(inData);
    makeCleanup();
  }
}

void loop() {
  receiveCommand();
}
