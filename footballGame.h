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
		//pointers to the team members
  		std::vector<SoccerTeam*>  mTeamVector;

		/******       Ball   ***************/
		SoccerBall*const Ball()const;
		SoccerBall* mBall;
  
		/******       game play member variables ***************/
		
		//true if a goal keeper has possession
  		bool                 m_bGoalKeeperHasBall;

		double MinPassDistance;
		double GoalkeeperMinPassDistance;

		double Spot_CanPassScore;
		double WithinRangeOfSweetSpot;
    
    		double Spot_PassSafeScore;
    		double Spot_CanScoreFromPositionScore;
    		double Spot_DistFromControllingPlayerScore;
    		double Spot_ClosenessToSupportingPlayerScore;
    		double Spot_AheadOfAttackerScore;

    		double SupportSpotUpdateFreq; 
    
    		double ChancePlayerAttemptsPotShot;
    		double ChanceOfUsingArriveTypeReceiveBehavior;
    
    		double BallSize;    
    		double BallMass;    
    		double Friction; 
    
    		double KeeperInBallRange;    
    		double PlayerInTargetRange; 
    		double PlayerKickingDistance; 
    		double PlayerKickFrequency;

    		double PlayerComfortZone;  
    		double PlayerKickingAccuracy;

    		int    NumAttemptsToFindValidStrike;

    		double MaxDribbleForce;    
    		double MaxShootingForce;    
    		double MaxPassingForce;  
    
    		double WithinRangeOfHome;    
    		double WithinRangeOfSupportSpot;    
    
   	 	double MinPassDist;
    		double GoalkeeperMinPassDist;
   	 
    		double GoalKeeperTendingDistance;    
   	 	double GoalKeeperInterceptRange;
    		double BallWithinReceivingRange;
    
    		bool bStates;    
    		bool bIDs; 
   		bool bSupportSpots;     
    		bool bRegions;
    		bool bShowControllingTeam;
    		bool bViewTargets;
    		bool bHighlightIfThreatened;

    		double SeparationCoefficient; 
    		double ViewDistance; 
    		bool bNonPenetrationConstraint; 

		//squared space
		double BallWithinReceivingRangeSq;
		double KeeperInBallRangeSq;
		double PlayerInTargetRangeSq;
		double PlayerKickingDistanceSq;
		double PlayerComfortZoneSq;
		double GoalKeeperInterceptRangeSq;
		double WithinRangeOfSupportSpotSq;

 		bool  GoalKeeperHasBall()const{return m_bGoalKeeperHasBall;}
  		void  SetGoalKeeperHasBall(bool b){m_bGoalKeeperHasBall = b;}

};

#endif
