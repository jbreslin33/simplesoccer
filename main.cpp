//custom
#include "server.h"
#include "utility.h"
#include "footballGame.h"

//standard
#include <iostream>
#include <thread>

//berkeley socket for read server
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> /* for close() for socket */
#include <stdlib.h>

void serverThread(Server* server)
{
        while (server->mRunning == true)
        {
                server->update();
        }
}

void readSocketData(Server* server)
{
        int sock;
        struct sockaddr_in sa;
        char buffer[1024];
        ssize_t recsize;
        socklen_t fromlen;

        memset(&sa, 0, sizeof sa);
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(INADDR_ANY);
        sa.sin_port = htons(7654);
        fromlen = sizeof sa;

        sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (bind(sock, (struct sockaddr *)&sa, sizeof sa) == -1)
        {
                perror("error bind failed");
                close(sock);
                exit(EXIT_FAILURE);
        }

        for (;;)
        {
                recsize = recvfrom(sock, (void*)buffer, sizeof buffer, 0, (struct sockaddr*)&sa, &fromlen);

                if (recsize > 0)
                {
                        buffer[recsize] = 0;
                        char* cBuffer;
                        cBuffer = (char*)buffer;
                        std::string s = std::string(cBuffer);
                        std::string delimiter = ",";

                        server->mUtility->split(s,delimiter);
                        std::vector<std::string> stringVector = server->mUtility->split(s,delimiter);

                        server->mFootballGameVector.at(0)->processBuffer(stringVector);
                }
        }
}

int main(void)
{
	Server server;
        FootballGame* footballGame = new FootballGame(700, 400, &server, server.getNextFootballGameId());
        server.mFootballGameVector.push_back(footballGame);

        std::thread tServer          (serverThread, &server);
        std::thread tReadSocketData (readSocketData, &server);

        tServer.join();
        tReadSocketData.join();
        return 0;
}
