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
#include <time.h>
#include <pthread.h>


void* socket_clients (void* arguments);
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


typedef struct
{
	int index;
	SOCKET serverSocket;

}server_args_t;

int main() {
	APP_nERROR enAppError = APP_enERROR_OK;

	WSAData lpWSADataReg;
	SOCKET serverSocket;
	SOCKADDR_IN serverAddress;
	fflush(stdout);
	setbuf(stdout, NULL);
	printf("Socket Exercise, Server\n"); // prints Socket

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
		serverAddress.sin_addr.S_un.S_addr = INADDR_ANY;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(5555);
		bindCode = bind(serverSocket, (sockaddr*) &serverAddress, sizeof(serverAddress));
		if(SOCKET_ERROR == bindCode)
		{
			printf("bind Socket, Error Code: %s\n", strerror(errno));
			enAppError = APP_enERROR_FUNCTION_RETURN;
		}
		else
		{
			printf("bind Socket, Address assigned: %s\n", inet_ntoa(serverAddress.sin_addr));
		}

	}

	if(APP_enERROR_OK == enAppError)
	{
		int listenCode;
		listenCode = listen(serverSocket, 10);
		if(SOCKET_ERROR == listenCode)
		{
			printf("listen Socket, Error Code: %s\n", strerror(errno));
			enAppError = APP_enERROR_FUNCTION_RETURN;
		}
		else
		{
			printf("Starting listening for incomming Connections...!!\n");
		}
	}

#define NUM_THREADS (100)
	__pthread_t* threads[NUM_THREADS];
	server_args_t server_args_t[NUM_THREADS];
    int i;

    for (i = 0; i < NUM_THREADS; i++)
    {
    	server_args_t[i].serverSocket = serverSocket;
    	server_args_t[i].index = i;
        pthread_create(&threads[i], (__pthread_attr_t * const *) NULL, &socket_clients, &server_args_t[i]);
    }


    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

}











