#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include<stdbool.h>
void *makeContainer( void *message );
void *listContainers( );
void *stopContainer( void *message );
void *deleteContainer( void *message );

int main() {
	int socket_desc, client_sock, c, read_size;
	struct sockaddr_in server, client;  // https://github.com/torvalds/linux/blob/master/tools/include/uapi/linux/in.h
	char client_message[2000];
	
	// Create socket
    // AF_INET (IPv4 protocol) , AF_INET6 (IPv6 protocol) 
    // SOCK_STREAM: TCP(reliable, connection oriented)
    // SOCK_DGRAM: UDP(unreliable, connectionless)
    // Protocol value for Internet Protocol(IP), which is 0
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		printf("Could not create socket");
	}
	//puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);
	
	//Bind the socket to the address and port number specified
	if( bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	//puts("bind done");
	
	// Listen
    // It puts the server socket in a passive mode, where it waits for the client 
    // to approach the server to make a connection. The backlog, defines the maximum 
    // length to which the queue of pending connections for sockfd may grow. If a connection 
    // request arrives when the queue is full, the client may receive an error with an 
    // indication of ECONNREFUSED.
	// https://man7.org/linux/man-pages/man2/listen.2.html
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	while(1) {
		//accept connection from an incoming client
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if(client_sock < 0) {
			perror("accept failed");
			return 1;
		}
		puts("Connection accepted");
        memset(client_message, 0, 2000);
        recv(client_sock, client_message, 2000, 0);
        char *token = strtok( client_message, " \n" );
        char commands[ 3 ][ 200 ];
        int i = 0;
        while( token != NULL )
        {
           strcpy( commands[ i ], token );
           i = i + 1;
           token = strtok( NULL, " \n" );
        }
        //for( int i = 0; i < strlen(  )) 
		/*int pid = fork();

		if (pid == 0) {
			memset(client_message, 0, 2000);
			//Receive a message from client
			if(recv(client_sock, client_message, 2000, 0) > 0) {
				printf("received message: %s\n", client_message);
				//Send the message back to client
				send(client_sock, client_message, strlen(client_message), 0);
			}
		}*/
	pthread_t tid;
        if( strcmp( commands[ 0 ], "make" ) == 0 )
        {
           pthread_create( &tid, NULL, makeContainer, ( void* ) commands[ 1 ] );
           pthread_join( tid, NULL );
        }
        else if( strcmp( commands[ 0 ], "list" ) == 0 )
        {
           pthread_create( &tid, NULL, listContainers, NULL );
           pthread_join( tid, NULL );
        }
        else if( strcmp( commands[ 0 ], "stop" ) == 0 )
        {
           pthread_create( &tid, NULL, stopContainer, ( void* ) commands[ 1 ] );
           pthread_join( tid, NULL );
        }
        else if( strcmp( commands[ 0 ], "delete" ) == 0 )
        {
           pthread_create( &tid, NULL, deleteContainer, ( void* ) commands[ 1 ] );
           pthread_join( tid, NULL );
        }
    }
   
	exit( 0 );
}

void *makeContainer( void *message )
{
   char *name = (char *) message;
   bool flag = true;
   pid_t pid = fork();
   if( pid == 0 )
   {
      if( execlp("docker", "docker", "run", "-di", "--name", name, "ubuntu:latest", "/bin/bash", NULL) == -1 )
      {
          flag = false;
      }
   }
   if( flag == true )
   {
       printf( "Contenedor creado con exito.\n" );
   }
   pthread_exit( 0 );
}

void *listContainers( )
{
   pid_t pid = fork();
   if( pid == 0 )
   {
      execlp("docker", "docker", "ps", NULL);
   }
   pthread_exit( 0 );
}

void *stopContainer( void *message )
{
   char *name = (char *) message;
   pid_t pid = fork();
   if( pid == 0 )
   {
      execlp("docker", "docker", "stop", name, NULL);
   }
   else{
      printf( "Contenedor detenido \n" );
   }
   pthread_exit( 0 );
}

void *deleteContainer( void *message )
{
   char *name = (char *) message;
   pid_t pid = fork();
   if( pid == 0 )
   {
      execlp("docker", "docker", "rm", name, NULL);
   }
   else{
      printf( "Contenedor eliminado \n" );
   }
   pthread_exit( 0 );
}
