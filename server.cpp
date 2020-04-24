#include "server.h"
#include "utility.h"
#include "SoccerPitch.h"

Server::Server()
{
	mUtility = new Utility();
	mSoccerPitchIdCounter = 0;

	mRunning = true;
	printf("Server Constructor\n");
}

int Server::getNextSoccerPitchId()
{
	mSoccerPitchIdCounter++;
	return mSoccerPitchIdCounter;
}

void Server::update()
{
	printf("Server::update()\n");
	for (int i = 0; i < mSoccerPitchVector.size(); i++)
	{
		printf("Server::update() 1\n");
		mSoccerPitchVector.at(i)->Update();		
	}                 

}


void Server::log(std::string s)
{
        std::ofstream ofs;
        ofs.open ("server.log", std::ofstream::out | std::ofstream::app);
        ofs << s << std::endl;
        ofs.close();
}

