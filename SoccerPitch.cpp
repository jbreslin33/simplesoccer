#include "SoccerPitch.h"
#include "SoccerBall.h"
#include "Goal.h"
#include "Game/Region.h"
#include "2D/geometry.h"
#include "SoccerTeam.h"
#include "Game/EntityManager.h"
#include "PlayerBase.h"
#include "TeamStates.h"
#include "misc/FrameCounter.h"
#include "server.h"
#include "client.h"
#include <sys/time.h>

//Berkeley
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <string.h>


//------------------------------- ctor -----------------------------------
//------------------------------------------------------------------------
SoccerPitch::SoccerPitch(int cx, int cy, Server* server, int id)
{

	NumRegionsHorizontal = 6; 
	NumRegionsVertical   = 3;
	m_cxClient = cx;
	m_cyClient = cy;
	m_bPaused = false;
	m_bGoalKeeperHasBall = false;
	m_Regions.resize(NumRegionsHorizontal * NumRegionsVertical);

	m_bGameOn = true;

	//params
	GoalWidth 				= 100;
	NumSupportSpotsX = 13;
        NumSupportSpotsY = 6;



	Spot_CanPassScore                     	= 2.0;
	Spot_CanScoreFromPositionScore        	= 1.0;
	Spot_DistFromControllingPlayerScore    	= 2.0;
	Spot_ClosenessToSupportingPlayerScore 	= 0.0;
	Spot_AheadOfAttackerScore             	= 0.0;
	SupportSpotUpdateFreq           	= 1;
	ChancePlayerAttemptsPotShot     	= 0.005;
	ChanceOfUsingArriveTypeReceiveBehavior  = 0.5;
	BallSize                        	= 5.0;
	BallMass                        	= 1.0;
	Friction                        	= -0.015;
	KeeperInBallRange               	= 10.0;
	PlayerInTargetRange             	= 10.0;
	PlayerKickingDistance           	= 6.0;
	PlayerKickFrequency               	= 8;
	PlayerMass                      	= 3.0;
	PlayerMaxForce                  	= 1.0;
	PlayerMaxSpeedWithBall          	= 1.2;
	PlayerMaxSpeedWithoutBall       	= 1.6;
	PlayerMaxTurnRate               	= 0.4;
	PlayerScale                     	= 1.0;
	PlayerComfortZone               	= 60.0;
	PlayerKickingAccuracy           	= 0.99;
	NumAttemptsToFindValidStrike    	= 5;
	MaxDribbleForce                 	= 1.5;
	MaxShootingForce                	= 6.0;
	MaxPassingForce                 	= 3.0;
	WithinRangeOfHome               	= 15.0;
	WithinRangeOfSweetSpot          	= 15.0;
	MinPassDistance                 	= 120.0;
	GoalkeeperMinPassDistance       	= 50.0;
	GoalKeeperTendingDistance       	= 20.0;
	GoalKeeperInterceptRange                = 100.0;
	BallWithinReceivingRange        	= 10.0;
	ViewStates                          	= 1;
	ViewIDs                             	= 1;
	ViewSupportSpots                    	= 1;
	ViewRegions                         	= 0;
	bShowControllingTeam                	= 1;
	ViewTargets                         	= 0;
	HighlightIfThreatened               	= 0;
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
	
	mId = id;
	mServer = server;

	//time
        mGameStartTime = getCurrentMilliseconds();
        mLastTime = mGameStartTime;
        mDelta = 0;
        mTickCount = 0;

	//clients
        mClientIdCounter = 0;

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
			m_pPlayingArea->Left(), cy - GoalWidth/2 //550
		),
                Vector2D
		(
			m_pPlayingArea->Left(), cy - (cy - GoalWidth/2) //600 - (600 - 50) = 50
		),
                Vector2D(1,0)
	);
   
  	m_pBlueGoal = new Goal
	( 
		Vector2D
		( 
			m_pPlayingArea->Right(), cy - GoalWidth/2
		),
                Vector2D
		(
			m_pPlayingArea->Right(), cy - (cy - GoalWidth/2)
		),
                Vector2D
		(
			-1,0
		)
	);


	//create the soccer ball
  	m_pBall = new SoccerBall
	(
	 	this,
		Vector2D((double)m_cxClient/2.0, (double)m_cyClient/2.0),
                BallSize,
                BallMass,
                m_vecWalls
	);

	//create the teams 
  	//m_pRedTeam  = new SoccerTeam(m_pRedGoal, m_pBlueGoal, this, SoccerTeam::red);
  	m_pRedTeam  = new SoccerTeam(m_pRedGoal, m_pBlueGoal, this, SoccerTeam::team_color::red);
  	m_pBlueTeam = new SoccerTeam(m_pBlueGoal, m_pRedGoal, this, SoccerTeam::team_color::blue);


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

	/*

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

int SoccerPitch::getNextClientId()
{
        mClientIdCounter++;
        return mClientIdCounter;
}




//----------------------------- tick -----------------------------------
//the following time has been chaneged to what buckland does... 
//  this demo works on a fixed frame rate (60 by default) so we don't need
//  to pass a time_elapsed as a parameter to the game entities
//------------------------------------------------------------------------
void SoccerPitch::tick()
{

	if (m_bPaused) 
	{
		return;
	}


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

        //send moves to clients
       	sendMovesToClients();

        //any new clients then send them message with the port
        sendDataToNewClients();

}

long SoccerPitch::getCurrentMilliseconds()
{
        struct timeval tp;
        gettimeofday(&tp, NULL);
        long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
        return ms;
}

void SoccerPitch::Update()
{
        mDelta = getCurrentMilliseconds() - mLastTime;

        if (mDelta > FrameRate)
        {
                tick();
                mLastTime = getCurrentMilliseconds();
        }
}

void SoccerPitch::processBuffer(std::vector<std::string> stringVector)
{
	/*
        if (stringVector.at(1).compare(0,1,"m") == 0)
        {
                processMove(stringVector);
        }
       */ 
	if (stringVector.at(1).compare(0,1,"j") == 0)
        {
                requestClient(stringVector);
        }
