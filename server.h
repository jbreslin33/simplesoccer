#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <iostream>
#include <fstream>

class FootballGame;
class Utility;

class Server
{
	private:

	public:
		Server();
		void update();
		int getNextFootballGameId();
	       	void log(std::string s);

		Utility* mUtility;

		std::vector<FootballGame*> mFootballGameVector;

		int mFootballGameIdCounter;
		bool mRunning;
};
#endif
