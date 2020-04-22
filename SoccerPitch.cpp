#include "SoccerPitch.h"
#include "SoccerBall.h"
#include "Goal.h"
#include "Game/Region.h"
#include "2D/Transformations.h"
#include "2D/geometry.h"
#include "SoccerTeam.h"
#include "Game/EntityManager.h"
#include "ParamLoader.h"
#include "PlayerBase.h"
#include "TeamStates.h"
#include "misc/FrameCounter.h"
#include "server.h"


const int NumRegionsHorizontal = 6; 
const int NumRegionsVertical   = 3;

//------------------------------- ctor -----------------------------------
//------------------------------------------------------------------------
SoccerPitch::SoccerPitch(int cx, int cy, Server* server, int id):m_cxClient(cx),
                                         m_cyClient(cy),
                                         m_bPaused(false),
                                         m_bGoalKeeperHasBall(false),
                                         m_Regions(NumRegionsHorizontal*NumRegionsVertical),
                                         m_bGameOn(true)
{
	printf("SoccerPitch Constructor\n");

	//params
	GoalWidth = 100;
	NumSweetSpotsX                  13
NumSweetSpotsY                  6
Spot_CanPassScore                     2.0
Spot_CanScoreFromPositionScore        1.0
Spot_DistFromControllingPlayerScore    2.0
Spot_ClosenessToSupportingPlayerScore 0.0
Spot_AheadOfAttackerScore             0.0
SupportSpotUpdateFreq           1
ChancePlayerAttemptsPotShot     0.005
ChanceOfUsingArriveTypeReceiveBehavior  0.5
BallSize                        5.0
BallMass                        1.0
Friction                        -0.015
KeeperInBallRange               10.0
PlayerInTargetRange             10.0
PlayerKickingDistance           6.0
PlayerKickFrequency               8
PlayerMass                      3.0
PlayerMaxForce                  1.0
PlayerMaxSpeedWithBall          1.2
PlayerMaxSpeedWithoutBall       1.6
PlayerMaxTurnRate               0.4
PlayerScale                     1.0
PlayerComfortZone               60.0
PlayerKickingAccuracy           0.99
NumAttemptsToFindValidStrike    5
MaxDribbleForce                 1.5
MaxShootingForce                6.0
MaxPassingForce                 3.0
WithinRangeOfHome               15.0
WithinRangeOfSweetSpot          15.0
MinPassDistance                 120.0
GoalkeeperMinPassDistance       50.0
GoalKeeperTendingDistance       20.0
GoalKeeperInterceptRange              100.0
BallWithinReceivingRange        10.0
ViewStates                          1
ViewIDs                             1
ViewSupportSpots                    1
ViewRegions                         0
bShowControllingTeam                1
ViewTargets                         0
HighlightIfThreatened               0
FrameRate                           60;
SeparationCoefficient                10.0;
ViewDistance                        30.0;
bNonPenetrationConstraint           0

	
	
	mId = id;
	mServer = server;


  	//define the playing area
  	m_pPlayingArea = new Region(20, 20, cx-20, cy-20);

  	//create the regions  
	CreateRegions
	(
		PlayingArea()->Width() / (double)NumRegionsHorizontal,
		PlayingArea()->Height() / (double)NumRegionsVertical
	);

	//Goals
	m_pRedGoal  = new Goal
	(
		Vector2D
		( 
			m_pPlayingArea->Left(), cy - 100/2
		),
                Vector2D
		(
			m_pPlayingArea->Left(), cy - 100/2
		),
                Vector2D(1,0)
	);
   
  	m_pBlueGoal = new Goal
	( 
		Vector2D
		( 
			m_pPlayingArea->Right(), cy - 100/2
		),
                Vector2D
		(
			m_pPlayingArea->Right(), cy - 100/2
		),
                Vector2D
		(
			-1,0
		)
	);


	//create the soccer ball
  	m_pBall = new SoccerBall
	(
		Vector2D((double)m_cxClient/2.0, (double)m_cyClient/2.0),
                5.0,
                1.0,
                m_vecWalls
	);

  
	//create the teams 
  	//m_pRedTeam  = new SoccerTeam(m_pRedGoal, m_pBlueGoal, this, SoccerTeam::red);
  	m_pRedTeam  = new SoccerTeam(m_pRedGoal, m_pBlueGoal, this, SoccerTeam::team_color::red);
	/*
  	m_pBlueTeam = new SoccerTeam(m_pBlueGoal, m_pRedGoal, this, SoccerTeam::blue);

  	//make sure each team knows who their opponents are
  	m_pRedTeam->SetOpponents(m_pBlueTeam);
  	m_pBlueTeam->SetOpponents(m_pRedTeam); 

  //create the walls
  Vector2D TopLeft(m_pPlayingArea->Left(), m_pPlayingArea->Top());                                        
  Vector2D TopRight(m_pPlayingArea->Right(), m_pPlayingArea->Top());
  Vector2D BottomRight(m_pPlayingArea->Right(), m_pPlayingArea->Bottom());
  Vector2D BottomLeft(m_pPlayingArea->Left(), m_pPlayingArea->Bottom());
                                      
  m_vecWalls.push_back(Wall2D(BottomLeft, m_pRedGoal->RightPost()));
  m_vecWalls.push_back(Wall2D(m_pRedGoal->LeftPost(), TopLeft));
  m_vecWalls.push_back(Wall2D(TopLeft, TopRight));
  m_vecWalls.push_back(Wall2D(TopRight, m_pBlueGoal->LeftPost()));
  m_vecWalls.push_back(Wall2D(m_pBlueGoal->RightPost(), BottomRight));
  m_vecWalls.push_back(Wall2D(BottomRight, BottomLeft));

  ParamLoader* p = ParamLoader::Instance();
  */
}

