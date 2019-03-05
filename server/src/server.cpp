#include "server.h"

#include <sys/syscall.h>

#include "simplethreadpool.h"
ThreadPool pool(5);

std::mutex accountMap_mutex;

std::map<int, int> accountMap;//1 to 1

SocketServer::SocketServer(const string& ip, const int& port):m_ip(ip),m_port(port)
{
    configServer();
    listenSocket();
}

void SocketServer::closeSocket()
{
    close(m_socket);
}

void SocketServer::listenSocket()
{
    listen(m_socket, 20);//why 20?
}

int SocketServer::getSocket()
{
    return m_socket;
}

int SocketServer::readMessage(const int& socket_fd)
{
//    printf("sock is %d, readMessage is running...\n", socket_fd);

    char buffer[100];
    int ret = read(socket_fd, buffer, sizeof(buffer));//read is ok?
    common::MessageBody tmp;
    memcpy(&tmp, buffer, sizeof(tmp));
    printf("receive message from account %d.\n", tmp.source);

    return ret;
}

void transfer(int fd, char* buffer, int len)
{
    
    common::MessageBody tmp;
    memcpy(&tmp, buffer, len);

            //if dest fd is active, write message
    std::unique_lock<std::mutex> lock(accountMap_mutex);
    if (accountMap.find(tmp.dest) != accountMap.end())
    {
        printf("%d is online.\n", tmp.dest);
        int destSockFd = accountMap.find(tmp.dest)->second;
//                struct tcp_info info;
//                int len = sizeof(info);
//                getsockopt(destSockFd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*)&len);

//            if (info.tcpi_state == TCP_ESTABLISHED)
//            {
        write(fd, &tmp, sizeof(tmp));
        printf("transfer message from %d to %d\n", tmp.source, tmp.dest);
//            }
//            else
//            {
                //error
//                printf("account %d disconnected.\n", tmp.dest);
//                close(destSockFd);
//                accountMap.erase(tmp.dest);
//            }
    }
        //if dest fd is not active, store message
    else
    {
        printf("%d is offline\n", tmp.dest);
    }

    delete [] buffer;
}

void handle()
{
    for(auto i : accountMap)
    {
//        char buffer[100];
        char *buffer = new char[100];
        if (read(i.second, buffer, sizeof(buffer)))
        {
            printf("%d account is active.", i.first);
            pool.addTask(std::bind(transfer, i.second, buffer, sizeof(buffer)));        
        }
    }
}

void SocketServer::handleMessage(const int& socket_fd)
{ 
//    int signal = 0;
//    int heartbeat = 0;
//    int displayConn = 0;
//
//    while(true)
//    {
//        char buffer[100];//
//        signal = read(socket_fd, buffer, sizeof(buffer));
//        if (signal <= 0)//heartbeat limited
//        {
//            ++heartbeat;
//            if (heartbeat == 3)
//            {
//                printf("no message received, thread shutdown.\n");
//                break;
//            }
//            
//            continue;
//        }
//
//        common::MessageBody tmp;
//        memcpy(&tmp, buffer, sizeof(tmp));
//
//        //if dest fd is active, write message
//        std::unique_lock<std::mutex> lock(accountMap_mutex);
//        if (accountMap.find(tmp.dest) != accountMap.end())
//        {
//            printf("%d is online.\n", tmp.dest);
//            int destSockFd = accountMap.find(tmp.dest)->second;
//            struct tcp_info info;
//            int len = sizeof(info);
////            getsockopt(destSockFd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*)&len);
////            if (info.tcpi_state == TCP_ESTABLISHED)
////            {
//                write(destSockFd, &tmp, sizeof(tmp));
//                printf("transfer message from %d to %d\n", tmp.source, tmp.dest);
////            }
////            else
////            {
//                //error
////                printf("account %d disconnected.\n", tmp.dest);
////                close(destSockFd);
////                accountMap.erase(tmp.dest);
////            }
//        }
//        //if dest fd is not active, store message
//        else
//        {
//            printf("%d is offline\n", tmp.dest);
//        }
//
//    }
}

void SocketServer::listenClients()
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);


    while(true){

    int client_sock = accept(m_socket, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_sock > 0)
    {
        //register new client
        char buffer[100];
        int ret = read(client_sock, buffer, sizeof(buffer));//read is ok?
        common::MessageBody tmp;
        memcpy(&tmp, buffer, sizeof(tmp));
        printf("account %d register server.\n", tmp.source);

        std::unique_lock<std::mutex> lock(accountMap_mutex);
        accountMap.insert(pair<int, int>(tmp.source, client_sock));
        printf("account %d connected.\n", tmp.source);

//        std::cout << syscall(SYS_gettid) << " is doing." << std::endl;
//        pool.addTask(std::bind(&SocketServer::handleMessage, this, client_sock));
    }
//		else
//		{
//			printf("are you connecting again???\n");
//			sleep(2);
//		}


    }
}

void SocketServer::run()
{
    std::thread listenThread(&SocketServer::listenClients, this);
    listenThread.detach();

    while(true)
    {
        //listenClients();
        //pool.addTask(std::bind(handle));
        handle();
    }
}

void SocketServer::configServer()
{    
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in socket_addr;
    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = inet_addr(m_ip.data());
    socket_addr.sin_port = htons(m_port);

//    int optionVal = 1;
//    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&optionVal, sizeof(optionVal)) < 0)
//    {
//        printf("setsockopt error!");
//    }

    bind(m_socket, (struct sockaddr*)&socket_addr, sizeof(socket_addr));

}
