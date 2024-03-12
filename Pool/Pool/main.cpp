#define WIN32_LEAN_AND_MEAN
#include "fishfactory.h"
#include "squarepool.h"
#include "circlepool.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <future>   
#include <vector>
#include <sstream>
#include <algorithm>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

#define DLL "*.dll"

static std::vector<FishFactory*> factories;
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8080"

map<string, Fish*> typeMap;
ofstream poolLog;
string initMessage;
bool run = true;
bool timeout = false;
bool tryConnect = false;
int step = 0;

//Load dlls types of fishes.
void loadFactories(std::vector<FishFactory*>& factories, const vector<Position> IllegalPos)
{
    poolLog << "----------------------------Start loading types of fishes----------------------------" << endl;
    cout << "----------------------------Start loading types of fishes----------------------------" << endl;
    std::vector<HMODULE> handles;
    WIN32_FIND_DATA find_data;
    HANDLE find_handle = FindFirstFile(DLL, &find_data);
    if (find_handle != INVALID_HANDLE_VALUE) {
        do {
            HMODULE handle = LoadLibrary(find_data.cFileName);
            if (handle) {
                typedef FishFactory* (*create_factory_t)(vector<Position>);
                create_factory_t create_factory = (create_factory_t)GetProcAddress(handle, "create_factory");
                if (create_factory) {
                    FishFactory* factory = create_factory(IllegalPos);
                    factories.push_back(factory);
                    handles.push_back(handle);
                }
                else {
                    FreeLibrary(handle);
                }
            }
        } while (FindNextFile(find_handle, &find_data));
    }
    FindClose(find_handle);
    poolLog << "- Loaded " << factories.size() << " types of fishes." << endl;
    cout << "- Loaded " << factories.size() << " types of fishes." << endl;
    for (FishFactory* factory : factories) {
        typeMap[factory->name()] = factory->create();
        poolLog << factory->name() << " , ";
        cout << factory->name() << " , ";
    }
    poolLog << endl;
    cout << endl;

}
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

//Open a file and put it to ifstream
ifstream open(string fileName)
{
    ifstream configFile(std::string(configFolder) + fileName);
    if (!configFile.good())
    {
        poolLog << "ERROR: File "<< fileName <<" is not valid." << endl;
        cout << "ERROR: File " << fileName << " is not valid." << endl;
        exit(1);
    }
    return configFile;
}

//Load number of dimension from file.
int getD()
{
    ifstream configFile = open("dimension.txt");
    string tmp;
    getline(configFile, tmp);
    if (isNumber(tmp)&& stoi(tmp) >0)
        return stoi(tmp);

}

const int DIM = getD();

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

string posToString(vector<int> vec)
{
    string str;
    if (vec.size() == 0)
        return str;
    for (int i = 0; i < vec.size(); i++)
        str += to_string(vec[i]) + ",";
    
    str.erase(str.end() - 1);
    str += " ";
    return str;
}

vector<int> parsePosition( string& str)
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
                poolLog << "- Warning : Positions must be a number, ignored." << endl;
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
        if (pstr.size() == DIM)
            p.push_back(pstr);
        else
        {
            poolLog << "- Warning : Ignored an invalid position." << endl;
            cout << "- Warning : Ignored an invalid position." << endl;;
        }
    }
}

// Parse data of pool, obtacles from config file.
Pool* parseConfig()
{
    poolLog << "----------------------------Start loading pool config----------------------------" << endl;
    cout << "----------------------------Start loading pool config----------------------------" << endl;
    Pool* pool=nullptr;
    vector<Obstacle*> obs;
    vector<vector<int>> send;
    ifstream configFile = open("config.txt");
    string tmp;
    while (getline(configFile, tmp)) 
    {
        vector<string> v = splitSpace(tmp);

        if (v.size() < 3)
            continue;
        vector<vector<int>> p;
        vector<string> strPos(v.begin() + 2, v.end());
        parsePositions(p, strPos);
        if (v[0] == "OBSTACLE")
        {
            if (p.size() > 0)
                obs.push_back(new Obstacle(p, v[1]));
        }
        else if (v[0] == "POOL")
        {
            if (v[1] == "CIRCLE")
                if (isNumber(v[2]) && stoi(v[2]) > 1)
                {
                    pool = new CirclePool(stoi(v[2]), DIM);
                }
                else
                {
                    poolLog << "- ERROR : Radius must be a positive number and dimensions must > 1" << endl;
                    cout << "- ERROR : Radius must be a positive number and dimensions must > 1" << endl;
                }
            else if (v[1] == "SQUARE")
            {
                if (p.size() > 1)
                {
                    pool = new SquarePool(p[0],p[1],send);
                }
                else
                {
                    poolLog << "- ERROR : Pool size invalid." << endl;
                    cout << "- ERROR : Pool size invalid." << endl;
                }
            }
            else if (v[1] == "SEND")
            {
                if (p.size() > 0)
                {
                    send = p;
                }
            }
        }

    }
    if(pool !=nullptr)
        for (auto ob : obs)
            pool->setObstacle(ob);
    else
    {
        poolLog << "ERROR : Can not create pool. Please check config file again" << endl;
        cout << "ERROR : Can not create pool. Please check config file again" << endl;
        exit(1);
    }
    poolLog << "Created pool successfully" << endl;
    cout << "Created pool successfully" << endl;
    return pool;
}


