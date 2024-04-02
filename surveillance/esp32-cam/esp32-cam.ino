/*
 * moving robot with exp32-cam camera
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


#include <WiFi.h>
#include "configuration.h"
#include "move_commands.h"
#include "camera_streaming.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

const char* ssid     = "Skynet";
const char* password = "12345678";

WiFiServer server(SERVER_PORT);
WiFiClient client;

// for input data from wifi
boolean cleanupSerial;
char inData[SERIAL_BUFFER]; // Allocate some space for the string
char buffer[SERIAL_BUFFER];
char inChar; // Where to store the character read
int indexReceive = 0; // indexReceive into array; where to store the character

boolean hasConnection = false;

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    isValidInput = false;
    cleanupSerial = false;
#ifdef SERIAL_DEBUG    
    Serial.begin(115200);
#endif
    delay(10);

    // We start by connecting to a WiFi network
#ifdef SERIAL_DEBUG    
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
#endif
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
#ifdef SERIAL_DEBUG            
        Serial.print(".");
#endif        
    }
#ifdef SERIAL_DEBUG
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
#endif    
    setupEngines();

    initCamera();
    
    server.begin();
    client = server.available();   // listen for incoming clients
    if ( client ) {
      hasConnection = true;
#ifdef SERIAL_DEBUG      
      Serial.println("New Client.");
#endif
    }
}

void makeCleanup() {
#ifdef SERIAL_DEBUG  
  Serial.print("MakeCleanup core=");Serial.println(xPortGetCoreID());
  Serial.flush();
#endif  
  for ( indexReceive = 0; indexReceive < SERIAL_BUFFER; indexReceive++ ) {
    inData[indexReceive] = '\0';
  }
  indexReceive = 0;
  inChar ='\0';
}

void sendOK() {
  client.println("OK");
}


bool processPlatformUnsupportedCommand() {
  sendOK();
  makeCleanup();
  isValidInput = true;
  return true;
}

bool makeMove() {
  for ( int i = 0; i < SERIAL_BUFFER; i++ ) {
    buffer[i] = '\0';
  }
  if ( indexReceive > 0 ) {
     inData[indexReceive-1] = '\0';
  }
  if ( strlen(inData) == 1 ) {
    if ( inData[0] == 'I' ) {
      sprintf(buffer,"unsupported\r\n");
      client.print(buffer);
    } else if ( inData[0] == 'V' ) {      
      sprintf(buffer,"%d\r\n",maxPower);
      client.print(buffer);
    } else if ( inData[0] == 'v' ) {
      sprintf(buffer,"%d\r\n",minPower);
      client.print(buffer);
    } else if ( inData[0] == 'c' ) {
      sprintf(buffer,"%d\r\n",currentPower);
      client.print(buffer);
    }  else if ( inData[0] == 'd' ) {
      sprintf(buffer,"%d\r\n",0);
      client.print(buffer);
    } else if ( inData[0] == 's' ) {
      sprintf(buffer,"%d\r\n",0);
      client.print(buffer);
    } else if ( inData[0] == 'b' ) {  //break all engines
#ifdef SERIAL_DEBUG      
      Serial.println("Break all engines");
#endif      
      go(0,0);
    } else {
      sprintf(buffer,"%d\r\n",0);
      client.print(buffer);
      makeCleanup();
      isValidInput = false;
      return false;
    }
  } else {
      if ( inData[0] == 'V' ) {
        return setMaxPowerCommand(inData);
      } else if ( inData[0] == 'v' ) {
        return setMinPowerCommand(inData);
      } else if ( inData[0] == 'c' ) {
        return  setCurrentPowerCommand(inData);
      } else if ( inData[0] == 'd' ) {
        //this does not support low power distance
        return processPlatformUnsupportedCommand();
      } else if ( inData[0] == 's' ) {
        //this does not support stop distance
        return processPlatformUnsupportedCommand();
      } else if ( inData[0] == 'M' ) {
        return moveOrRotateUntilNextCommand(inData);
      } else {
        sprintf(buffer,"%d\r\n",0);
        client.print(buffer);
        makeCleanup();
        isValidInput = false;
        return false;
      }
    }
    isValidInput = true;
    makeCleanup();
    return true;
}

void receiveCommands() {
  while ( client.connected() ) {
    while ( client.available() > 0 ) // Don't read unless there you know there is data
    {
#ifdef SERIAL_DEBUG      
      Serial.print("receive core=");Serial.println(xPortGetCoreID());
      Serial.flush();
#endif      
      if ( indexReceive < (SERIAL_BUFFER - 1) ) // One less than the size of the array
      {
          inChar = client.read(); // Read a character
          if ( inChar=='\r' || inChar=='\n' || inChar =='\0' || inChar < 35 || inChar > 122 ) {
            continue;
          }
          //commands start with a letter capital or small
          if ( indexReceive == 0 && !( ( inChar >64 && inChar <91 ) || ( inChar > 96 && inChar<123 ) ) ) {
            continue;
          }    
          inData[indexReceive++] = inChar; // Store it
          inData[indexReceive] = '\0'; // Null terminate the string
          if ( inChar == '#' )
            break;
      } else {
          makeCleanup();
          cleanupSerial = true;
      }
    }
    if ( indexReceive >= 1 ) {
      if ( inData[indexReceive - 1] == '#' ) {
#ifdef SERIAL_DEBUG        
        Serial.print("indexReceive = ");Serial.println(indexReceive);
        Serial.print("Make move = ");Serial.println(inData);
#endif        
        makeMove();
      } else if ( cleanupSerial ) {
#ifdef SERIAL_DEBUG        
        Serial.print("indexReceive = ");Serial.println(indexReceive);
        Serial.print("Make cleanup extern = ");Serial.println(inData);
#endif        
        makeCleanup();
        cleanupSerial = false;
      } else {
        delay(10);
      }
    }
  }
 if ( !client.connected() ) {
#ifdef SERIAL_DEBUG  
  Serial.println("Client disconnected!");Serial.flush();
#endif  
  client.flush();
  client.stop();
  hasConnection = false;
 }
}
void loop()
{
  if ( !hasConnection ) {
    client = server.available();   // listen for incoming clients
    if (client) {
#ifdef SERIAL_DEBUG
        Serial.println("New Client.");
#endif        
        hasConnection = true;
        receiveCommands();
    }
  }
  
}
