#include "stdafx.h"
#include "CPUUtil.h"
#include "ProcessUtil.h"
#include "stdio.h"
#include "windows.h"
#include "Psapi.h"
#include "Shlwapi.h"
#include <string>
#include <iostream>
#include <atlconv.h>
#include <atlbase.h> 
#include <atlstr.h>
#include "RamUtil.h"
#include "VirtualMemoryUtil.h"
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

CPUUtil cpuUtil = CPUUtil();
ProcessUtil procUtil = ProcessUtil();
RamUtil ramUtil = RamUtil();
VirtualMemoryUtil vmUtil = VirtualMemoryUtil();



int main() {
	

	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "waiting for client to connect";

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "client connected";

	// No longer need server socket
	closesocket(ListenSocket);

	
	while (true) {
		std::vector<HANDLE> processHandles = procUtil.getAllProcesses();
		printf("%d number of processes\n", processHandles.size());
		std::vector<double> CPUpercentage = cpuUtil.getCPUUsageForProcesses(processHandles);

		//send how many processes there are + the total CPU usage:
		std::string processCount = std::to_string(CPUpercentage.size() + 1) + '\n';
		
		iSendResult = send(ClientSocket, processCount.c_str(), processCount.size(), 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}


		std::cout << "PROCESS NAME ---------------------------- CPU USAGE\n" << std::endl;
		double total = 0.0f;
		for (int i = 0; i < processHandles.size(); i++) {
		
			std::string buff;
			
			TCHAR Buffer[MAX_PATH];
			if (GetModuleFileNameEx(processHandles.at(i), 0, Buffer, MAX_PATH)) {
				buff = CT2A(PathFindFileName(Buffer));
				buff += ",";
				buff += std::to_string(CPUpercentage.at(i));
				buff += ",";
				buff += std::to_string(ramUtil.getRAMUsedByProcess(processHandles.at(i)));
				buff += ",";
				buff += std::to_string(vmUtil.getVMUsedByProcess(processHandles.at(i))) + '\n';
				std::cout << buff << std::endl;
			}

			//send data to the client
			iSendResult = send(ClientSocket, buff.c_str(), buff.size(), 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}

			total += CPUpercentage.at(i);
		}

		std::cout << "Total CPU usage is: " << total;
		
		//send total cpu usage
		std::string totalCpuUsage = std::to_string(total) + '\n';

		//send how many processes there are:
		iSendResult = send(ClientSocket, totalCpuUsage.c_str(), totalCpuUsage.size(), 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	}
}