#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <iostream>
#include <fstream>

class SoccerPitch;
class Utility;

class Server
{
	private:

	public:
		Server();
		void update();
		int getNextSoccerPitchId();
	       	void log(std::string s);

		Utility* mUtility;

		std::vector<SoccerPitch*> mSoccerPitchVector;

		int mSoccerPitchIdCounter;
		bool mRunning;
};
#endif
