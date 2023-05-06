/*
    Go to thingspeak.com and create an account if you don't have one already.
    After logging in, click on the "New Channel" button to create a new channel for your data. This is where your data will be stored and displayed.
    Fill in the Name, Description, and other fields for your channel as desired, then click the "Save Channel" button.
    Take note of the "Write API Key" located in the "API keys" tab, this is the key you will use to send data to your channel.
    Replace the channelID from tab "Channel Settings" and privateKey with "Read API Keys" from "API Keys" tab.
    Replace the host variable with the thingspeak server hostname "api.thingspeak.com"
    Upload the sketch to your ESP32 board and make sure that the board is connected to the internet. The ESP32 should now send data to your Thingspeak channel at the intervals specified by the loop function.
    Go to the channel view page on thingspeak and check the "Field1" for the new incoming data.
    You can use the data visualization and analysis tools provided by Thingspeak to display and process your data in various ways.
    Please note, that Thingspeak accepts only integer values.

    You can later check the values at https://thingspeak.com/channels/2005329
    Please note that this public channel can be accessed by anyone and it is possible that more people will write their values.
 */

#include "src/WiFi.h"
#include "aws_server.h"

void setup()
{
    Serial.begin(115200);
    while(!Serial){delay(100);}

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println("******************************************************");
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);

    WiFiClient client;
    
    // SEND ACK --------------------------------------------------------------------------------------------
    Serial.print(AWS_IP);
    Serial.print(":");
    Serial.println(AWS_PORT);
    while (!client.connect(AWS_IP, AWS_PORT)) {
      Serial.println("Connection failed.");
      Serial.println("retrying...");
      delay(1000);
    }
  
    client.print("ESP32-LOCKER");
  
    digitalWrite(3, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(1, HIGH);
    delay(5000);
}

void blinkLED(int LED) {
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
    delay(250);
  }
}

void readResponse(WiFiClient *client){
  unsigned long timeout = millis();
  while(client->available() == 0){
    if(millis() - timeout > 5000){
      Serial.println(">>> Client Timeout !");
      client->stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while(client->available()) {
    String line = client->readStringUntil('\r');
    Serial.print(line);
    if (line == "cmd") {
      blinkLED(3);
      blinkLED(1);
    }
  }

  Serial.printf("\nClosing connection\n\n");
}



void loop(){
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  digitalWrite(1, LOW);

//  delay(500);
//  digitalWrite(2, LOW);
//  digitalWrite(1, LOW);

    
  WiFiClient client;
//  
//  // SEND ACK --------------------------------------------------------------------------------------------
  if (client.connect(AWS_IP, AWS_PORT)) {
      client.print("ESP32-LOCKER");
      readResponse(&client);
      Serial.println("line");
  }
  
//  client.print("ESP32-LOCKER");
  
//
//  // READ CMDS --------------------------------------------------------------------------------------------
//
////  String readRequest = "";
////
////  if (!client.connect(AWS_IP, AWS_PORT)) {
////    return;
////  }
////  
////  readResponse(&client);
//
//  // -------------------------------------------------------------------------------------------------
//  delay(1000);
//  digitalWrite(3, HIGH);
  delay(25);
}
