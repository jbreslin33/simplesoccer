#ifndef FOOTBALLGAME_H
#define FOOTBALLGAME_H
//------------------------------------------------------------------------
//
//  Name:   footballGame.h
//
//  Desc:   A FootballGame is the main game object. It owns instances of
//          two soccer teams, two goals, the playing area, the ball
//          etc. This is the root class for all the game updates and
//          renders etc
//
//  Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
//custom
#include "game.h"
#include "2D/Vector2D.h"
#include "constants.h"

//standard
#include <vector>
#include <cassert>

class SoccerTeam;
class SoccerBall;
class Server;
class FootballPitch;

class FootballGame : public Game
{ 
	public:

  		FootballGame(int screenX, int screenY, Server* server, int id);
  		~FootballGame();

		//Pitch field of play, can change size....
		FootballPitch* mFootballPitch;

		//start game
		void startGame(std::vector<std::string> stringVector);

		/*****************Game ***********/
		void tick();
		void processMove(std::vector<std::string> stringVector);

		/*********** Clients ****************/
                void sendDataToNewClients();
		void sendMovesToClients();

		/******       Teams   ***************/
  		std::vector<SoccerTeam*>  mTeamVector;

		/******       Ball   ***************/
		SoccerBall*const getBall()const;
		SoccerBall* mBall;
};

#endif
