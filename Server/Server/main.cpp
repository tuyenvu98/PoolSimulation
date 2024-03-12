#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define PORT "8080"

#define DATA_BUFSIZE 512

using namespace std;

typedef struct _SOCKET_INFORMATION {

    SOCKET Socket;

    vector<vector<int>> vec;

} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;



// Prototypes

BOOL CreateSocketInformation(SOCKET s);

void FreeSocketInformation(DWORD Index);



// Global var

DWORD TotalSockets = 0;

LPSOCKET_INFORMATION SocketArray[FD_SETSIZE];

//Check if a string is number
bool isNumber(const string& s)
{

    if (s.empty())
        return false;
    for (char const& ch : s) {
        if (std::isdigit(ch) == 0)
            return false;
    }
    return true;
}

vector<string> splitSpace(string tmp)
{
    vector<string> v;
    stringstream ss(tmp);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ' ');
        substr.erase(remove_if(substr.begin(), substr.end(), [](char s) {return s == ' '; }));
        if (!substr.empty())
            v.push_back(substr);
    }
    return v;
}

vector<int> parsePosition(string& str)
{
    vector<int> pstr;
    stringstream ss(str);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        if (isNumber(substr))
            pstr.push_back(stoi(substr));
        else
        {
            cout << "- Warning : Positions must be a number, ignored." << endl;
            continue;
        }
    }
    return pstr;

}

// convert a vector of string to a vector of position
void parsePositions(vector<vector<int>>& p, vector<string>& v)
{
    for (int i = 0; i < v.size(); i++)
    {
        vector<int> pstr = parsePosition(v[i]);
        if (pstr.size() == 2)
            p.push_back(pstr);
        else
            cout << "- Warning : Ignored an invalid position." << endl;;
    }
}

bool checkInside(vector<int> a, vector<vector<int>> b)
{
    if (a.size() != 2)
        return false;
    for (int i = 0; i < a.size(); i++)
    {
        if (b[0][i] >= a[i] || b[1][i] <= a[i])
            return false;
    }
    return true;
}


