//custome
#include "footballGame.h"
#include "SoccerBall.h"
#include "Goal.h"
#include "2D/geometry.h"
#include "SoccerTeam.h"
#include "Game/EntityManager.h"
#include "PlayerBase.h"
#include "TeamStates.h"
#include "misc/FrameCounter.h"
#include "server.h"
#include "client.h"
#include "utility.h"
#include "footballPitch.h"

//standard
#include <sys/time.h>

#include <string.h>


//------------------------------- ctor -----------------------------------
//------------------------------------------------------------------------
FootballGame::FootballGame(int screenX, int screenY, Server* server, int id) : Game(screenX, screenY, server, id)
{
	//make the pitch
	mFootballPitch = new FootballPitch(this);
	
	mKeeperHasBall = false;

	ChancePlayerAttemptsPotShot     	= 0.005;
	ChanceOfUsingArriveTypeReceiveBehavior  = 0.5;
	BallSize                        	= 5.0;
	BallMass                        	= 1.0;
	Friction                        	= -0.015;
	KeeperInBallRange               	= 10.0;
	PlayerInTargetRange             	= 10.0;
	PlayerKickingDistance           	= 6.0;
	PlayerKickFrequency               	= 8;

	PlayerComfortZone               	= 60.0;
	PlayerKickingAccuracy           	= 0.99;
	NumAttemptsToFindValidStrike    	= 5;
	MaxDribbleForce                 	= 1.5;
	MaxShootingForce                	= 6.0;
	MaxPassingForce                 	= 3.0;
	WithinRangeOfHome               	= 15.0;
	GoalKeeperTendingDistance       	= 20.0;
	GoalKeeperInterceptRange                = 100.0;
	BallWithinReceivingRange        	= 10.0;
	bShowControllingTeam                	= 1;
	FrameRate                           	= 30;
	SeparationCoefficient                	= 10.0;
	ViewDistance                        	= 30.0;
	bNonPenetrationConstraint           	= 0;

	//squared space
	BallWithinReceivingRangeSq = BallWithinReceivingRange * BallWithinReceivingRange;
	KeeperInBallRangeSq      = KeeperInBallRange * KeeperInBallRange;
	PlayerInTargetRangeSq    = PlayerInTargetRange * PlayerInTargetRange;   
	PlayerKickingDistanceSq  = PlayerKickingDistance * PlayerKickingDistance;
	PlayerComfortZoneSq      = PlayerComfortZone * PlayerComfortZone;
	GoalKeeperInterceptRangeSq     = GoalKeeperInterceptRange * GoalKeeperInterceptRange;
	WithinRangeOfSupportSpotSq = WithinRangeOfSupportSpot * WithinRangeOfSupportSpot;
	
	mBall = new SoccerBall
        (
        	0, Vector2D( (double) this->mScreenX / 2.0, (double) this->mScreenY / 2.0), 5.0, //BaseGameEntity
                Vector2D(0.0,0.0), Vector2D(0,1), 1, 0, 0, 0,        //MovingEntity
		this
        );

	//create the teams 
  	mTeamVector.push_back(new SoccerTeam(mFootballPitch->m_pBlueGoal, mFootballPitch->m_pRedGoal, this, SoccerTeam::team_color::blue));
  	mTeamVector.push_back(new SoccerTeam(mFootballPitch->m_pRedGoal, mFootballPitch->m_pBlueGoal, this, SoccerTeam::team_color::red));

  	//make sure each team knows who their opponents are
  	mTeamVector.at(0)->SetOpponents(mTeamVector.at(1));
  	mTeamVector.at(1)->SetOpponents(mTeamVector.at(0));
}

//-------------------------------- dtor ----------------------------------
//------------------------------------------------------------------------
FootballGame::~FootballGame()
{
	delete mBall;

	for (int i = 0; i < mTeamVector.size(); i++)
	{
		delete mTeamVector.at(i);
	}
}

SoccerBall* const FootballGame::getBall()const
{
	return mBall;
}

void FootballGame::startGame(std::vector<std::string> stringVector)
{
        //no need to parse string
        setGameOn(true);
        //call parent... instead of above...

	printf("startGame\n");
        //get the teams ready for kickoff
	for (int i = 0; i < mTeamVector.size(); i++)
	{
        	mTeamVector.at(i)->GetFSM()->ChangeState(PrepareForKickOff::Instance());
	}
}

//----------------------------- tick -----------------------------------
//the following time has been chaneged to what buckland does... 
//  this demo works on a fixed frame rate (60 by default) so we don't need
//  to pass a time_elapsed as a parameter to the game entities
//------------------------------------------------------------------------
void FootballGame::tick()
{
        //any new clients then send them message with the port
        sendDataToNewClients();

	if (mPaused == true) 
	{
		return;
	}

  	static int tick = 0;

  	//update the balls
  	mBall->Update();

  	//update the teams
	for (int i = 0; i < mTeamVector.size(); i++)
	{
		mTeamVector.at(i)->Update();
	}
	

  	//if a goal has been detected reset the game ready for kickoff
  	if (mFootballPitch->m_pBlueGoal->Scored(mBall) || mFootballPitch->m_pRedGoal->Scored(mBall))
 	{
    		setGameOn(false);
    
    		//reset the ball                                                      
    		mBall->PlaceAtPosition(Vector2D( (double) mScreenX / 2.0, (double) mScreenY / 2.0));

    		//get the teams ready for kickoff
		for (int i = 0; i < mTeamVector.size(); i++)
		{
    			mTeamVector.at(i)->GetFSM()->ChangeState(PrepareForKickOff::Instance());
		}
  	}

        //send moves to clients
       	sendMovesToClients();


}

