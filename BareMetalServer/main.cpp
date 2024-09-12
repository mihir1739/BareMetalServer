#include <iostream>
// Required for socket functionalities of TCP in Windows.
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main() {
	// Init Winsock
	WSADATA wsdata;
	WORD ver = MAKEWORD(2, 2); // version tag

	int wsok = WSAStartup(ver, &wsdata); // returns 0 for success

	if(wsok != 0)
	{
		cerr << "Socket Initialization failed..... Quiting";
		return 1;
	}
	cout << "Status: " << wsdata.szSystemStatus << endl;

	// New Socket creation
	SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener == INVALID_SOCKET)
	{
		cerr << "Socket creation failed.....Quitting " << WSAGetLastError() << endl;
		return 1;
	}

	// Bind the socket to an IP Address : Server
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000); // host to network little endian => big endian
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton

	bind(listener, (sockaddr*)&hint, sizeof(hint)); // File descriptors

	// Configure the socket for listening
	listen(listener, SOMAXCONN);

	// Waiting for connection
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientsocket = accept(listener, (sockaddr*)&client, &clientsize);

	char host[NI_MAXHOST], service[NI_MAXHOST];
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST); // Windows SPecific use memset for Unix based systems.

	if(getnameinfo((sockaddr*)&client, sizeof(client),host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << "Connected on Port :" << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "Connected on Port" << ntohs(client.sin_port) << endl;
	}
	// Disable listening
	closesocket(listener);

	// Accept and echo message
	char buf[4096];
	while(true)
	{
		ZeroMemory(buf, 4096);

		// Waiting for client to connect
		int bytesreceived = recv(clientsocket, buf, 4096, 0);
		if(bytesreceived == SOCKET_ERROR)
		{
			cerr << "Error in receiving....Quitting " << WSAGetLastError() << endl;
		}
		if(bytesreceived == 0)
		{
			cout << "Connection Terminated" << endl;
			break;
		}
		// Echo message back
		send(clientsocket, buf, bytesreceived + 1, 0);
	}

	// Close socket
	closesocket(clientsocket);

	// Cleanup Winsock
	WSACleanup();
	return 0;
}