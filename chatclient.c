/*
CS372 Introduction to Networking
Chat Client Created by Matthew Albrecht
May 5, 2019
This program is a chat client.  Once a server is established the client 
can chat with the server.  The client makes first contact and continues
until either the server or the client type "\quit".
This client was designed with assistance from the book, Beej's Guide to
Network Programming.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAXDATASIZE 500 // max number of bytes we can get at once
/************************************************
get sockaddr function: This function was created
by Beej in the book Guide to Network Programming.
************************************************/
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
/************************************************
initiateContact function: this function determines and creates
the socket.  The functions and design are from Beej's Guide to
Network Programming Stream Server Example.  The function takes
the pointer to the addrinfo struct and the int sockfd.  It returns
sockfd for use in the main function.
************************************************/
int initiateContact(struct addrinfo *servinfo, struct addrinfo *p, int sockfd)
{
	//connect to the first socket available
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		//set sockfd and check for errors
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}
		//initiate the connection and check for errors
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			continue;
		}
		break;
	}
	//if the client fails to connect exit the program
	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	return sockfd;
}
/************************************************
sendMessage function: this function sends messages to
the server.  it takes the user's handle, and the quit string, and the int sockfd 
to send messages.  If the user transmits \quit, the message will not be sent and
the program will close. The function will return 0.  If a message is sent the
function will return 1, and continue in the while loop in the main function.
Message sending function was influenced by Beej and his Guide to Network Programming
************************************************/
int sendMessage(char handle[10], char quit[6], int sockfd)
{
	printf("%s> ", handle);
	char input[500];
	char *ptr = input;//pointer to the input string

	fgets(input, 500, stdin);
	//remove the newline character
	if (input[strlen(input) - 1] == '\n')
		input[strlen(input) - 1] = '\0';
	//check if the user has quit
	if (strcmp(input, quit) == 0)
	{
		printf("%s\n", quit);
		return 0;
	}

	int len = strlen(ptr);
	if (send(sockfd, ptr, len, 0) == -1)
		perror("send");
	return 1;
}
/************************************************
receiveMessage function: this function reveives messages from
the server.  it takes the int sockfd and the buffer string to receive
messages.  Once the server transmits the quit message, the function
will return 0, signaling an end to the program.  If normal messsages
are received the program will return 1.  General flow of message receiveing
was influenced by Beej in the Guide to Network Programming
************************************************/
int receiveMessage(int sockfd, char buf[MAXDATASIZE])
{
	int numbytes;
	//receive the message and check for errors.  Error checking courtesy of Beej.
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}
	//all a null terminator
	buf[numbytes] = '\0';
	
	//check if the server has quit
	if (strcmp(buf, "Server has Quit") == 0)
	{
		printf("Server has Quit\n");
		return 0;
	}
	//print the message from the server
	printf("Server> %s\n", buf);
	return 1;
}
/************************************************
Main function: initiates the starting variables.  This function also
calls the intiateContact function, and has a repeating while loop
to send and receive messages.  Once either the client or the server
quits the while loop will break and end the function.
************************************************/
int main(int argc, char *argv[])
{
	char quit[6] = "\\quit";
	int sockfd;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	//ask for the User's handle (up to 10 characters)
	char handle[10];
	printf("Please enter your handle: ");
	scanf("%s", handle);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	//function called to set up the socket
	sockfd = initiateContact(servinfo, p, sockfd);
	//if failed to connect
	if (sockfd == 2)
	{
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	freeaddrinfo(servinfo); // all done with this structure
	char *ptr = handle;
	int len = strlen(ptr);
	if (send(sockfd, ptr, len, 0) == -1)
		perror("send");
	getchar();//clear the buffer
	int cont = 1;

	//while loop continues until either the client or the server types "\quit"
	while (cont) 
	{
		//called to recieve the messages
		cont = receiveMessage(sockfd, buf);

		if(cont != 0)
			//called to send messages, only if the server hasnt quit
			cont = sendMessage(handle, quit, sockfd);
	
	}
	//close the socket, and send a empty string to the server, signifying the closuer of the socket
	close(sockfd);
	return 0;
}