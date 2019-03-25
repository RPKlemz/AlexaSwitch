#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
//#include <Hash.h>
#include <ArduinoJson.h> // get it from https://arduinojson.org/ or install via Arduino library manager

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define MyApiKey "yourApiKey" // From sinric.com dashboard

#define MySSID "yourSSID" 
#define MyWifiPassword "yourWifiPassword"

Servo myservo;  // Create servo object to control a servo
int pos = 15;    // Variable to store the servo position

void turnOn(String MyDeviceID) {
  if (MyDeviceID == "5axxxxxxxxxxxxxxxxxxx") // Device ID of first device
  {  
   Serial.print("Turn ON first device with ID: ");
   Serial.println(MyDeviceID);
   
   myservo.attach(14);  // Attaches the servo on pin 12 to the servo object

   for (pos = 15; pos <= 90; pos += 1)// goes from 0 degrees to 180 degrees
   {
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
   }
  
   for (pos = 90; pos >= 15; pos -= 1) // goes from 180 degrees to 0 degrees
   { 
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
   }

   myservo.detach();  // attaches the servo on pin 12 to the servo object
  }
  else if (MyDeviceID == "5axxxxxxxxxxxxxxxxxxx") // Device ID of second device
  { 
   Serial.print("Turn ON second device with ID: ");
   Serial.println(MyDeviceID);
  }
  // else if (MyDeviceID == "5axxxxxxxxxxxxxxxxxxx") // TODO: Add more devices if you want ... third device ....
  // { 
  //   Serial.print("Turn ON third device with ID: ");
  //   Serial.println(MyDeviceID);
  // } 
  else {
     Serial.print("Turn ON for unknown device with ID: ");
     Serial.println(MyDeviceID);    
  }     
}

void turnOff(String MyDeviceID) {
   if (MyDeviceID == "5c8436a720b6086935c7b26d") // Device ID of first device
   {  
     Serial.print("Turn OFF first device with ID: ");
     Serial.println(MyDeviceID);
   } 
   else if (MyDeviceID == "5axxxxxxxxxxxxxxxxxxx") // Device ID of second device
   { 
     Serial.print("Turn OFF second device with ID: ");
     Serial.println(MyDeviceID);
  }
  // else if (MyDeviceID == "5axxxxxxxxxxxxxxxxxxx") // TODO: Add more devices if you want ... third device ....
  // { 
  //   Serial.print("Turn OFF third device with ID: ");
  //   Serial.println(MyDeviceID);
  // } 
  else {
     Serial.print("Turn OFF for unknown device with ID: ");
     Serial.println(MyDeviceID);    
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payload for ON :
        //{"deviceId":"5a2b908a74a6703928abf38b","powerState":"ON"}
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload); 
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        if(action == "setPowerState") {
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        } else if (action == "test") {
          Serial.printf("Successfully received test command from sinric.com\n");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
}

void loop() {
  webSocket.loop();
}
