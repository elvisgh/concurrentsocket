#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "structure.h"

using namespace std;

class ClientInterface
{
public:
    ClientInterface(){};
    ClientInterface(const string& ip, const int& port):m_ip(ip),m_port(port)
    {
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        struct sockaddr_in socket_addr;
        memset(&socket_addr, 0, sizeof(socket_addr));
        socket_addr.sin_family = AF_INET;
        socket_addr.sin_addr.s_addr = inet_addr(ip.data());
        socket_addr.sin_port = htons(port);
        
        bool l = true;
        if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&l, sizeof(l)) < 0)
        {
            printf("setsockopt error!\n");
        }
        
        bind(m_socket, (struct sockaddr*)&socket_addr, sizeof(socket_addr));

    }

    int connectServer(const string& serverIP, const int& serverPort)
    {  
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(serverIP.data());
        server_addr.sin_port = htons(serverPort);

        return connect(m_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    }

    int registerServer()
    {
        common::MessageBody mb;
        mb.source = 110;
        return write(m_socket, &mb, sizeof(mb));
    }

    int writeMessage()
    {
        int result = -1;
		while(true)
		{
	    	common::MessageBody writeMB;
	    	//strcpy(writeMB.destIp, "127.0.0.1");
	    	//writeMB.destPort = 12336;
	    	//strcpy(writeMB.message, "hello, i am 12334");

            writeMB.source = 110;//
            writeMB.dest = 120;
            strcpy(writeMB.message, "hello 120, this is 110");

	    	//printf("send message to %s %d\n", writeMB.destIp, writeMB.destPort);
	    	printf("send message to client %d\n", writeMB.dest);

	    	char buffer[100];
	    	memcpy(buffer, &writeMB, sizeof(writeMB));
	    	result = write(m_socket, buffer, sizeof(buffer));
	    	
	    	sleep(2);
		}
        return result;
    }

    void readMessage()
    {
		while(true)
		{
             char buffer[100];
             
             if (read(m_socket, buffer, sizeof(buffer)))
             {
	         	common::MessageBody readMB;
	         	memcpy(&readMB, buffer, sizeof(readMB));
	         	printf("receive message from  server : %s\n", readMB.message);
             }
             else
             {
                 //read failure
                 printf("server crashed? reconnect...\n");
                 while(connectServer("127.0.0.1", 12333) != 0);
             }
		}
    }

    void closeSocket()
    {
        close(m_socket);
    }

private:
    string m_ip;
    int m_port;
    int m_socket;
};

int main()
{
    ClientInterface clientDemo("127.0.0.1", 12334);

    int signal = clientDemo.connectServer("127.0.0.1", 12333);
    clientDemo.registerServer();

	std::thread readThread = std::thread(&ClientInterface::readMessage, &clientDemo);
	std::thread writeThread = std::thread(&ClientInterface::writeMessage, &clientDemo);
	
	readThread.detach();
	writeThread.detach();

	while(true)
    {
        //keep client running
    	if (signal == 0)
    	{
        	//printf("connect server sucessfully.\n");
			//clientDemo.writeMessage();
			//sleep(5);
    	}
		else
		{
			printf("connect failure, reconnecting\n");
			signal = clientDemo.connectServer("127.0.0.1", 12333);
			sleep(2);
		}
    }

    clientDemo.closeSocket();
    return 0;
}
