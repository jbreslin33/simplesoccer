#ifndef SOCCERPITCH_H
#define SOCCERPITCH_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   SoccerPitch.h
//
//  Desc:   A SoccerPitch is the main game object. It owns instances of
//          two soccer teams, two goals, the playing area, the ball
//          etc. This is the root class for all the game updates and
//          renders etc
//
//  Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
//#include <windows.h>
#include <vector>
#include <cassert>

#include "2D/Wall2D.h"
#include "2D/Vector2D.h"
#include "constants.h"

class Region;
class Goal;
class SoccerTeam;
class SoccerBall;
class SoccerTeam;
class PlayerBase;
class Server;
class Client;


class SoccerPitch
{ 
	public:

		SoccerBall*          m_pBall;
		double MinPassDistance                  = 120.0;
		double GoalkeeperMinPassDistance;
		int ViewStates;
		int ViewIDs;
		int ViewSupportSpots;
		int ViewRegions;
		int ViewTargets;
		int HighlightIfThreatened;
  
		//params
  		double GoalWidth; 
    
    		int NumSupportSpotsX;    
    		int NumSupportSpotsY;   

		int NumSweetSpotsX;
		int NumSweetSpotsY;

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

    		double PlayerMass; 
    		double PlayerMaxForce;    
    		double PlayerMaxSpeedWithBall;   
    		double PlayerMaxSpeedWithoutBall;   
    		double PlayerMaxTurnRate;   
    		double PlayerScale;      
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


  		int mId;

  		SoccerTeam*          m_pRedTeam;
  		SoccerTeam*          m_pBlueTeam;

  		Goal*                m_pRedGoal;
  		Goal*                m_pBlueGoal;
   
  		//container for the boundary walls
  		std::vector<Wall2D>  m_vecWalls;

 		//defines the dimensions of the playing area
  		Region*              m_pPlayingArea;

  		//the playing field is broken up into regions that the team
  		//can make use of to implement strategies.
  		std::vector<Region*> m_Regions;

  		//true if a goal keeper has possession
  		bool                 m_bGoalKeeperHasBall;

  		//true if the game is in play. Set to false whenever the players
  		//are getting ready for kickoff
  		bool                 m_bGameOn;

  		//set true to pause the motion
  		bool                 m_bPaused;

  		//local copy of client window dimensions
  		int                  m_cxClient;
       		int               m_cyClient;  
  
  		//this instantiates the regions the players utilize to  position
 	 	//themselves
  		void CreateRegions(double width, double height);

  		Server* mServer;

	public:

  		SoccerPitch(int cxClient, int cyClient, Server* server, int id);

  		~SoccerPitch();

		//tick and update
               	long getCurrentMilliseconds();
		void tick();
	  	void  Update();
  		void processBuffer(std::vector<std::string> stringVector);
	  	void processMove(std::vector<std::string> stringVector);
		void requestClient(std::vector<std::string> stringVector);
		void sendMovesToClients();

               	//client
                int getNextClientId();
                std::vector<Client*> mClientVector;
                int mClientIdCounter;

		//time
                long mGameStartTime;
                long mLastTime;
                long mDelta;
                long mTickCount;



		void  TogglePause(){m_bPaused = !m_bPaused;}
  		bool  Paused()const{return m_bPaused;}

  		int   cxClient()const{return m_cxClient;}
  		int   cyClient()const{return m_cyClient;}

 		bool  GoalKeeperHasBall()const{return m_bGoalKeeperHasBall;}
  		void  SetGoalKeeperHasBall(bool b){m_bGoalKeeperHasBall = b;}

  		const Region*const         PlayingArea()const{return m_pPlayingArea;}
  		const std::vector<Wall2D>& Walls(){return m_vecWalls;}                      
  		SoccerBall*const           Ball()const{return m_pBall;}

  		const Region* const GetRegionFromIndex(int idx)                                
  		{
    			assert ( (idx >= 0) && (idx < (int)m_Regions.size()) );

    			return m_Regions[idx];
  		}

  		bool  GameOn()const{return m_bGameOn;}
  		void  SetGameOn(){m_bGameOn = true;}
  		void  SetGameOff(){m_bGameOn = false;}

		void sendToClient(Client* client, std::string message);
		void sendDataToNewClients();

};

#endif
