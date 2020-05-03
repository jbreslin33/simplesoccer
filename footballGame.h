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
#include <vector>
#include <cassert>

#include "2D/Vector2D.h"
#include "constants.h"

class SoccerTeam;
class SoccerBall;
class SoccerTeam;
class PlayerBase;
class Server;
class Client;
class Utility;
class FootballPitch;

class FootballGame
{ 
	public:

  		FootballGame(int screenX, int screenY, Server* server, int id);
  		~FootballGame();

		FootballPitch* mFootballPitch;

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

		//client
		void sendToClient(Client* client, std::string message);
		void sendDataToNewClients();
                int getNextClientId();
                std::vector<Client*> mClientVector;
                int mClientIdCounter;
  	
		//id	
		int mId;

		/******       Teams   ***************/
  		SoccerTeam*          m_pRedTeam;
  		SoccerTeam*          m_pBlueTeam;

		/******       Ball   ***************/
		SoccerBall*const           Ball()const{return m_pBall;}
		SoccerBall*          m_pBall;
  
		/******       Utility ***************/
		Utility* mUtility;
               
		/******       Time and ticks ***************/
		long getCurrentMilliseconds();
		void tick();
	  	void  Update();
  		void processBuffer(std::vector<std::string> stringVector);
	  	void processMove(std::vector<std::string> stringVector);
		//time
                long mGameStartTime;
                long mLastTime;
                long mDelta;
                long mTickCount;

		//client
		void requestClient(std::vector<std::string> stringVector);
		void sendMovesToClients();
		
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

    		int FrameRate;

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
