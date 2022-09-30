#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>

int main(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in server;
	char message[1000], server_reply[2000];
	
	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Could not create socket");
	}
	//puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");
	
	printf("Enter option: ");
	fgets(message, 1000, stdin);
	
	//Send some data
	if (send(sock, message, strlen(message)+1, 0) < 0) {
		puts("Send failed");
		return 1;
	} else {
		puts("Send ok");
	}
	
	//Receive a reply from the server
	/*memset(server_reply, 0, 2000);
	if (recv(sock, server_reply, 2000, 0) < 0) {
		puts("Recv failed");
		//break;
	} else {
		puts("Recv ok");
	}*/
	
	//puts("Server reply :");
	//puts(server_reply);

	
	close( sock );
	return 0;
}
