#include <iostream>
#include <ws2tcpip.h> //winsock header

#pragma comment (lib, "ws2_32.lib") //directive providing extra required details to the compiler


using namespace std; //bad boy

//~~~~~~~~~~~~~~~~~BAREBONES SERVER PROCESS~~~~~~~~~~~~~~~~~~~~~~
	//initialize winsock
	//create a socket
	//bind the socket to an ip address and port (server side)
	//tell winsock the socket is for listening
	//wait for connection
	//close listening socket
	//while loop : accept and echo message back to client
	//close the socket
	//clean up winsock
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void main()
{

	//initialize winsock

	WSADATA wsData;							//structure to call winsock startup
	WORD ver = MAKEWORD(2, 2);				//provides version 2.2

	int ws0k = WSAStartup(ver, &wsData);	//specify the WORD we want (the version), pointer to data
	if (ws0k != 0)							//error handling if winsock can't be initialized
	{
		cerr << "can't initialize winsock" << endl;
		return;
	}

	//create a socket
	
	//in windows SOCKET is a type, but really socket is just a number (aka file descipter in unix)
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); //function socket (lowercase) takes (address family, SOCK_STREAM = TCP connection (SOCK_DGRAM = UDP), 0).
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket" << endl;
		return;
	}

	//bind the socket to an ip address and port (server side)

	sockaddr_in hint;						//link IP Address and port to the socket
	hint.sin_family = AF_INET;				//IPv4
	hint.sin_port = htons(54000);			//networks are big endian, pcs are little endain, htons = "host to network short" links the two endians, 54000 is the port.
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //any address on the given port (could also use inet_pton)

	bind(listening, (sockaddr*)&hint, sizeof(hint)); //socket binding to network code, usable to reference networking code, so we can send out and recieve (aka file descripter)


	//tell winsock the socket is for listening

	listen(listening, SOMAXCONN); //indicates a readiness to accept client connection. SOMAXCONN is the backlog, the maximum number of queued connections you want on a socket.

	//wait for connection

	sockaddr_in client;
	int clientSize = sizeof(client);

	//client connected pair, remote IP address + port number and local connection to server + port number
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize); //listening for connetion, connection happens, connection happens on particular server (54000), socket that returns the value is going to be some random number
	//if (clientSocket == INVALID_SOCKET) { do something } ... for error safety

	char host[NI_MAXHOST];				//clients remote name
	char service[NI_MAXHOST];			// service (i.e) port the client is conneted on

	ZeroMemory(host, NI_MAXHOST);		//on mac's this is memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << "connected on port: " << service << endl;
	}
	else 
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);			 //IP address in 4 bytes 127.0.0.1 (local server)
		cout << host << "connected on port: " << ntohs(client.sin_port) << endl;
	}

	//close listening socket
	closesocket(listening);				//leaving this open will allow the accepting of new clients

	//while loop : accept and echo message back to client
	char buf[4096];						//this is not fully correct, "should be reading until it reads no bytes"

	while (true)
	{
		ZeroMemory(buf, 4096);

		//wait for client to send data
		int bytesRecieved = recv(clientSocket, buf, 4096, 0);
		if (bytesRecieved == SOCKET_ERROR)
		{
			cerr << "error in recv(). quitting" << endl;
			break;
		}

		if (bytesRecieved == 0)
		{
			cout << "client disconneted" << endl;
			break;
		}

		cout << string(buf, 0, bytesRecieved) << endl;

		//echo message back to client
		send(clientSocket, buf, bytesRecieved + 1, 0); //+1 because it needs to include a zero buffer
		//this could also have error handling

	}

	//close the socket
	closesocket(clientSocket);

	//clean up winsock
	WSACleanup;
}