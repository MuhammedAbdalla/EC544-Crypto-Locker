/*
 * Code from: https://techtutorialsx.com/2017/11/13/esp32-arduino-setting-a-socket-server/
 * Needed to connect to WiFi and open a socket easier than programming in C :/
 */

// Include C Libraries
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/types.h>
// #include <unistd.h>

// arduino wifi library
#include "src/WiFi.h"
#include <String>

// webserver port number
WiFiServer wifiServer(SERVER_PORT);
  
void setup()
{
  reservations = NULL;
  Serial.begin(115200);

  delay(1000);

  WiFi.begin(SSID);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  String SERVER_IP = WiFi.localIP().toString();
  String msg = "INIT_AWS|"+SERVER_IP;
  Serial.print("Device IP: ");
  Serial.print(SERVER_IP);
  Serial.print("|");
  Serial.println(SERVER_PORT);
  
  while (connect_aws(SERVER_IP.c_str(), msg.c_str()) != SUCCESS){
    delay(1000);
    Serial.println("Retrying connection to AWS..");
  }

  wifiServer.begin();
}

void loop()
{
	char buffer[1024] = {0};

  // is there a client
  WiFiClient client = wifiServer.available();

  if (client)
  {

    while (client.connected()) {
      Serial.print("client connected\n data stream: ");
      for (int i = 0; client.available() > 0; i++) {
        char c = client.read();
        Serial.print(c);
        buffer[i] = c;  
      }

      Serial.println();

      client.write(buffer, strlen(buffer));

      	if (strcmp(buffer, "CREATE") == 0) {
        Serial.println("create");
				// create_reservation(data);
		} else if (strcmp(buffer, "DELETE") == 0) {
        Serial.println("delete");
				// delete_reservation(buffer);
		} else if (strcmp(buffer, "MODIFY") == 0) {
        Serial.println("modify");
		} else {

		}
		print_reservations(reservations);

      // // send back a server response
      // printf("[SERVER] \"client: %s key...\"\n", buffer);
      // if (send(client_fd, ACK, strlen(ACK), 0) == -1) {
      // 	printf("\tmessage failed\n");
      // 	close(client_fd);
      // } else {
      // 	printf("\tmessage sent\n");
      // }
      
      
    }
    
    delay(10);
    
    client.stop();
    Serial.println("Client disconnected");
  }
}
