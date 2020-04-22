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


class SoccerPitch
{ 
public:

	SoccerBall*          m_pBall;
  
	//params
  	double GoalWidth; 
    
    	int NumSupportSpotsX;    
    	int NumSupportSpotsY;   
    
    	double Spot_PassSafeScore;
    	double Spot_CanScoreFromPositionScore;
    	double Spot_DistFromControllingPlayerScore;
    	double Spot_ClosenessToSupportingPlayerScore;
    	double Spot_AheadOfAttackerScore;

    	double SupportSpotUpdateFreq; 
    
    	double ChancePlayerAttemptsPotShot;
    ChanceOfUsingArriveTypeReceiveBehavior = GetNextParameterDouble();
    
    BallSize                    = GetNextParameterDouble();    
    BallMass                    = GetNextParameterDouble();    
    Friction                    = GetNextParameterDouble(); 
    
    KeeperInBallRange           = GetNextParameterDouble();    
    PlayerInTargetRange         = GetNextParameterDouble(); 
    PlayerKickingDistance       = GetNextParameterDouble(); 
    PlayerKickFrequency         = GetNextParameterDouble();


    PlayerMass                  = GetNextParameterDouble(); 
    PlayerMaxForce              = GetNextParameterDouble();    
    PlayerMaxSpeedWithBall      = GetNextParameterDouble();   
    PlayerMaxSpeedWithoutBall   = GetNextParameterDouble();   
    PlayerMaxTurnRate           = GetNextParameterDouble();   
    PlayerScale                 = GetNextParameterDouble();      
    PlayerComfortZone           = GetNextParameterDouble();  
    PlayerKickingAccuracy       = GetNextParameterDouble();

    NumAttemptsToFindValidStrike = GetNextParameterInt();


    
    MaxDribbleForce             = GetNextParameterDouble();    
    MaxShootingForce            = GetNextParameterDouble();    
    MaxPassingForce             = GetNextParameterDouble();  
    
    WithinRangeOfHome           = GetNextParameterDouble();    
    WithinRangeOfSupportSpot    = GetNextParameterDouble();    
    
    MinPassDist                 = GetNextParameterDouble();
    GoalkeeperMinPassDist       = GetNextParameterDouble();
    
    GoalKeeperTendingDistance   = GetNextParameterDouble();    
    GoalKeeperInterceptRange    = GetNextParameterDouble();
    BallWithinReceivingRange    = GetNextParameterDouble();
    
    bStates                     = GetNextParameterBool();    
    bIDs                        = GetNextParameterBool(); 
    bSupportSpots               = GetNextParameterBool();     
    bRegions                    = GetNextParameterBool();
    bShowControllingTeam        = GetNextParameterBool();
    bViewTargets                = GetNextParameterBool();
    bHighlightIfThreatened      = GetNextParameterBool();

    FrameRate                   = GetNextParameterInt();

    SeparationCoefficient       = GetNextParameterDouble(); 
    ViewDistance                = GetNextParameterDouble(); 
    bNonPenetrationConstraint   = GetNextParameterBool(); 

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
  int                  m_cxClient,
                       m_cyClient;  
  
  //this instantiates the regions the players utilize to  position
  //themselves
  void CreateRegions(double width, double height);

  Server* mServer;


public:

  SoccerPitch(int cxClient, int cyClient, Server* server, int id);

  ~SoccerPitch();

  void  Update();
  void processBuffer(std::vector<std::string> stringVector);
  void processMove(std::vector<std::string> stringVector);



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

};

#endif
