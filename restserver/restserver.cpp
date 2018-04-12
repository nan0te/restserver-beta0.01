// restserver.cpp: define el punto de entrada de la aplicación de consola.
//

#pragma once 
#undef UNICODE

#include "stdafx.h"
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
#define DEFAULT_PORT "80"


int createServer();
void *threadMain(void* arg);
struct threadArgs { int cltSock; };
void HandleClient(int clientSocket);

int main()
{
   
	SOCKET ClntSock = INVALID_SOCKET;
	

	int servSock;
	
	pthread_t threadID;
	struct threadArgs *threadargs;


	


	
	servSock = createServer();

	for (; ; ) {

		ClntSock = accept(servSock, NULL, NULL);
		if (ClntSock == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(servSock);
			WSACleanup();
			return 1;
		}
		if ((threadargs = (struct threadArgs*) new threadArgs()) == NULL) printf("error \n");

		threadargs->cltSock = (int)ClntSock;

		if (pthread_create(&threadID, NULL, threadMain, (void*)threadargs) != 0) printf("erroe \n");
	}

	return 0;

}

int createServer() {

	WSADATA wsaData;
	int iResult;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	SOCKET servSock = INVALID_SOCKET;


	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	memset(&hints, NULL, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}


	servSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (servSock == INVALID_SOCKET) {

		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}


	iResult = bind(servSock, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(servSock);
		WSACleanup();
		return 1;
	}
		iResult = listen(servSock, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(servSock);
			WSACleanup();
			return 1;
		}

	}

	void *threadMain(void *arg) {
	
		int clntSocket;
		
		pthread_detach(pthread_self());

		clntSocket = ((struct threadArgs*) arg) -> cltSock ;

		delete(arg);

		HandleClient(clntSocket);

		return (NULL);
	}

	void HandleClient(int clientSocket) {


		char recvbuf[DEFAULT_BUFLEN];
		int recvbuflen = DEFAULT_BUFLEN;
	
		if ((recvbuflen = recv(clientSocket, recvbuf, recvbuflen, 0)) < 0) printf("error receive data\n");

		while (recvbuflen > 0) {



		int	iSendResult = send(clientSocket, recvbuf, recvbuflen, 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			
		}

		if ((recvbuflen = recv(clientSocket, recvbuf, recvbuflen, 0)) < 0) printf("error receive data\n");
		closesocket(clientSocket);
		
		}

	}

