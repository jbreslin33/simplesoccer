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
	printf("pitch con");
	mId = id;
	mServer = server;

  //define the playing area
  m_pPlayingArea = new Region(20, 20, cx-20, cy-20);

  //create the regions  
  CreateRegions(PlayingArea()->Width() / (double)NumRegionsHorizontal,
                PlayingArea()->Height() / (double)NumRegionsVertical);
/*

  //create the goals
   m_pRedGoal  = new Goal(Vector2D( m_pPlayingArea->Left(), (cy-Prm.GoalWidth)/2),
                          Vector2D(m_pPlayingArea->Left(), cy - (cy-Prm.GoalWidth)/2),
                          Vector2D(1,0));
   


  m_pBlueGoal = new Goal( Vector2D( m_pPlayingArea->Right(), (cy-Prm.GoalWidth)/2),
                          Vector2D(m_pPlayingArea->Right(), cy - (cy-Prm.GoalWidth)/2),
                          Vector2D(-1,0));


  //create the soccer ball
  m_pBall = new SoccerBall(Vector2D((double)m_cxClient/2.0, (double)m_cyClient/2.0),
                           Prm.BallSize,
                           Prm.BallMass,
                           m_vecWalls);

  
  //create the teams 
  m_pRedTeam  = new SoccerTeam(m_pRedGoal, m_pBlueGoal, this, SoccerTeam::red);
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
      m_Regions[idx--] = new Region(PlayingArea()->Left()+col*width,
                                   PlayingArea()->Top()+row*height,
                                   PlayingArea()->Left()+(col+1)*width,
                                   PlayingArea()->Top()+(row+1)*height,
                                   idx);
    }
  }
}








