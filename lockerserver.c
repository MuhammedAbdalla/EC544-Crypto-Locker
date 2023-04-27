// source material
// https://www.geeksforgeeks.org/socket-programming-cc/

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
// socket
#include <sys/socket.h>

// waitpid
#include <sys/wait.h> 

// for the signal handling
#include <signal.h>
#include "lockerserver.h"



pid_t pid, childpid;     
int status;

int execute_sys_cmd(char* text, char* error) {
	char* cmd;
	strcpy(cmd, text);
	if (system(cmd) == -1) {
		perror(error);
		return FAILURE;
	}

	return SUCCESS;
}

int main(int argc, char const* argv[]) {
	struct sockaddr_in address;
	struct hostent *host_entry;

	int8_t server_fd, new_socket, valread, opt, addrlen;
	char *ACK, *server_ip, host[256];
	char buffer[1024] = {0};

	opt = 1;
	addrlen = sizeof(address);

	ACK = "ACK";

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		perror("socket failed");
		return FAILURE;
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		return FAILURE;
	}

	// prepare the the socket structure
	address.sin_family = AF_INET; // inet_addr("IP ADDRESS")
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// // get IP and Host Name of Server (self)
	// if (gethostname(host, sizeof(host)) == -1) {
	// 	perror("gethostname");
	// 	return FAILURE;
	// }

	// if ((host_entry = gethostbyname(host)) == NULL) {
	// 	perror("gethostentry");
	// 	return FAILURE;
	// }

	// int ip_list_size = (sizeof(host_entry->h_addr_list)/sizeof(host_entry->h_addr_list[0]));
	// for (int i = 0; i < ip_list_size; i++) {
	// 	if ( (server_ip = inet_ntoa( *((struct in_addr*) host_entry->h_addr_list[0]))) != NULL) {
	// 		printf("IP %d: %s\n", i, server_ip);
	// 	}
	// }

	// if ( (server_ip = inet_ntoa( *((struct in_addr*) host_entry->h_addr_list[0]))) == NULL) {
	// 	perror("inet_ntoa");
	// 	return FAILURE;
	// }
	
	// printf("Current Host Name: %s\n", host);
   	// printf("Host IP: %s\n", server_ip);

	// bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return FAILURE;
    }

	// printf("starting server on:\n");

	
	pid = fork();

	if (pid == 0) {
		int status = 0b0;

		printf("executing SSH commands...\n");
		status = execute_sys_cmd("./AWS/activate_aws.sh", "activate AWS SSH") << 1;
		status = status | execute_sys_cmd("ssh -i ~/.keys/ec2sshkey.pem ec2-user@18.222.161.150 echo hello EC2 :)", "connect AWS ssh");

		printf("status %0xb\n", status);
		if (status == 0b11) {
			exit(SUCCESS);
		} else {
			exit(FAILURE);
		}
		
	} else if (pid == -1) {
		perror("fork");
		exit(FAILURE);
	} else {
		childpid = waitpid(-1, &status, 0);
		printf("Status 0b%d\n",status);
		if (WIFEXITED(status) == -1){
			return FAILURE;
		} else {
			printf("ssh completed\n");
		}
	}

	while (1) {
		if (listen(server_fd, 3) == -1) {
			perror("listen");
			return FAILURE;
		}


		new_socket = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen);
		if (new_socket == -1) {
			perror("accept");
			return FAILURE;
		}

		valread = read(new_socket, buffer, 1024);
		if (valread == -1) {
			perror("read");
			return FAILURE;
		}
		printf("%s\n", buffer);
		send(new_socket, ACK, strlen(ACK), 0);
		printf("Hello message sent\n");
	}

    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return SUCCESS;
}

// docker run --mount source=shared-stuff,target=/stuff