/*
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

void SoccerPitch::requestClient(std::vector<std::string> stringVector)
{
        int personIdInt = atoi(stringVector.at(2).c_str());
        int portInt = atoi(stringVector.at(3).c_str());

        // if this person requesting already has a client instance on server than rock on and just update the port if needed
        for (int c = 0; c < mClientVector.size(); c++)
        {
                if (mClientVector.at(c)->mPersonId == personIdInt)
                {
                        if (mClientVector.at(c)->mPort == portInt)
                        {
                                printf("Person ID:%d already has a client and port is good to go at:%d.\n",personIdInt, portInt);
                        }
                        else
                        {
                                mClientVector.at(c)->mPort = portInt;
                                mClientVector.at(c)->mSentToClient = false;
                                printf("Person ID:%d already has a client. Updating port to: %d\n and sending confirmation to client again.\n",personIdInt, portInt);

                        }
                        return;
                }
        }

        //fall thru make a new client instance
        Client* client = new Client(this,getNextClientId(),0,0);
        mClientVector.push_back(client);
        client->mPersonId = personIdInt;
        printf("Make new Client for Person ID:%d \n",personIdInt);
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
//Game sending this message to clients: 1,m,
//2,-nan,-nan,0,
//
//3,49.383342,68.147893,0,
//4,723.016325,68.887522,0,
//5,326.820060,486.674330,0,
//6,210.043380,290.842605,0,
//
//8,775.271641,244.005153,0,
//9,407.223145,486.219261,0,
//10,407.927494,101.106716,0,
//11,580.469619,486.644198,0,
//12,742.387647,79.174584,0,
//
//734.228040,75.278572,
void SoccerPitch::sendMovesToClients()
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
			
                        
			for (int p = 0; p < m_pRedTeam->Members().size(); p++)
                        {

                                std::string id = std::to_string(m_pRedTeam->Members().at(p)->ID()); //player id
                                std::string x  = std::to_string(m_pRedTeam->Members().at(p)->Pos().x); //player x
                                std::string y  = std::to_string(m_pRedTeam->Members().at(p)->Pos().y); //player y

                                double rotDegrees = atan2(m_pRedTeam->Members().at(p)->Heading().y,m_pRedTeam->Members().at(p)->Heading().x) * 180 / 3.14;
                                std::string headingAngle = std::to_string(rotDegrees); //left foot angle

                                message.append(id);
                                message.append(",");
                                message.append(x);
                                message.append(",");
                                message.append(y);
                                message.append(",");
                                message.append(headingAngle);
                                message.append(",");
				message.append(m_pRedTeam->Members().at(p)->mStateName);
                                message.append(",");
                        }


                        for (int p = 0; p < m_pBlueTeam->Members().size(); p++)
                        {

                                std::string id = std::to_string(m_pBlueTeam->Members().at(p)->ID()); //player id
                                std::string x  = std::to_string(m_pBlueTeam->Members().at(p)->Pos().x); //player x
                                std::string y  = std::to_string(m_pBlueTeam->Members().at(p)->Pos().y); //player y
				
				double rotDegrees = atan2(m_pBlueTeam->Members().at(p)->Heading().y,m_pBlueTeam->Members().at(p)->Heading().x) * 180 / 3.14;
                                std::string headingAngle = std::to_string(rotDegrees); //left foot angle

                                message.append(id);
                                message.append(",");
                                message.append(x);
                                message.append(",");
                                message.append(y);
                                message.append(",");
                                message.append(headingAngle);
                                message.append(",");
				message.append(m_pBlueTeam->Members().at(p)->mStateName);
                                message.append(",");
                        }

                        //add ball
                        std::string x  = std::to_string(Ball()->Pos().x); //ball x
                        std::string y  = std::to_string(Ball()->Pos().y); //ball y
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

void SoccerPitch::sendDataToNewClients()
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

                        printf("TO NEW CLIENT:%s AND mSentToClient set true.\n",message.c_str());
                        sendToClient(mClientVector.at(c),message);

                        mClientVector.at(c)->mSentToClient = true;
                }
        }
}



void SoccerPitch::sendToClient(Client* client, std::string message)
{
        int sock;
        struct sockaddr_in sa;
        int bytes_sent;
        char buffer[1000];

        strcpy(buffer, message.c_str());

        /* create an Internet, datagram, socket using UDP */
        sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == -1)
        {
                /* if socket failed to initialize, exit */
                printf("Error Creating Socket");
                exit(EXIT_FAILURE);
        }

        /* Zero out socket address */
        memset(&sa, 0, sizeof sa);

        /* The address is IPv4 */
        sa.sin_family = AF_INET;

        /* IPv4 adresses is a uint32_t, convert a string representation of the octets to the appropriate value */
        sa.sin_addr.s_addr = inet_addr("127.0.0.1");

        /* sockets are unsigned shorts, htons(x) ensures x is in network byte order, set the port to 7654 */
        sa.sin_port = htons(client->mPort);

        bytes_sent = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&sa, sizeof sa);
        if (bytes_sent < 0)
        {
                printf("Error sending packet: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }

        close(sock); /* close the socket */
	
}






