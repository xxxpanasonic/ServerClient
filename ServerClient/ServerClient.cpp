#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;
int main()
{   
    WSADATA wsaData;
	ADDRINFO hints;
	ADDRINFO* addrResult = NULL;
	SOCKET ConnectSocket = INVALID_SOCKET;
	

	const char* sendBuffer = "Hello from client!";

	char recvBuffer[512];

	int result;


	
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		cout << "WSAStartup failed, result = " << result << endl;
		return 1;
	}


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	result = getaddrinfo("localhost", "1945", &hints, &addrResult);


	if (result != 0)
	{
		cout << "getaddrinfo faild with error: " << result << endl; 
		WSACleanup();
		return 1;
	}
	ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Socket creation failed" << endl;
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		cout << "Unable connect to server :(" << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
	if (result == SOCKET_ERROR)
	{
		cout << "send failed, error: " << result << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	cout << "Sent: " << result << "bytes" << endl;
	result = shutdown(ConnectSocket, SD_SEND); //Получение on, передачи off
	if (result == SOCKET_ERROR)
	{
		cout << "Shutdown error: " << result << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	ZeroMemory(recvBuffer, 512);
	do
	{
		result = recv(ConnectSocket, recvBuffer, 512, 0); // Отрицательное - ошибка, 0 - сокет закрыт с той стороны, положительное - ОК
		if (result > 0)
		{
			cout << "Received" << result << " bytes" << endl;
			cout << "Received data: " << recvBuffer << endl;
		}
		else if (result == 0)
			cout << "Connection closed" << endl;
		else
			cout << "recv failed with error" << endl;
	} while (result > 0);
	closesocket(ConnectSocket);
	freeaddrinfo(addrResult);
	WSACleanup();
	return 1;
}