void FootballGame::processMove(std::vector<std::string> stringVector)
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

void FootballGame::sendMovesToClients()
{
        for (int c = 0; c < mClientVector.size(); c++)
        {
                //only clients with ports
                if (mClientVector.at(c)->mPort != 0 && mClientVector.at(c)->mSentToClient == true)
                {
                        //we could just send 5 a pop with no id??? that would be 20...
                        std::string message = "";
                        message.append(std::to_string(mId)); //game id
                        message.append(",");
                        message.append("m"); //move code
                        message.append(",");
			
			for (int t = 0; t < mTeamVector.size(); t++)
			{
				for (int p = 0; p < mTeamVector.at(t)->Members().size(); p++)
                        	{

                                	std::string id = std::to_string(mTeamVector.at(t)->Members().at(p)->ID()); //player id
                                	std::string x  = std::to_string(mTeamVector.at(t)->Members().at(p)->Pos().x); //player x
                                	std::string y  = std::to_string(mTeamVector.at(t)->Members().at(p)->Pos().y); //player y

					double rotDegrees = mUtility->getRotationInDegreesFromVector(mTeamVector.at(t)->Members().at(p)->Heading());
                                	std::string headingAngle = std::to_string(rotDegrees); //left foot angle

                                	message.append(id);
                                	message.append(",");
                                	message.append(x);
                                	message.append(",");
                                	message.append(y);
                                	message.append(",");
                                	message.append(headingAngle);
                                	message.append(",");
					message.append(mTeamVector.at(t)->Members().at(p)->mStateName);
                                	message.append(",");
				}
                        }

                        //add ball
                        std::string x  = std::to_string(getBall()->Pos().x); //ball x
                        std::string y  = std::to_string(getBall()->Pos().y); //ball y
                        message.append(x);
                        message.append(",");
                        message.append(y);
                        message.append(",");

                        if (c == 0)
                        {
                                //printf("Game sending this message to clients: %s\n",message.c_str()); //print to console what we are about to send
                        }

                        sendToClient(mClientVector.at(c),message);

                }
        }
}


void FootballGame::sendDataToNewClients()
{
        for (int c = 0; c < mClientVector.size(); c++)
        {
                if (mClientVector.at(c)->mPort != 0 && mClientVector.at(c)->mSentToClient == false)
                {
                        //then we need to send to client so lets construct a messsage
                        std::string message = "";

                        message.append(std::to_string(mId)); //gameID
                        message.append(",");

                        message.append("j"); //new client code
                        message.append(",");

                        message.append(std::to_string(mClientVector.at(c)->mId)); //client id
                        message.append(",");     //extra comma

                        //top left x
                        message.append( std::to_string( mFootballPitch->m_pPlayingArea->Left()  ));
                        message.append(",");     //extra comma

                        //top left y
                        message.append( std::to_string( mFootballPitch->m_pPlayingArea->Top()  ));
                        message.append(",");     //extra comma

                        //bottom right x
                        message.append( std::to_string( mFootballPitch->m_pPlayingArea->Right()  ));
                        message.append(",");     //extra comma

                        //bottom right y
                        message.append( std::to_string( mFootballPitch->m_pPlayingArea->Bottom()  ));
                        message.append(",");     //extra comma

                        //ballsize radius
                        message.append( std::to_string( BallSize  ));
                        message.append(",");     //extra comma
			
			for (int t = 0; t < mTeamVector.size(); t++)
			{

                        	for (int p = 0; p < mTeamVector.at(t)->Members().size(); p++)
                        	{
                                	message.append( std::to_string( mTeamVector.at(t)->Members().at(p)->ID() ));
                                	message.append(",");     //extra comma

                                	//size radius
                                	message.append( std::to_string( mTeamVector.at(t)->Members().at(p)->BRadius() ));
                                	message.append(",");     //extra comma


                                	if (p < (mTeamVector.at(t)->Members().size() - 1))
                                	{
						if (t == 0)
						{
                                        		message.append("blue");
                                        		message.append(",");     //extra comma
						}
						if (t == 1)
						{
                                        		message.append("red");
                                        		message.append(",");     //extra comma
						}
                                	}
                                	else
                                	{
						if (t == 0)
						{
                                        		message.append("violet");
                                        		message.append(",");     //extra comma
						}
						if (t == 1)
						{
                                        		message.append("purple");
                                        		message.append(",");     //extra comma
						}
                                	}
				}
                        }

                        printf("TO NEW CLIENT:%s AND mSentToClient set true.\n",message.c_str());
                        sendToClient(mClientVector.at(c),message);

                        mClientVector.at(c)->mSentToClient = true;
                }
        }
}







