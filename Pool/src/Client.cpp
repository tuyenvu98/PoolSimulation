#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_COUNT       20

#define DEFAULT_PORT        "8080"

#define DEFAULT_BUFFER      2048

#define DEFAULT_MESSAGE     "\'A test message from client\'"

using namespace std;


BOOL  bSendOnly = FALSE;                        // Send data only; don't receive


// Function: main

// Description:

//    Main thread of execution. Initialize Winsock, parse the

//    command line arguments, create a socket, connect to the

//    server, and then send and receive data.

int main(int argc, char** argv)
{
    string f;
    getline(cin, f);
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    int iResult;
    int ret, i;
    char szBuffer[DEFAULT_BUFFER];
    // Validate the parameters

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        else
        {
            int iResult = send(ConnectSocket, f.c_str(), f.length(), 0);
            if (iResult == SOCKET_ERROR) {
                std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
                WSACleanup();
            }
            else
                cout << "\033[1;31mSENT:\033[0m" << f << endl;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Unable to connect to server! " << std::endl;
        WSACleanup();
        return 1;
    }

    // Send and receive data

    cout << "Sending and receiving data if any..." << endl;

    while (1)
    {
        string str;
        getline(cin, str);
        iResult = send(ConnectSocket, str.c_str(), str.size(), 0);

        if (iResult == 0)
            break;
        else if (iResult == SOCKET_ERROR)
        {
            cout << "send() failed with error code " << WSAGetLastError() << endl;
            break;
        }
        cout << "send() should be fine. Send bytes : " << iResult << endl;

        if (!bSendOnly)
        {
            iResult = recv(ConnectSocket, szBuffer, DEFAULT_BUFFER, 0);
            if (iResult == 0)        // Graceful close
            {
                cout << "It is a graceful close!"<< endl;
                break;
            }
            else if (iResult == SOCKET_ERROR)
            {
                cout << "recv() failed with error code "<< WSAGetLastError() << endl;
                break;
            }
            szBuffer[iResult] = '\0';
            cout << "recv() is OK. Received " << iResult << " bytes: " << szBuffer << endl;
        }
    }

    if (closesocket(ConnectSocket) == 0)
        cout << "closesocket() is OK!" << endl;
    else
        cout << "closesocket() failed with error code " << WSAGetLastError() << endl;

    if (WSACleanup() == 0)
        cout << "WSACleanup() is fine" << endl;
    else
        cout << "WSACleanup() failed with error code " << WSAGetLastError() << endl;
    return 0;

}