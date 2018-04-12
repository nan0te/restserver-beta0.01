#pragma once 
#undef UNICODE

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>


#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512




int createServer();
void *threadMain(void* arg);
struct threadArgs { int cltSock; };
void HandleClient(int clientSocket);

int main()
{

	SOCKADDR_IN clntinfo;
	int clntinfo_len = 0;

	int servSock;
	SOCKET clntSock ;

	pthread_t threadID;
	struct threadArgs *threadargs;

	servSock = createServer();	
	
	for (; ; ) {

		clntSock = accept(servSock, (struct sockaddr*) &clntinfo, &clntinfo_len);
		if (clntSock == INVALID_SOCKET) {
			// printf("dont conecction:  %d \n ", WSAGetLastError());
			Sleep(3000);
		}
		else if (clntSock != INVALID_SOCKET) printf("Client accepted .. \n");
		
		char *client_ip = inet_ntoa(clntinfo.sin_addr);
		int client_port = ntohs(clntinfo.sin_port);
		char *message = "Hello client";

		send(clntSock, message, sizeof(message), 0);
		
		closesocket(servSock);
		WSACleanup();


		/*else {

					printf("Client: Receiver IP(s) used: %s\n", inet_ntoa(clntinfo.sin_addr));


					if ((threadargs = (struct threadArgs*) new threadArgs()) != NULL)
					 printf("server aceppet conecction thread...\n");
					 threadargs->cltSock = sock;

					if (pthread_create(&threadID, NULL, threadMain, (void*)threadargs) == 0)

					 printf("creating one thread... \n \n"); 				}
				}*/
	}
	system("pause");
	return 0;

}

int createServer() {

	WSADATA wsaData;
	int iResult;
	
	 struct sockaddr_in svrinfo;
	unsigned short int PORT = 8084;
	SOCKET servSock = INVALID_SOCKET;
	static const char SERVER_IP[] = "127.0.0.1";

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		
	}

	
	memset( &svrinfo, 0, sizeof(svrinfo));
	svrinfo.sin_family = AF_INET;		
	      
	svrinfo.sin_port = htons(PORT);
	svrinfo.sin_addr.s_addr = inet_addr(SERVER_IP); ///< IP Address of Server

	servSock = socket(AF_INET, SOCK_STREAM, 0);




	servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (servSock == INVALID_SOCKET) {

		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();	
	}



	iResult = bind(servSock, (struct sockaddr *) &svrinfo, sizeof(svrinfo));
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(servSock);
		WSACleanup();
		
	}
	printf("Server Address is : %s\n", inet_ntoa(svrinfo.sin_addr));
	//std::cout << "Server address is  : " << ipserver << std::endl;
	printf("PORT  : %d \n\n " , PORT ) ;
	iResult = listen(servSock, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(servSock);
		WSACleanup();
		}
	else if (iResult != SOCKET_ERROR) printf("server waiting conecctions...\n");
	
	return 0;
}

void *threadMain(void *arg) {

	int clntSocket;

	pthread_detach(pthread_self());

	clntSocket = ((struct threadArgs*) arg)->cltSock;

	delete(arg);

	HandleClient(clntSocket);

	return (NULL);
}

void HandleClient(int clientSocket) {


	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int re = NULL;

	if ((re = recv(clientSocket, recvbuf, recvbuflen, 0)) < 0) printf("error receive data\n");

	while (re > 0) {

		printf("received: '%s'\n", recvbuf);

		int	iSendResult = send(clientSocket, recvbuf, recvbuflen, 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();

		}

		if ((re = recv(clientSocket, recvbuf, recvbuflen, 0)) < 0) printf("error receive data\n");
		closesocket(clientSocket);

	}

}

