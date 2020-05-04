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

                int mScreenX;
                int mScreenY;

                /******       outside variables ***************/
                Server* mServer;

                /******       game life ***************/
                bool  GameOn()const{return m_bGameOn;}
                void  SetGameOn(){m_bGameOn = true;}
                void  SetGameOff(){m_bGameOn = false;}
                //true if the game is in play. Set to false whenever the players
                //are getting ready for kickoff
                bool                 m_bGameOn;

                //set true to pause the motion
                bool                 m_bPaused;

                void  TogglePause(){m_bPaused = !m_bPaused;}
                bool  Paused()const{return m_bPaused;}

                //client no need for virtual
                void sendToClient(Client* client, std::string message);

                virtual void sendDataToNewClients() {}
                int getNextClientId();
                std::vector<Client*> mClientVector;
                int mClientIdCounter;

                //id
                int mId;

                /******       Utility ***************/
                Utility* mUtility;

                /******       Time and ticks ***************/
                long getCurrentMilliseconds();
                virtual void tick() {};
                void  Update();
                void processBuffer(std::vector<std::string> stringVector);
                virtual void processMove(std::vector<std::string> stringVector) {};
                //time
                long mGameStartTime;
                long mLastTime;
                long mDelta;
                long mTickCount;

               	int FrameRate;

                //client
                void requestClient(std::vector<std::string> stringVector);
                virtual void sendMovesToClients() {};

};
#endif