//-------------------------------- dtor ----------------------------------
//------------------------------------------------------------------------
SoccerPitch::~SoccerPitch()
{
  delete m_pBall;

  delete m_pRedTeam;
  delete m_pBlueTeam;

  delete m_pRedGoal;
  delete m_pBlueGoal;

  delete m_pPlayingArea;

  for (unsigned int i=0; i<m_Regions.size(); ++i)
  {
    delete m_Regions[i];
  }
}



//----------------------------- Update -----------------------------------
//
//  this demo works on a fixed frame rate (60 by default) so we don't need
//  to pass a time_elapsed as a parameter to the game entities
//------------------------------------------------------------------------
void SoccerPitch::Update()
{
  if (m_bPaused) return;

  static int tick = 0;

  //update the balls
  m_pBall->Update();

  //update the teams
  m_pRedTeam->Update();
  m_pBlueTeam->Update();

  //if a goal has been detected reset the pitch ready for kickoff
  if (m_pBlueGoal->Scored(m_pBall) || m_pRedGoal->Scored(m_pBall))
  {
    m_bGameOn = false;
    
    //reset the ball                                                      
    m_pBall->PlaceAtPosition(Vector2D((double)m_cxClient/2.0, (double)m_cyClient/2.0));

    //get the teams ready for kickoff
    m_pRedTeam->GetFSM()->ChangeState(PrepareForKickOff::Instance());
    m_pBlueTeam->GetFSM()->ChangeState(PrepareForKickOff::Instance());
  }
}


void SoccerPitch::processBuffer(std::vector<std::string> stringVector)
{
/*
        if (stringVector.at(1).compare(0,1,"m") == 0)
        {
                processMove(stringVector);
        }

        if (stringVector.at(1).compare(0,1,"j") == 0)
        {
                requestClient(stringVector);
        }

        if (stringVector.at(1).compare(0,1,"p") == 0)
        {
                requestPlayer(stringVector);
        }

        if (stringVector.at(1).compare(0,1,"g") == 0)
        {
                startGame(stringVector);
        }
	*/
}

void SoccerPitch::processMove(std::vector<std::string> stringVector)
{
	/*
        int clientIdInt = atoi(stringVector.at(2).c_str());

        for (int c = 0; c < mClientVector.size(); c++)
        {
                if (mClientVector.at(c)->mId == clientIdInt)
                {
                        mClientVector.at(c)->mUp = atoi(stringVector.at(3).c_str());
                        mClientVector.at(c)->mRight = atoi(stringVector.at(4).c_str());
                        mClientVector.at(c)->mDown = atoi(stringVector.at(5).c_str());
                        mClientVector.at(c)->mLeft = atoi(stringVector.at(6).c_str());
                        mClientVector.at(c)->mRotateLeft = atoi(stringVector.at(7).c_str());
                        mClientVector.at(c)->mRotateRight = atoi(stringVector.at(8).c_str());
                }
        }
	*/
}



//------------------------- CreateRegions --------------------------------
void SoccerPitch::CreateRegions(double width, double height)
{  
	//index into the vector
  	int idx = m_Regions.size()-1;
    
  	for (int col=0; col<NumRegionsHorizontal; ++col)
  	{
    		for (int row=0; row<NumRegionsVertical; ++row)
    		{
      			m_Regions[idx--] = new Region
			(
				PlayingArea()->Left()+col*width,
                                PlayingArea()->Top()+row*height,
                                PlayingArea()->Left()+(col+1)*width,
                                PlayingArea()->Top()+(row+1)*height,
                                idx
			);
    		}
  	}
}








