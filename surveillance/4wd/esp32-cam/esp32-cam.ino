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
#include "camera_streaming.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

WiFiServer server(SERVER_PORT);
WiFiClient client;

// for input data from wifi
boolean cleanupSerial;
char inData[SERIAL_BUFFER]; // Allocate some space for the string
char inPicoData[SERIAL_BUFFER];
char inChar; // Where to store the character read
int indexReceive = 0; // indexReceive into array; where to store the character

bool isValidInput = false;
boolean hasConnection = false;
int indexPicoReceive = 0;
char inPicoChar;

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    isValidInput = false;
    cleanupSerial = false;
    Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
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
        for ( int i  = 0; i < SERIAL_BUFFER; i++ ) {
          inPicoData[i] = '\0';
        }
        indexPicoReceive = 0;
        Serial2.print(inData);
        makeCleanup();
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
    while(Serial2.available() > 0)
    {
      inPicoChar = Serial2.read();
#ifdef SERIAL_DEBUG 
      Serial.print(inPicoChar);
#endif
      inPicoData[indexPicoReceive++] = inPicoChar;
      if (inPicoChar == '\n')
      {
        inPicoData[indexPicoReceive] = '\0';
#ifdef SERIAL_DEBUG 
        Serial.print(inPicoData);
#endif        
        client.print(inPicoData);
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
    client = server.available();   // listen for incoming clients on
    if (client) {
#ifdef SERIAL_DEBUG
        Serial.println("New Client.");
#endif        
        hasConnection = true;
        receiveCommands();
    }
  }
  
}
