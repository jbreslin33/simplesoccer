#include "server.h"
#include "utility.h"
#include "footballGame.h"

Server::Server()
{
	mUtility = new Utility();
	mFootballGameIdCounter = 0;

	mRunning = true;
}

int Server::getNextFootballGameId()
{
	mFootballGameIdCounter++;
	return mFootballGameIdCounter;
}

void Server::update()
{
	for (int i = 0; i < mFootballGameVector.size(); i++)
	{
		mFootballGameVector.at(i)->Update();		
	}                 

}


void Server::log(std::string s)
{
        std::ofstream ofs;
        ofs.open ("server.log", std::ofstream::out | std::ofstream::app);
        ofs << s << std::endl;
        ofs.close();
}

