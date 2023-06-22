//============================================================================
// Name        : Server-Client.cpp
// Author      : Eduardo Alvarez
// Version     :
// Copyright   : InDeviceMex
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>


int closesocket_(SOCKET s);

typedef enum
{
	APP_enERROR_OK,
	APP_enERROR_ARGUMENT_POINTER,
	APP_enERROR_ARGUMENT_VALUE,
	APP_enERROR_INTERNAL_POINTER,
	APP_enERROR_FUNCTION_RETURN,
	APP_enERROR_FUNCTION_VALUE,
}APP_nERROR;

int main() {
	APP_nERROR enAppError = APP_enERROR_OK;

	WSAData lpWSADataReg;
	SOCKET serverSocket;
	SOCKADDR_IN serverAddress;

	//printf("Socket Exercise, Client\n"); // prints Socket

	if(APP_enERROR_OK == enAppError)
	{
		int WSACode;
		WSACode = WSAStartup(MAKEWORD(2,0), &lpWSADataReg);
		if(0 != WSACode)
		{
			int WSAErrorCode;
			WSAErrorCode = WSAGetLastError();
			printf("WSAStartup Main Error: %d, Error Code: %d\n", WSACode, WSAErrorCode);
			enAppError = APP_enERROR_FUNCTION_RETURN;
		}
		else
		{
			printf("WSAStartup ready!\n");
		}
	}

	if(APP_enERROR_OK == enAppError)
	{
	    if ((2 != LOBYTE(lpWSADataReg.wHighVersion)) || (2 != HIBYTE(lpWSADataReg.wHighVersion)))
		{
			printf("WSAStartup was init with a incorrect version\n");
			enAppError = APP_enERROR_FUNCTION_VALUE;
		}
	    else
	    {
			printf("WSAStartup version: %08x\n", lpWSADataReg.wVersion);
			printf("WSAStartup High version: %08x\n", lpWSADataReg.wHighVersion);
			printf("WSAStartup Max num of sockets: %d\n", lpWSADataReg.iMaxSockets);
			printf("WSAStartup Max UDP dg: %d\n", lpWSADataReg.iMaxUdpDg);
			printf("WSAStartup System Status: %s\n", lpWSADataReg.szSystemStatus);
			printf("WSAStartup Descriptor: %s\n", lpWSADataReg.szDescription);
	    }
	}

	if(APP_enERROR_OK == enAppError)
	{
		serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if((SOCKET) SOCKET_ERROR == serverSocket)
		{
			printf("Open Socket, Error Code: %s\n", strerror(errno));
			enAppError = APP_enERROR_FUNCTION_RETURN;
		}
		else
		{
			printf ("Open Socket FileDescriptor: %llu\n", serverSocket);
		}

	}

	if(APP_enERROR_OK == enAppError)
	{
		int bindCode;
		serverAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(5555);
		bindCode = connect(serverSocket, (sockaddr*) &serverAddress, sizeof(serverAddress));
		if(SOCKET_ERROR == bindCode)
		{
			printf("connect Socket, Error Code: %s\n", strerror(errno));
			enAppError = APP_enERROR_FUNCTION_RETURN;
		}
		else
		{
			printf("connect Socket, Address assigned: %s\n", inet_ntoa(serverAddress.sin_addr));
		}

	}

#define MAX_CHAR (1024)
	char buffer[MAX_CHAR];
	int sendCode;

	memset(buffer,0,sizeof(buffer));
	strncpy(buffer, "Hola mundo, soy un cliente.", MAX_CHAR);

	sendCode = send(serverSocket, buffer, sizeof(buffer), 0);
	if(SOCKET_ERROR == sendCode)
	{
		printf("Socket sending error, Error Code: %s\n", strerror(errno));
		enAppError = APP_enERROR_FUNCTION_RETURN;
	}
	else
	{
		printf("Socket string sent, data sent: %d\n", sendCode);
	}

	closesocket_(serverSocket);


	printf("ReturnCode value: %d\n", enAppError);
	return (enAppError);
}



int closesocket_(SOCKET s)
{
	int socketCode;
	int WSACode;
	socketCode = closesocket(s);
	if(SOCKET_ERROR == socketCode)
	{
		//printf("Close Socket Main Error: %d, Error Code: %s\n",socketCode, strerror(errno));
	}
	else
	{
		//printf("Close Socket successfully!\n");
	}

	WSACode = WSACleanup();
	if(0 != WSACode)
	{
		int WSAErrorCode;
		WSAErrorCode = WSAGetLastError();
		//printf("WSACleanup Main Error: %d, Error Code: %d\n", WSACode, WSAErrorCode);

	}
	else
	{
		//printf("WSAStartup cleaned!\n");
	}

	return socketCode;
}
