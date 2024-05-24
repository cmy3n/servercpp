#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
	setlocale(LC_ALL, "RUS");

	//Создаем переменные
	WSADATA wsadata;
	char recvBuffer[512];
	ADDRINFO hints;
	ADDRINFO* addrresult;
	SOCKET ConnectSocket = INVALID_SOCKET;
	SOCKET ServerSocket = INVALID_SOCKET;
	const char* sendBuffer = "Hello from Server";

	// Подключение библиотеки Winsock
	int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (result != 0)
	{
		cout << "WSAStartup failed\n";
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//Получение информации о сетевом адресе клиента
	result = getaddrinfo(NULL, "1161", &hints, &addrresult);
	if (result != 0)
	{
		cout << "Getaddrinfo error\n";
		WSACleanup();
		return 1;
	}

	//Создание сокета-сервера
	ServerSocket = socket(addrresult->ai_family, addrresult->ai_socktype, addrresult->ai_protocol);
	if (ServerSocket == INVALID_SOCKET)
	{
		cout << "Socket Error\n";
		freeaddrinfo(addrresult);
		WSACleanup();
		return 1;
	}

	// Привязка сокета к адрессу и порту
	result = bind(ServerSocket, addrresult->ai_addr, (int)addrresult->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		cout << "Binding failed\n";
		closesocket(ServerSocket);
		ServerSocket = INVALID_SOCKET;
		freeaddrinfo(addrresult);
		WSACleanup();
		return 1;
	}

	// Перевод сокета в режим прослушивания
	result = listen(ServerSocket, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		cout << "Listening socket error\n";
		closesocket(ServerSocket);
		freeaddrinfo(addrresult);
		WSACleanup();
		return 1;
	}

	// Принятие входящего соединения
	ConnectSocket = accept(ServerSocket, NULL, NULL);
	if (result == SOCKET_ERROR)
	{
		cout << "Accepting failed" << result << endl;
		WSACleanup();
		return 1;
	}
	closesocket(ServerSocket);


	// Отправка сообщения сокету клиенту
	result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
	if (result == SOCKET_ERROR)
	{
		cout << "Send error" << result << endl;
		freeaddrinfo(addrresult);
		WSACleanup();
		return 1;
	}

	cout << "Send " << result << endl;

	// Завершение передачи данных
	result = shutdown(ConnectSocket, SD_SEND);
	if (result == SOCKET_ERROR)
	{
		cout << "Shutdown error " << result << endl;
		freeaddrinfo(addrresult);
		WSACleanup();
		return 1;
	}
	

	// Цикл для получения сообщений от клиента
	do
	{
		ZeroMemory(recvBuffer, sizeof(recvBuffer), 0);
		result = recv(ConnectSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (result > 0)
		{
			cout << "Recieved: " << result << " bytes\n";
			cout << "Recieved data: " << recvBuffer << endl;
		}
		else if (result == 0)
		{
			cout << "Connection closed\n";
		}
		else
		{
			cout << "Received error\n";
		}
		cout << endl;
	} while (result > 0);
	
	freeaddrinfo(addrresult);
	WSACleanup();
}