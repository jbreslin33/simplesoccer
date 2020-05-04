//custome
#include "game.h"
#include "client.h"

//standard
#include <string.h>
#include <sys/time.h>

//Berkeley
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


Game::Game(int screenX, int screenY, Server* server, int id)
{
        mScreenX = screenX;
        mScreenY = screenY;

        mGameOn = true;
        mPaused = true;

        mId = id;
        mServer = server;

        //time
        mGameStartTime = getCurrentMilliseconds();
        mLastTime = mGameStartTime;
        mDelta = 0;
        mTickCount = 0;

        //clients
        mClientIdCounter = 0;
}

Game::~Game()
{
}

void Game::update()
{
        mDelta = getCurrentMilliseconds() - mLastTime;

        if (mDelta > FrameRate)
        {
                tick();
                mLastTime = getCurrentMilliseconds();
        }
}

//getters and setters
bool Game::getGameOn()
{
	return mGameOn;
}

void Game::setGameOn(bool b)
{
	mGameOn = b;
}

bool Game::getPaused()
{
	return mPaused;
}

void Game::setPaused(bool b)
{
	mPaused = b;	
}

int Game::getNextClientId()
{
        mClientIdCounter++;
        return mClientIdCounter;
}


void Game::sendToClient(Client* client, std::string message)
{
        int sock;
        struct sockaddr_in sa;
        int bytes_sent;
        char buffer[1000];

        strcpy(buffer, message.c_str());

        /* create an Internet, datagram, socket using UDP */
        sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == -1)
        {
                /* if socket failed to initialize, exit */
                printf("Error Creating Socket");
                exit(EXIT_FAILURE);
        }

        /* Zero out socket address */
        memset(&sa, 0, sizeof sa);

        /* The address is IPv4 */
        sa.sin_family = AF_INET;

        /* IPv4 adresses is a uint32_t, convert a string representation of the octets to the appropriate value */
        sa.sin_addr.s_addr = inet_addr("127.0.0.1");

        /* sockets are unsigned shorts, htons(x) ensures x is in network byte order, set the port to 7654 */
        sa.sin_port = htons(client->mPort);

        bytes_sent = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&sa, sizeof sa);
        if (bytes_sent < 0)
        {
                printf("Error sending packet: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }

        close(sock); /* close the socket */
}

long Game::getCurrentMilliseconds()
{
        struct timeval tp;
        gettimeofday(&tp, NULL);
        long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
        return ms;
}

void Game::processBuffer(std::vector<std::string> stringVector)
{
        //requestPlayer(stringVector);
        /*
        if (stringVector.at(1).compare(0,1,"m") == 0)
        {
                processMove(stringVector);
        }
       */
        if (stringVector.at(1).compare(0,1,"j") == 0)
        {
                requestClient(stringVector);
        }

        if (stringVector.at(1).compare(0,1,"p") == 0)
        {
		printf("p\n");
		setPaused(true);
        }

        if (stringVector.at(1).compare(0,1,"u") == 0)
        {
		printf("u\n");
		setPaused(false);
        }

        if (stringVector.at(1).compare(0,1,"g") == 0)
        {
                //startGame(stringVector);
        }
}

void Game::requestClient(std::vector<std::string> stringVector)
{
        int personIdInt = atoi(stringVector.at(2).c_str());
        int portInt = atoi(stringVector.at(3).c_str());

        // if this person requesting already has a client instance on server than rock on and just update the port if needed
        for (int c = 0; c < mClientVector.size(); c++)
        {
                if (mClientVector.at(c)->mPersonId == personIdInt)
                {
                        if (mClientVector.at(c)->mPort == portInt)
                        {
                                printf("Person ID:%d already has a client and port is good to go at:%d.\n",personIdInt, portInt);
                        }
                        else
                        {
                                mClientVector.at(c)->mPort = portInt;
                                mClientVector.at(c)->mSentToClient = false;
                                printf("Person ID:%d already has a client. Updating port to: %d\n and sending confirmation to client again.\n",personIdInt, portInt);

                        }
                        return;
                }
        }

        //fall thru make a new client instance
        Client* client = new Client(this,getNextClientId(),0,0);
        mClientVector.push_back(client);
        client->mPersonId = personIdInt;
        printf("Make new Client for Person ID:%d \n",personIdInt);
}