int main(int argc, char** argv)
{

    WSADATA wsaData;

    FD_SET ReadSet;

    DWORD Total;

    ULONG NonBlock;

    DWORD Flags;

    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ConnectSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;


    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed with error: " << iResult << std::endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Change the socket mode on the listening socket from blocking to
    // non-block so the application will not block waiting for requests

    NonBlock = 1;
    if (ioctlsocket(ListenSocket, FIONBIO, &NonBlock) == SOCKET_ERROR)
    {
        cout << "ioctlsocket() failed with error " << WSAGetLastError() << endl;
        return 1;
    }
    else
        cout << "ioctlsocket() is OK!" << endl;

    while (TRUE)
    {
        // Prepare the Read and Write socket sets for network I/O notification
        FD_ZERO(&ReadSet);
        //FD_ZERO(&WriteSet);

        // Always look for connection attempts
        FD_SET(ListenSocket, &ReadSet);

        // Set Read and Write notification for each socket based on the
        // current state the buffer.  If there is data remaining in the
        // buffer then set the Write set otherwise the Read set

        for (int i = 0; i < TotalSockets; i++)
               FD_SET(SocketArray[i]->Socket, &ReadSet);

        if ((Total = select(0, &ReadSet, NULL, NULL, NULL)) == SOCKET_ERROR)
        {
            cout << "select() returned with error " << WSAGetLastError() << endl;
            return 1;
        }
        else
            cout << "select() is OK! " << endl;

        // Check for arriving connections on the listening socket.
        if (FD_ISSET(ListenSocket, &ReadSet))
        {
            if ((ConnectSocket = accept(ListenSocket, NULL, NULL)) != INVALID_SOCKET)
            {
                // Set the accepted socket to non-blocking mode so the server will
                // not get caught in a blocked condition on WSASends
                ULONG Block = 0;
                if (ioctlsocket(ConnectSocket, FIONBIO, &Block) == SOCKET_ERROR)
                {
                    cout << "ioctlsocket(FIONBIO) failed with error " << WSAGetLastError() << endl;
                    return 1;
                }
                else
                    cout << "ioctlsocket(FIONBIO) is OK! " << endl;

                if (CreateSocketInformation(ConnectSocket) == FALSE)
                {
                    cout << "CreateSocketInformation(ConnectSocket) failed!" << endl;
                    return 1;
                }
                else
                    cout << "CreateSocketInformation() is OK!" << endl;
            }
            else
            {
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    cout << "accept() failed with error " << WSAGetLastError() << endl;
                    return 1;
                }
                else
                    cout << "accept() is fine! " << endl;
            }
        }
        Sleep(1000);
        char buffer[DATA_BUFSIZE];
        // Check each socket for Read and Write notification until the number
        // of sockets in Total is satisfied
        for (int i = 0; Total > 0 && i < TotalSockets; i++)
        {
            LPSOCKET_INFORMATION SocketInfo = SocketArray[i];
            // If the ReadSet is marked for this socket then this means data
            // is available to be read on the socket
            string run = "RUN";
            send(SocketInfo->Socket, run.c_str(), run.size(), 0);
            if (FD_ISSET(SocketInfo->Socket, &ReadSet))
            {
                memset(buffer, 0, sizeof(buffer));
                if (recv(SocketInfo->Socket, buffer, DATA_BUFSIZE, 0) == SOCKET_ERROR)
                {
                    if (WSAGetLastError() != WSAEWOULDBLOCK)
                    {
                        cout << "WSARecv() failed with error " << WSAGetLastError() << endl;
                        FreeSocketInformation(i);
                    }
                    else
                        cout << "WSARecv() is OK! " << endl;
                    continue;
                }
                // If the WriteSet is marked on this socket then this means the internal
                // data buffers are available for more data

                string str = buffer;

                vector<string> strV = splitSpace(str);
                if (strV.size() == 0)
                    continue;
                if (strV.size() < 4)
                {
                    cout << "Not enough information message :" << str << endl;
                    continue;
                }
                string sendStr = strV[0] + " ";
                sendStr += strV[1] + " ";
                string backStr = sendStr + strV[2];
                vector<vector<int>> tmp;
                vector<string> strPos(strV.begin() + 2, strV.end());
                parsePositions(tmp, strPos);
                if (tmp.size() < 2)
                {
                    cout << "Invalid in or out position" << endl;
                    continue;
                }
                int a = -1;
                for (int i = 0; i < TotalSockets; i++)
                {
                    if (checkInside(tmp[1], SocketArray[i]->vec))
                    {
                        a = i;
                        break;
                    }
                }
                if (a == -1)
                {
                    send(SocketInfo->Socket, backStr.c_str(), backStr.size(), 0);
                    continue;
                }
                sendStr += strV[3];
                if (send(SocketArray[a]->Socket, sendStr.c_str(), sendStr.size(), 0) == SOCKET_ERROR)
                {
                    if (WSAGetLastError() != WSAEWOULDBLOCK)
                    {
                        cout << "WSASend() failed with error " << WSAGetLastError() << endl;
                        FreeSocketInformation(i);
                    }
                    else
                        cout << "WSASend() is OK! " << endl;
                    continue;
                }
                else
                {
                      char buf[10];
                      int res;
                      res = recv(SocketArray[a]->Socket, buf,10, 0);
                      if (res > 0)
                      {
                           string state(buf,buf+2);
                           if (state != "OK")
                               send(SocketInfo->Socket, backStr.c_str(), backStr.size(), 0);
                           else
                           {
                               string success = "SUCCESSFULLY MOVED : " + str;
                               send(SocketInfo->Socket, success.c_str(), success.size(), 0);
                           }
                      }

                }
            }

        }
    }
}


BOOL CreateSocketInformation(SOCKET s)
{
    char recvbuf[512];
    int iResult;
    int recvbuflen = 512;
    LPSOCKET_INFORMATION SI;
    cout << "Accepted socket number" << endl;

    if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL)
    {
        cout << "GlobalAlloc() failed with error " << GetLastError() << endl;
        return FALSE;
    }
    else
        cout << "GlobalAlloc() for SOCKET_INFORMATION is OK! " << endl;
    // Prepare SocketInfo structure for use

    SI->Socket = s;
    SocketArray[TotalSockets] = SI;
    TotalSockets++;

    while (SI->vec.size() == 0)
    {
        memset(&recvbuf, 0, sizeof(recvbuf));
        iResult = recv(SI->Socket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            string tmp = recvbuf;
            vector<string> strVec = splitSpace(tmp);
            parsePositions(SI->vec, strVec);
        }
    }
    return(TRUE);
}

void FreeSocketInformation(DWORD Index)
{
    LPSOCKET_INFORMATION SI = SocketArray[Index];
    DWORD i;

    closesocket(SI->Socket);
    cout << "Closing socket number " << SI->Socket << endl;
    GlobalFree(SI);

    // Squash the socket array
    for (i = Index; i < TotalSockets; i++)
    {
        SocketArray[i] = SocketArray[i + 1];
    }

    TotalSockets--;
}