Pool* setUp()
{

    Pool* pool = parseConfig();

    loadFactories(factories, pool->getIllegalPos());
    poolLog << "Please enter the number of each type you want to put into the pool :" << endl;
    cout << "Please enter the number of each type you want to put into the pool :" << endl;
    for (FishFactory* factory : factories) {
        poolLog << factory->name() << " : ";
        cout << factory->name() << " : ";
        int n=-1;
        while(n<0)
            cin >> n;
        for (int i=0; i < n; i++)
        {
            Fish* f = factory->create();
            pool->addFish(f);
        }
    }
    return pool;
}

SOCKET ConnectSocket;

SOCKET init()
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    int iResult;
    // Validate the parameters

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        poolLog << "WSAStartup failed with error: " << iResult << endl;
        cout << "WSAStartup failed with error: " << iResult << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        poolLog << "getaddrinfo failed with error: " << iResult << endl;
        cout << "getaddrinfo failed with error: " << iResult << endl;
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);

        // Set timeout for the socket
        int timeoutInMilliseconds = 5000; // 5 seconds
        setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutInMilliseconds, sizeof(timeoutInMilliseconds));

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
            ULONG Block = 0;
            if (ioctlsocket(ConnectSocket, FIONBIO, &Block) == SOCKET_ERROR)
            {
                cout << "ioctlsocket(FIONBIO) failed with error " << WSAGetLastError() << endl;
                return 1;
            }
            else
                cout << "ioctlsocket(FIONBIO) is OK! " << endl;
            break;
        }
    }
    freeaddrinfo(result);
    if (ConnectSocket == INVALID_SOCKET) {
        poolLog << "Unable to connect to server! " << endl;
        cout << "Unable to connect to server! " << endl;
        WSACleanup();
        return 1;
    }
    send(ConnectSocket, initMessage.c_str(), initMessage.length(), 0);
    return ConnectSocket;
}
void reconnect()
{
    ConnectSocket = INVALID_SOCKET;
    while (ConnectSocket == INVALID_SOCKET)
    {
        ConnectSocket = init();
        cout << "Trying to reconnect to the server..." << endl;
        Sleep(500);
    }
}
void Send(string f)
{
    // Send an initial buffer
    int iResult = send(ConnectSocket, f.c_str(), f.length(), 0);
    if (iResult == SOCKET_ERROR) {
        poolLog << "send failed with error: " << WSAGetLastError() << endl;
        cout << "send failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        thread t(reconnect);
        t.detach();
    }
    else if (f != " ")
    {
        poolLog << "\033[1;31mSENT:\033[0m" << f << endl;
        cout << "\033[1;31mSENT:\033[0m" << f << endl;
    }

}

void Recv(Pool* pool)
{
    char recvbuf[512];
    int iResult;
    int recvbuflen = 512;
    // Receive until the peer closes the connection
    while (1)
    {
        memset(&recvbuf, 0, sizeof(recvbuf));
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

        if (iResult > 0)
        {
            std::string str(recvbuf);
            vector<string> vec = splitSpace(str);
            if (vec.size() < 3)
            {
                if (str == "RUN")
                    run = true;
            }
            else if (vec.size() > 3)
            {
                cout << str << endl;
                poolLog << str << endl;
            }
            else if (typeMap.count(vec[0]) == 0 || !isNumber(vec[1]))
            {
                poolLog << "Warning : Fish name or strength incorect, ignore." << endl;
                cout << "Warning : Fish name or strength incorect, ignore." << endl;
            }
            else
            {
                Fish* f = typeMap[vec[0]];
                vector<int> vs = parsePosition(vec[2]);
                Position fPos(vs);
                vector<Position> ilPos = pool->getIllegalPos();
                if (std::find(ilPos.begin(), ilPos.end(), fPos) == ilPos.end())
                {
                    Send("OK");
                    if (vs.size() == DIM)
                        f->deserialize(stoi(vec[1]), vs);
                    pool->addFish(f);
                    poolLog << "\033[1;31mRECEIVED:\033[0m" << str << endl;
                    cout << "\033[1;31mRECEIVED:\033[0m" << str << endl;
                }
                else
                    Send("ER");
            }
        }

    }
}

void timeOut()
{
    while (1)
    {
        int tmp = step;
        Sleep(3000);
        timeout = step == tmp;
    }
}

int main()
{   
    poolLog.open("log.txt");
    Pool* pool;
    pool = setUp();
    initMessage = posToString(pool->getBot()) + posToString(pool->getTop());
    ConnectSocket = init();
    std::thread t1(Recv, pool);
    t1.detach();
    std::thread t2(timeOut);
    t2.detach();
    while(pool->getFishes().size() >1)
    {
        Send(" ");
        while (!run && !timeout)
            continue;
        if (timeout)
        {
            if (!tryConnect)
            {
                thread t(reconnect);
                t.detach();
                tryConnect = true;
            }
            Sleep(1000);
        }
        step ++;
        pool->show(poolLog);
        for (int i = 0; i < pool->getFishes().size(); i++)
        {
            Position tar = pool->getFishes()[i]->getTarget();
            vector<Position> vp = pool->getSendPos();
            if (std::find(vp.begin(), vp.end(), tar) != vp.end())
            {
                string str;
                pool->getFishes()[i]->serialize(str);
                Send(str);
                pool->sendFish(i);
            }
        }
        pool->makeMove();
        pool->fight(poolLog);
        run = false;
    }
    poolLog << "---------------------Game over after " << step << " steps--------------------------" << endl;
    cout << "---------------------Game over after " << step<< " steps--------------------------" << endl;
    pool->show(poolLog);

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
    for (FishFactory* factory : factories) {
        delete factory;
    }
    for (auto fish : pool->getFishes())
        delete fish;
      
    return 0;
}
