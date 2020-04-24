//berkeley socket for read server
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> /* for close() for socket */
#include <stdlib.h>

#include <iostream>
#include <thread>

#include "server.h"
#include "utility.h"
#include "SoccerPitch.h"



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
                //printf("waiting on port %d\n", 7654);

                recsize = recvfrom(sock, (void*)buffer, sizeof buffer, 0, (struct sockaddr*)&sa, &fromlen);

                if (recsize > 0)
                {
                        buffer[recsize] = 0;
                        char* cBuffer;
                        cBuffer = (char*)buffer;
                        std::string s = std::string(cBuffer);
                        //printf("recfrom on server:%s\n",s.c_str());
                        std::string delimiter = ",";

                        server->mUtility->split(s,delimiter);
                        std::vector<std::string> stringVector = server->mUtility->split(s,delimiter);

                        server->mSoccerPitchVector.at(0)->processBuffer(stringVector);
                }
        }
}

int main(void)
{
	Server server;
        SoccerPitch* soccerPitch = new SoccerPitch(104, 68, &server, server.getNextSoccerPitchId());
        server.mSoccerPitchVector.push_back(soccerPitch);

        std::thread tServer          (serverThread, &server);
        std::thread tReadSocketData (readSocketData, &server);

        tServer.join();
        tReadSocketData.join();
        return 0;
}