void* socket_clients (void* arguments)
{

	APP_nERROR enAppError = APP_enERROR_OK;
	server_args_t* server_args;
	SOCKADDR_IN clientAddress;
	int index;
	SOCKET serverSocket;
#define MAX_CHAR (1024)
#define RESPONSE_MAX_CHAR (MAX_CHAR * 10)

	char buffer[MAX_CHAR];
	char response[RESPONSE_MAX_CHAR];
	char body[RESPONSE_MAX_CHAR];
	int headerLength;
	int bodyLength;
	int responseLength;

	char* responsePointer;
	memset(buffer,0,sizeof(buffer));
	memset(response,0,sizeof(response));
	server_args = (server_args_t*) arguments;
	serverSocket = server_args->serverSocket;
	index = server_args->index;
	printf("Starting task %u!!\n", index);
	while(APP_enERROR_OK == enAppError)
	{

		APP_nERROR enSocketError = APP_enERROR_OK;
		SOCKET clientSocket;
		int clientAddressSize;

		clientAddressSize = sizeof(clientAddress);
		clientSocket = accept(serverSocket, (sockaddr*) &clientAddress,&clientAddressSize);
		if((SOCKET) SOCKET_ERROR != clientSocket)
		{
			printf("Client Connected, FD: %llu!!\n", clientSocket);
			printf("Client Address: %s\n", inet_ntoa(clientAddress.sin_addr));
			printf("Client Port: %d\n", ntohs(clientAddress.sin_port));

			int receiveCode;
			//do
			{
				receiveCode = recv(clientSocket, buffer, sizeof(buffer), 0);
				if(SOCKET_ERROR == receiveCode)
				{
					printf("Socket without data, Error Code: %s\n", strerror(errno));
					enSocketError = APP_enERROR_FUNCTION_RETURN;
				}
				else
				{
					if(0 != receiveCode)
					{
						printf("Socket string information [%d]\n\n%s\n\n", receiveCode, buffer);
						char* bufferPointer = buffer;
						/*GET command*/
						int getComparison = strncmp((const char*) bufferPointer, "GET ", strlen("GET "));
						bufferPointer += strlen("GET "); /*GET(space)*/
						if(0 == getComparison)
						{
							printf("GET request detected, REQUEST: %s\n", bufferPointer);
							int requestComparison = strncmp((const char*) bufferPointer, "/favicon.ico", strlen("/favicon.ico"));
							if(0 == requestComparison)
							{
								printf("Opening favicon...\n\n");
								FILE *favicon = fopen("../images/favicon.ico", "rb");
								if(0 != favicon)
								{
									printf("open favicon...\n\n");
									fseek(favicon, 0, SEEK_END);
									long faviconSize = ftell(favicon);
									rewind(favicon);
									char *faviconBuffer = (char *)calloc(faviconSize, sizeof(char));
									if (NULL != faviconBuffer)
									{
										size_t faviconRead = fread(faviconBuffer, 1, faviconSize, favicon);
										printf("Responding Request...\n\n");
										printf("Body length: [%d]\n", faviconSize);


										memset(response,0,sizeof(response));
										headerLength = snprintf(response, RESPONSE_MAX_CHAR, "HTTP/1.1 200 OK\nContent-Type: image/x-icon\nContent-length:%u\nCache-Control: public, max-age=604800\n\n", faviconRead);
										printf("Header length: [%d]\n", headerLength);

										responsePointer = response;
										responsePointer += headerLength;
										memcpy(responsePointer,faviconBuffer,faviconRead);
										responseLength =  headerLength + faviconRead;

										printf("length: [%d]\n%s\n", responseLength, response);
										int sendCode;
										sendCode = send(clientSocket, response, responseLength, 0);
									}
								}
								else
								{
									printf("INVALID favicon...\n\n");
								}
							}
							requestComparison = strncmp((const char*) bufferPointer, "/value=", strlen("/value="));

							if(0 == requestComparison)
							{
								bufferPointer += strlen("/value="); /*GET(space)*/
								printf("Sending generic response...\n\n");
								int value = 0;
								char* tailPointer;
								value = strtol(bufferPointer, &tailPointer, 10);
								memset(body,0,sizeof(body));
								snprintf(body, RESPONSE_MAX_CHAR, "<html><body><H1>Hi from <b>Mexico</b> or not?, client number %u, value received %u</H1></body></html>", index, value);
								bodyLength =  strnlen(body,RESPONSE_MAX_CHAR);
								printf("Body length: [%d]\n", bodyLength);


								memset(response,0,sizeof(response));
								headerLength = snprintf(response, RESPONSE_MAX_CHAR, "HTTP/1.1 200 OK\nContent-length:%d\nContent-Type: text/html\n\n", bodyLength);
								printf("Header length: [%d]\n", headerLength);

								responsePointer = response;
								responsePointer += headerLength;
								strncpy(responsePointer,body,RESPONSE_MAX_CHAR);
								responseLength =  strnlen(response,RESPONSE_MAX_CHAR);

								printf("length: [%d]\n%s\n", responseLength, response);
								int sendCode;
								sendCode = send(clientSocket, response, responseLength, 0);
							}


						}
						else
						{
							printf("Invalid request detected\n");

							memset(body,0,sizeof(body));
							snprintf(body, RESPONSE_MAX_CHAR, "<html><body><H1>Hi from <b>Mexico</b> or not?, client number %u</H1></body></html>", index);
							bodyLength =  strnlen(body,RESPONSE_MAX_CHAR);
							printf("Body length: [%d]\n", bodyLength);


							memset(response,0,sizeof(response));
							headerLength = snprintf(response, RESPONSE_MAX_CHAR, "HTTP/1.1 200 OK\nContent-length:%d\nContent-Type: text/html\n\n", bodyLength);
							printf("Header length: [%d]\n", headerLength);


							responsePointer = response;
							responsePointer += headerLength;
							strncpy(responsePointer,body,RESPONSE_MAX_CHAR);
							responseLength =  strnlen(response,RESPONSE_MAX_CHAR);

							printf("length: [%d]\n%s\n", responseLength, response);
							int sendCode;
							sendCode = send(clientSocket, response, responseLength, 0);

						}
						memset(buffer,0,sizeof(buffer));

					}
					else
					{
						printf("Socket with data 0, Error Code: %s\n", strerror(errno));
						enSocketError = APP_enERROR_FUNCTION_VALUE;
					}
				}
			}//while((SOCKET_ERROR != receiveCode) && (APP_enERROR_OK == enSocketError));
			int socketCode;
			socketCode = closesocket(clientSocket);
			if(SOCKET_ERROR == socketCode)
			{
				printf("Close Client Socket Main Error: %d, Error Code: %s\n",socketCode, strerror(errno));
				enAppError = APP_enERROR_FUNCTION_RETURN;
			}
			else
			{
				printf("Close Client Socket successfully!\n");
			}

		}
		else
		{
			printf("Socket client connection Error: %s\n", strerror(errno));
			enAppError = APP_enERROR_FUNCTION_RETURN;
		}
	}

	closesocket_(serverSocket);


	printf("ReturnCode value: %d\n", enAppError);
	return ((void*) enAppError);
}


int closesocket_(SOCKET s)
{
	int socketCode;
	int WSACode;
	socketCode = closesocket(s);
	if(SOCKET_ERROR == socketCode)
	{
		printf("Close Socket Main Error: %d, Error Code: %s\n",socketCode, strerror(errno));
	}
	else
	{
		printf("Close Socket successfully!\n");
	}

	WSACode = WSACleanup();
	if(0 != WSACode)
	{
		int WSAErrorCode;
		WSAErrorCode = WSAGetLastError();
		printf("WSACleanup Main Error: %d, Error Code: %d\n", WSACode, WSAErrorCode);

	}
	else
	{
		printf("WSAStartup cleaned!\n");
	}

	return socketCode;
}
