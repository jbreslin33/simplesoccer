#include "server.h"
#include "utility.h"
#include "SoccerPitch.h"

Server::Server()
{
	mUtility = new Utility();
	mSoccerPitchIdCounter = 0;

	mRunning = true;
}

int Server::getNextSoccerPitchId()
{
	mSoccerPitchIdCounter++;
	return mGameIdCounter;
}

void Server::update()
{
	for (int i = 0; i < mSoccerPitchVector.size(); i++)
	{
		mSoccerPitchVector.at(i)->update();		
	}                 

}


void Server::log(std::string s)
{
        std::ofstream ofs;
        ofs.open ("server.log", std::ofstream::out | std::ofstream::app);
        ofs << s << std::endl;
        ofs.close();
}

