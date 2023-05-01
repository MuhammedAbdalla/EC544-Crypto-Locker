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

extern "C"{
#include "locker.h"
}

// webserver port number
WiFiServer wifiServer(8080);

static struct locker_reservations* reservations;
static struct user_reservation set_bit_duration(int start_idx, int end_idx);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// print all reservation details
static void print_reservations(struct locker_reservations *R) {
	printf(" LOCKER: print_reservations\n");
	struct user_reservation *element = reservations->head;
	while (element != NULL) {
		printf("  User: %s\n", element->name);
		element = element -> next;
	}
}

// searches for reservations
// returns null if none found
static struct user_reservation* search_reservation(char* name) {
	if (reservations != NULL) {
		struct user_reservation *element = reservations->head;
		// go to the end
		while (element != NULL) {
			// printf("%s\n",elgit ement -> name);
			if (strcmp(element->name, name) == 0) {
				printf(" LOCKER: search_reservation found %s\n",element->name);
				return element;
			}
			element = element -> next;
		}
	}
	return NULL;
}

// create new user reservation if name doesn't exist
// use name as client IP for uniqueness
static struct user_reservation* create_reservation(char** entry_data) {
	printf(" LOCKER: create_reservation: ");

	if (reservations == NULL) {
		reservations = malloc(sizeof(struct locker_reservations));
		printf("first entry\n");

		// malloc the linked list

		reservations->head = malloc(sizeof(struct user_reservation));

		reservations->head->name = strdup(entry_data[0]);
		reservations->head->start_time = strdup(entry_data[1]);
		reservations->head->end_time = strdup(entry_data[2]);
		reservations->head->duration_hours = atoi(entry_data[3]);

		reservations->head->next = NULL;	
	} else {
		printf("append entry\n");
		if (search_reservation(entry_data[0]) == NULL) {
			struct user_reservation* cursor = reservations->head;

			while (cursor->next!=NULL)
				cursor=cursor->next;
			

			/*
			char* name;
			char* start_time;
			char* end_time;

			int KEY;
			int start_time_idx;
			int end_time_idx;

			int duration_hours;
			int bit_duration;
			*/
		
			struct user_reservation* newReservation = malloc(sizeof(struct user_reservation));
			
			newReservation->name = strdup(entry_data[0]);
			newReservation->start_time = strdup(entry_data[1]);
			newReservation->end_time = strdup(entry_data[2]);
			newReservation->duration_hours = atoi(entry_data[3]);
			newReservation -> next = NULL;	

			cursor->next = newReservation;

			return newReservation;
		}	
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int test_reservations(void) {
  reservations = NULL;
	// reservations->head = NULL;

	for (int i = 0; i < 10; i++) {
		char buffer[32];
		sprintf(buffer,"test.client.%d",i);

		char* data[4] = {buffer, "ts", "te", "duration"};
		create_reservation(data);
	}

	printf("reservations exists");

	print_reservations(reservations);

	delete_reservation("test.client.0");

	delete_reservation("test.client.3");

	delete_reservation("test.client.10");


	for (int i = 6; i < 15; i++) {
		char buffer[32];
		sprintf(buffer,"test.client.%d",i);

		char* data[4] = {buffer, "ts", "te", "duration"};
		create_reservation(data);
	}

	print_reservations(reservations);

	printf("passed reservations test\n");

	return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  reservations = NULL;
  Serial.begin(115200);

  delay(1000);

  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  const char* server_ip = WiFi.localIP().toString().c_str();

  // while (connect_aws(server_ip) != SUCCESS){
  //   delay(1000);
  //   Serial.println("Retrying connection to AWS..");
  // }
  
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
//			print_reservations(reservations);

      // // send back a server response
      // printf("[SERVER] \"client: %s key...\"\n", buffer);
      // if (send(client_fd, ACK, strlen(ACK), 0) == -1) {
      // 	printf("\tmessage failed\n");
      // 	close(client_fd);
      // } else {
      // 	printf("\tmessage sent\n");
      // }
      
      delay(10);
    }

    client.stop();
    Serial.println("Client disconnected");
  }
}
