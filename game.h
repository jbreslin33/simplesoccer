#ifndef GAME_H
#define GAME_H

//standard
#include <string>
#include <iostream>
#include <vector>

class Server;
class Client;
class Utility;

class Game
{
	public: 
                Game(int screenX, int screenY, Server* server, int id);
		~Game();
                
		//id. We are possibly just one of many games running on this server. This will help identify us.
                int mId;

		//What we are basing game world off of. Not to be confused with actual client screen as we do not know what the client screen is.
		//so the client will try to model this screen by scaling up or down.
                int mScreenX;
                int mScreenY;

		//the server can run multiple games. So we are possibly just one of many games running.
		Server* mServer;

               	//this is used start, stop and pause the game 
                void setGameOn(bool gameOn);
                bool getGameOn();
                bool mGameOn;

                void setPaused(bool paused);
                bool getPaused();
                bool mPaused;

                //clients 
                int getNextClientId();
                int mClientIdCounter;
                std::vector<Client*> mClientVector;

                void sendToClient(Client* client, std::string message);

		//game specific so make virtual atleast for game. Maybe all football games will use same function though.
		//lets clients know what to setup. Number of players, colors size of field, etc.
                virtual void sendDataToNewClients() {}

               	//used for random usefull functions that don't have a class home as yet. 
		Utility* mUtility;

		//client moves
                void processBuffer(std::vector<std::string> stringVector);
                virtual void processMove(std::vector<std::string> stringVector) {};
               
	       	//called by server as fast as possible. Server may have other games and sockets to deal with.	
		void update();

		//This is where the game loop is. Each game may implement it's own tick so make it virtual. 
                virtual void tick() {};

               	//time 
		long getCurrentMilliseconds();
                long mGameStartTime;
                long mLastTime;
                long mDelta;
                long mTickCount;

		//frameRate. How long to wait until you process another tick
               	int FrameRate;

                //give client a connection client.
                void requestClient(std::vector<std::string> stringVector);

		//start game. This comes from the client
                virtual void startGame(std::vector<std::string> stringVector) {};

		//send positions, orientations to clients so they can move their local objects to mirror server game world.
                virtual void sendMovesToClients() {};
};
#endif
