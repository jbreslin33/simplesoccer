#include "SoccerTeam.h"
#include "footballGame.h"
#include "goal.h"
#include "PlayerBase.h"
#include "Goalkeeper.h"
#include "FieldPlayer.h"
#include "misc/utils.h"
#include "SteeringBehaviors.h"
#include "GoalKeeperStates.h"
#include "2D/geometry.h"
#include "Game/EntityManager.h"
#include "Messaging/MessageDispatcher.h"
#include "SoccerMessages.h"
#include "TeamStates.h"
#include "footballPitch.h"

using std::vector;


//----------------------------- ctor -------------------------------------
//
//------------------------------------------------------------------------
SoccerTeam::SoccerTeam(Goal*        home_goal,
                       Goal*        opponents_goal,
                       FootballGame* game,
                       team_color   color):m_pOpponentsGoal(opponents_goal),
                                           m_pHomeGoal(home_goal),
                                           m_pOpponents(NULL),
                                           m_pGame(game),
                                           m_Color(color),
                                           m_dDistSqToBallOfClosestPlayer(0.0),
                                           m_pSupportingPlayer(NULL),
                                           m_pReceivingPlayer(NULL),
                                           m_pControllingPlayer(NULL),
                                           m_pPlayerClosestToBall(NULL)
{
 	NumAttemptsToFindValidStrike            = 5;
        MaxPassingForce                         = 3.0;
        MaxShootingForce                        = 6.0;
  
	//setup the state machine
	m_pStateMachine = new StateMachine<SoccerTeam>(this);

  	m_pStateMachine->SetCurrentState(Defending::Instance());
  	m_pStateMachine->SetPreviousState(Defending::Instance());
  	m_pStateMachine->SetGlobalState(NULL);

  	//create the players and goalkeeper
  	CreatePlayers();
  
  	//set default steering behaviors
  	std::vector<PlayerBase*>::iterator it = m_Players.begin();

  	for (it; it != m_Players.end(); ++it)
  	{
    		(*it)->Steering()->SeparationOn();   
  	}


  	//create the sweet spot calculator
  	m_pSupportSpotCalc = new SupportSpotCalculator(Game()->mFootballPitch->mNumSupportSpotsX,
                                                 Game()->mFootballPitch->mNumSupportSpotsY,
                                                 this);
}

//----------------------- dtor -------------------------------------------
//
//------------------------------------------------------------------------
SoccerTeam::~SoccerTeam()
{
  delete m_pStateMachine;

  std::vector<PlayerBase*>::iterator it = m_Players.begin();
  for (it; it != m_Players.end(); ++it)
  {
    delete *it;
  }

  delete m_pSupportSpotCalc;
}

//-------------------------- update --------------------------------------
//
//  iterates through each player's update function and calculates 
//  frequently accessed info
//------------------------------------------------------------------------
void SoccerTeam::Update()
{
  	//this information is used frequently so it's more efficient to 
  	//calculate it just once each frame
  	CalculateClosestPlayerToBall();

  	//the team state machine switches between attack/defense behavior. It
  	//also handles the 'kick off' state where a team must return to their
  	//kick off positions before the whistle is blown
  	m_pStateMachine->Update();
  
  	//now update each player
  	std::vector<PlayerBase*>::iterator it = m_Players.begin();

  	for (it; it != m_Players.end(); ++it)
  	{
    		(*it)->Update();
  	}
}

//------------------------ CalculateClosestPlayerToBall ------------------
//
//  sets m_iClosestPlayerToBall to the player closest to the ball
//------------------------------------------------------------------------
void SoccerTeam::CalculateClosestPlayerToBall()
{
  double ClosestSoFar = MaxFloat;

  std::vector<PlayerBase*>::iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {
    //calculate the dist. Use the squared value to avoid sqrt
    double dist = Vec2DDistanceSq((*it)->Pos(), Game()->getBall()->Pos());

    //keep a record of this value for each player
    (*it)->SetDistSqToBall(dist);
    
    if (dist < ClosestSoFar)
    {
      ClosestSoFar = dist;

      m_pPlayerClosestToBall = *it;
    }
  }

  m_dDistSqToBallOfClosestPlayer = ClosestSoFar;
}


//------------- DetermineBestSupportingAttacker ------------------------
//
// calculate the closest player to the SupportSpot
//------------------------------------------------------------------------
PlayerBase* SoccerTeam::DetermineBestSupportingAttacker()
{
  
	double ClosestSoFar = MaxFloat;

  	PlayerBase* BestPlayer = NULL;

  	std::vector<PlayerBase*>::iterator it = m_Players.begin();

  	for (it; it != m_Players.end(); ++it)
  	{
    		//only attackers utilize the BestSupportingSpot
    		if ( ((*it)->Role() == PlayerBase::attacker) && ((*it) != m_pControllingPlayer) )
    		{
      			//calculate the dist. Use the squared value to avoid sqrt
			//test
			Vector2D bestSupportingSpot = m_pSupportSpotCalc->GetBestSupportingSpot();
			
			
			//end test
      			double dist = Vec2DDistanceSq((*it)->Pos(), m_pSupportSpotCalc->GetBestSupportingSpot());
    
      			//if the distance is the closest so far and the player is not a
      			//goalkeeper and the player is not the one currently controlling
      			//the ball, keep a record of this player
      			if ((dist < ClosestSoFar) )
      			{
        			ClosestSoFar = dist;
        			BestPlayer = (*it);
      			}
    		}
  	}

  	return BestPlayer;
}

//-------------------------- FindPass ------------------------------
//
//  The best pass is considered to be the pass that cannot be intercepted 
//  by an opponent and that is as far forward of the receiver as possible
//------------------------------------------------------------------------
bool SoccerTeam::FindPass(const PlayerBase*const passer,
                         PlayerBase*&           receiver,
                         Vector2D&              PassTarget,
                         double                  power,
                         double                  MinPassingDistance)const
{  
	std::vector<PlayerBase*>::const_iterator curPlyr = Members().begin();

  	double    ClosestToGoalSoFar = MaxFloat;
  	Vector2D Target;

  	//iterate through all this player's team members and calculate which
  	//one is in a position to be passed the ball 
  	for (curPlyr; curPlyr != Members().end(); ++curPlyr)
  	{   
    		//make sure the potential receiver being examined is not this player
    		//and that it is further away than the minimum pass distance
    		if ( (*curPlyr != passer) &&            
        		(Vec2DDistanceSq(passer->Pos(), (*curPlyr)->Pos()) > 
         		MinPassingDistance*MinPassingDistance))                  
    		{           
      			if (GetBestPassToReceiver(passer, *curPlyr, Target, power))
      			{
        			//if the pass target is the closest to the opponent's goal line found
        			// so far, keep a record of it
        			double Dist2Goal = fabs(Target.x - OpponentsGoal()->getCenter().x);

        			if (Dist2Goal < ClosestToGoalSoFar)
        			{
          				ClosestToGoalSoFar = Dist2Goal;
          
          				//keep a record of this player
          				receiver = *curPlyr;

          				//and the target
          				PassTarget = Target;
        			}     
      			}
    		}
  	}//next team member

  	if (receiver) 
	{
		return true;
	}
  	else 
	{
		return false;
	}
}


//---------------------- GetBestPassToReceiver ---------------------------
//
//  Three potential passes are calculated. One directly toward the receiver's
//  current position and two that are the tangents from the ball position
//  to the circle of radius 'range' from the receiver.
//  These passes are then tested to see if they can be intercepted by an
//  opponent and to make sure they terminate within the playing area. If
//  all the passes are invalidated the function returns false. Otherwise
//  the function returns the pass that takes the ball closest to the 
//  opponent's goal area.
//------------------------------------------------------------------------
bool SoccerTeam::GetBestPassToReceiver(const PlayerBase* const passer,
                                       const PlayerBase* const receiver,
                                       Vector2D&               PassTarget,
                                       double                   power)const
{  
	//first, calculate how much time it will take for the ball to reach 
  	//this receiver, if the receiver was to remain motionless 
  	double time = Game()->getBall()->TimeToCoverDistance(Game()->getBall()->Pos(),
                                                    receiver->Pos(),
                                                    power);

  	//return false if ball cannot reach the receiver after having been
  	//kicked with the given power
  	if (time < 0) return false;

  	//the maximum distance the receiver can cover in this time
  	double InterceptRange = time * receiver->MaxSpeed();
  
  	//Scale the intercept range
  	const double ScalingFactor = 0.3;
  	InterceptRange *= ScalingFactor;

  	//now calculate the pass targets which are positioned at the intercepts
  	//of the tangents from the ball to the receiver's range circle.
  	Vector2D ip1, ip2;

  	GetTangentPoints(receiver->Pos(),
                   InterceptRange,
                   Game()->getBall()->Pos(),
                   ip1,
                   ip2);
 
  	const int NumPassesToTry = 3;
  	Vector2D Passes[NumPassesToTry] = {ip1, receiver->Pos(), ip2};
  
  
  	// this pass is the best found so far if it is:
  	//
  	//  1. Further upfield than the closest valid pass for this receiver
  	//     found so far
  	//  2. Within the playing area
  	//  3. Cannot be intercepted by any opponents

  	double ClosestSoFar = MaxFloat;
  	bool  bResult      = false;

  	for (int pass=0; pass<NumPassesToTry; ++pass)
  	{   	 
    		double dist = fabs(Passes[pass].x - OpponentsGoal()->getCenter().x);

    		if (( dist < ClosestSoFar) &&
        		Game()->mFootballPitch->getPlayingArea()->getIsInside(Passes[pass], 1) &&
        		isPassSafeFromAllOpponents(Game()->getBall()->Pos(),
                                   Passes[pass],
                                   receiver,
                                   power))
        
    		{
			//not getting here...
      			ClosestSoFar = dist;
      			PassTarget   = Passes[pass];
      			bResult      = true;
    		}
  	}

  	return bResult;
}

//----------------------- isPassSafeFromOpponent -------------------------
//
//  test if a pass from 'from' to 'to' can be intercepted by an opposing
//  player
//------------------------------------------------------------------------
bool SoccerTeam::isPassSafeFromOpponent(Vector2D    from,
                                        Vector2D    target,
                                        const PlayerBase* const receiver,
                                        const PlayerBase* const opp,
                                        double       PassingForce)const
{
	//move the opponent into local space.
  	Vector2D ToTarget = target - from;
  	Vector2D ToTargetNormalized = Vec2DNormalize(ToTarget);

	Vector2D v = ToTargetNormalized.Perp();
	//needs fixing as i hacked it
  	//Vector2D LocalPosOpp;
  	Vector2D LocalPosOpp = PointToLocalSpace(opp->Pos(),
                                        ToTargetNormalized,
                                         v,
                                         from);
  	//if opponent is behind the kicker then pass is considered okay(this is 
  	//based on the assumption that the ball is going to be kicked with a 
  	//velocity greater than the opponent's max velocity)
  	if ( LocalPosOpp.x < 0 )
  	{     
    		return true;
  	}
  
  	//if the opponent is further away than the target we need to consider if
  	//the opponent can reach the position before the receiver.
  	if (Vec2DDistanceSq(from, target) < Vec2DDistanceSq(opp->Pos(), from))
  	{
    		if (receiver)
    		{
      			if ( Vec2DDistanceSq(target, opp->Pos())  > 
           		Vec2DDistanceSq(target, receiver->Pos()) )
      			{
        			return true;
      			}
      			else
      			{
        			return false;
      			}
    		}
    		else
    		{
      			return true;
    		} 
  	}
  
  	//calculate how long it takes the ball to cover the distance to the 
  	//position orthogonal to the opponents position
  	double TimeForBall = 
  	Game()->getBall()->TimeToCoverDistance(Vector2D(0,0),
                                       Vector2D(LocalPosOpp.x, 0),
                                       PassingForce);

  	//now calculate how far the opponent can run in this time
  	double reach = opp->MaxSpeed() * TimeForBall +
                Game()->getBall()->getBoundingRadius()+
                opp->getBoundingRadius();

  	//if the distance to the opponent's y position is less than his running
  	//range plus the radius of the ball and the opponents radius then the
  	//ball can be intercepted
  	if ( fabs(LocalPosOpp.y) < reach )
  	{
    		return false;
  	}

  	return true;
}

//---------------------- isPassSafeFromAllOpponents ----------------------
//
//  tests a pass from position 'from' to position 'target' against each member
//  of the opposing team. Returns true if the pass can be made without
//  getting intercepted
//------------------------------------------------------------------------
bool SoccerTeam::isPassSafeFromAllOpponents(Vector2D                from,
                                            Vector2D                target,
                                            const PlayerBase* const receiver,
                                            double     PassingForce)const
{
	std::vector<PlayerBase*>::const_iterator opp = Opponents()->Members().begin();

  	for (opp; opp != Opponents()->Members().end(); ++opp)
  	{
    		if (!isPassSafeFromOpponent(from, target, receiver, *opp, PassingForce))
    		{
      			return false;
    		}
  	}

  	return true;
}

//------------------------ CanShoot --------------------------------------
//
//  Given a ball position, a kicking power and a reference to a vector2D
//  this function will sample random positions along the opponent's goal-
//  mouth and check to see if a goal can be scored if the ball was to be
//  kicked in that direction with the given power. If a possible shot is 
//  found, the function will immediately return true, with the target 
//  position stored in the vector ShotTarget.
//------------------------------------------------------------------------
bool SoccerTeam::CanShoot(Vector2D  BallPos, double power)
{
	//the number of randomly created shot targets this method will test 
  	int NumAttempts = NumAttemptsToFindValidStrike;

  	while (NumAttempts--)
  	{
    		//choose a random position along the opponent's goal mouth. (making
    		//sure the ball's radius is taken into account)

      		//you messed with this...	  
      		Vector2D* ShotTarget = new Vector2D();
		//Vector2D shotTarget = OpponentsGoal()->Center();
    		ShotTarget->x = OpponentsGoal()->getCenter().x;
    		ShotTarget->y = OpponentsGoal()->getCenter().y;


    		//the y value of the shot position should lay somewhere between two
    		//goalposts (taking into consideration the ball diameter)
    		int MinYVal = (int)(OpponentsGoal()->getRightPost().y - Game()->getBall()->getBoundingRadius());
    		int MaxYVal = (int)(OpponentsGoal()->getLeftPost().y + Game()->getBall()->getBoundingRadius());

    		ShotTarget->y = (double)RandInt(MinYVal, MaxYVal);

    		//make sure striking the ball with the given power is enough to drive
    		//the ball over the goal line.
		Vector2D shotTarget;
		shotTarget.x = ShotTarget->x;
		shotTarget.y = ShotTarget->y;
    		double time = Game()->getBall()->TimeToCoverDistance(BallPos,
                                                      shotTarget,
                                                      power);
    
    		//if it is, this shot is then tested to see if any of the opponents
    		//can intercept it.
    		if (time >= 0)
    		{
      			if (isPassSafeFromAllOpponents(BallPos, shotTarget, NULL, power))
      			{
        			return true;
      			}
    		}
  	}
  	return false;
}

 
//--------------------- ReturnAllFieldPlayersToHome ---------------------------
//
//  sends a message to all players to return to their home areas forthwith
//------------------------------------------------------------------------
void SoccerTeam::ReturnAllFieldPlayersToHome()const
{
  std::vector<PlayerBase*>::const_iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {
    if ((*it)->Role() != PlayerBase::goal_keeper)
    {
      Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
                            1,
                            (*it)->ID(),
                            Msg_GoHome,
                            NULL);
    }
  }
}


//------------------------- CreatePlayers --------------------------------
//
//  creates the players
//------------------------------------------------------------------------
void SoccerTeam::CreatePlayers()
{

	//vars
        double PlayerMass                              = 3.0;
        double PlayerMaxForce                          = 1.0;
        double PlayerMaxSpeedWithBall                  = 1.2;
        double PlayerMaxSpeedWithoutBall               = 1.6;
        double PlayerMaxTurnRate                       = 0.4;


/*
        int id, Vector2D position, Vector2D scale, double boundingRadius, //BaseGameEntity
        Vector2D velocity, Vector2D heading, double mass, double maxSpeed, double maxForce, double maxTurnRate         //MovingEntity
        SoccerTeam* soccerTeam, int homeRegion, double maxSpeedWithBall, player_role role, //PlayerBase 
        State<FieldPlayer>* startState //FieldPlayer
	*/


	if (Color() == blue)
  	{
 
		//create the players
    		m_Players.push_back
		(
			new FieldPlayer
			(
        			0, this->Game()->mFootballPitch->getRegionFromIndex(6)->getCenter(), 10, //BaseGameEntity
        			Vector2D(0.0,0.0), Vector2D(0,1), PlayerMass, PlayerMaxSpeedWithoutBall, PlayerMaxForce, PlayerMaxTurnRate,        //MovingEntity
        			this, 6, PlayerMaxSpeedWithBall, PlayerBase::attacker, //PlayerBase 
        			Wait::Instance() //FieldPlayer
			)
		);



        	m_Players.push_back
		(
			new FieldPlayer
			(
        			0, this->Game()->mFootballPitch->getRegionFromIndex(8)->getCenter(), 10, //BaseGameEntity
        			Vector2D(0.0,0.0), Vector2D(0,1), PlayerMass, PlayerMaxSpeedWithoutBall, PlayerMaxForce, PlayerMaxTurnRate,        //MovingEntity
        			this, 8, PlayerMaxSpeedWithBall, PlayerBase::attacker, //PlayerBase 
        			Wait::Instance() //FieldPlayer
			)
		);
        	
		m_Players.push_back
		(
			new FieldPlayer
			(
        			0, this->Game()->mFootballPitch->getRegionFromIndex(3)->getCenter(), 10, //BaseGameEntity
        			Vector2D(0.0,0.0), Vector2D(0,1), PlayerMass, PlayerMaxSpeedWithoutBall, PlayerMaxForce, PlayerMaxTurnRate,        //MovingEntity
        			this, 3, PlayerMaxSpeedWithBall, PlayerBase::defender, //PlayerBase 
        			Wait::Instance() //FieldPlayer
			)
		);
        	
		m_Players.push_back
		(
			new FieldPlayer
			(
        			0, this->Game()->mFootballPitch->getRegionFromIndex(5)->getCenter(), 10, //BaseGameEntity
        			Vector2D(0.0,0.0), Vector2D(0,1), PlayerMass, PlayerMaxSpeedWithoutBall, PlayerMaxForce, PlayerMaxTurnRate,        //MovingEntity
        			this, 5, PlayerMaxSpeedWithBall, PlayerBase::defender, //PlayerBase 
        			Wait::Instance() //FieldPlayer
			)
		);
    		
		//goalkeeper
    		m_Players.push_back
		(
			new GoalKeeper
			(
        			0, this->Game()->mFootballPitch->getRegionFromIndex(1)->getCenter(), 10, //BaseGameEntity
        			Vector2D(0.0,0.0), Vector2D(0,1), PlayerMass, PlayerMaxSpeedWithoutBall, PlayerMaxForce, PlayerMaxTurnRate,        //MovingEntity
        			this, 1, PlayerMaxSpeedWithBall, PlayerBase::goal_keeper, //PlayerBase 
        			TendGoal::Instance() //FieldPlayer
			)
		);
  	}

  	else
  	{



    		//create the players
    		m_Players.push_back
		(
		 	new FieldPlayer
			(
        			0, this->Game()->mFootballPitch->getRegionFromIndex(9)->getCenter(), 10, //BaseGameEntity
        			Vector2D(0.0,0.0), Vector2D(0,1), PlayerMass, PlayerMaxSpeedWithoutBall, PlayerMaxForce, PlayerMaxTurnRate,        //MovingEntity
        			this, 9, PlayerMaxSpeedWithBall, PlayerBase::attacker, //PlayerBase 
        			Wait::Instance() //FieldPlayer
			)
		);

    		m_Players.push_back
		(
		 	new FieldPlayer
			(
        			0, this->Game()->mFootballPitch->getRegionFromIndex(11)->getCenter(), 10, //BaseGameEntity
        			Vector2D(0.0,0.0), Vector2D(0,1), PlayerMass, PlayerMaxSpeedWithoutBall, PlayerMaxForce, PlayerMaxTurnRate,        //MovingEntity
        			this, 11, PlayerMaxSpeedWithBall, PlayerBase::attacker, //PlayerBase 
        			Wait::Instance() //FieldPlayer
			)
		);


 
    		m_Players.push_back
		(
		 	new FieldPlayer
			(
        			0, this->Game()->mFootballPitch->getRegionFromIndex(12)->getCenter(), 10, //BaseGameEntity
        			Vector2D(0.0,0.0), Vector2D(0,1), PlayerMass, PlayerMaxSpeedWithoutBall, PlayerMaxForce, PlayerMaxTurnRate,        //MovingEntity
        			this, 12, PlayerMaxSpeedWithBall, PlayerBase::defender, //PlayerBase 
        			Wait::Instance() //FieldPlayer
			)
		);

    		m_Players.push_back
		(
			new FieldPlayer
			(
        			0, this->Game()->mFootballPitch->getRegionFromIndex(14)->getCenter(), 10, //BaseGameEntity
        			Vector2D(0.0,0.0), Vector2D(0,1), PlayerMass, PlayerMaxSpeedWithoutBall, PlayerMaxForce, PlayerMaxTurnRate,        //MovingEntity
        			this, 14, PlayerMaxSpeedWithBall, PlayerBase::defender, //PlayerBase 
        			Wait::Instance() //FieldPlayer
			)
		);
     		
		//goalkeeper
    		m_Players.push_back
		(
		 	new GoalKeeper
			(
        			0, this->Game()->mFootballPitch->getRegionFromIndex(16)->getCenter(), 10, //BaseGameEntity
        			Vector2D(0.0,0.0), Vector2D(0,1), PlayerMass, PlayerMaxSpeedWithoutBall, PlayerMaxForce, PlayerMaxTurnRate,        //MovingEntity
        			this, 16, PlayerMaxSpeedWithBall, PlayerBase::goal_keeper, //PlayerBase 
        			TendGoal::Instance() //FieldPlayer
			)
		);
  	}


  //register the players with the entity manager
  std::vector<PlayerBase*>::iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {
    EntityMgr->RegisterEntity(*it);
  }
}


PlayerBase* SoccerTeam::GetPlayerFromID(int id)const
{
  std::vector<PlayerBase*>::const_iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {
    if ((*it)->ID() == id) return *it;
  }

  return NULL;
}


void SoccerTeam::SetPlayerHomeRegion(int plyr, int region)const
{
  assert ( (plyr>=0) && (plyr<(int)m_Players.size()) );

  m_Players[plyr]->SetHomeRegion(region);
}


//---------------------- UpdateTargetsOfWaitingPlayers ------------------------
//
//  
void SoccerTeam::UpdateTargetsOfWaitingPlayers()const
{
  std::vector<PlayerBase*>::const_iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {  
    if ( (*it)->Role() != PlayerBase::goal_keeper )
    {
      //cast to a field player
      FieldPlayer* plyr = static_cast<FieldPlayer*>(*it);
      
      if ( plyr->GetFSM()->isInState(*Wait::Instance()) ||
           plyr->GetFSM()->isInState(*ReturnToHomeRegion::Instance()) )
      {
        plyr->Steering()->SetTarget(plyr->HomeRegion()->getCenter());
      }
    }
  }
}


//--------------------------- AllPlayersAtHome --------------------------------
//
//  returns false if any of the team are not located within their home region
//-----------------------------------------------------------------------------
bool SoccerTeam::AllPlayersAtHome()const
{
  std::vector<PlayerBase*>::const_iterator it = m_Players.begin();

  for (it; it != m_Players.end(); ++it)
  {
    if ((*it)->InHomeRegion() == false)
    {
      return false;
    }
  }

  return true;
}

//------------------------- RequestPass ---------------------------------------
//
//  this tests to see if a pass is possible between the requester and
//  the controlling player. If it is possible a message is sent to the
//  controlling player to pass the ball asap.
//-----------------------------------------------------------------------------
void SoccerTeam::RequestPass(FieldPlayer* requester)const
{
  //maybe put a restriction here
  if (RandFloat() > 0.1) return;
  
  if (isPassSafeFromAllOpponents(ControllingPlayer()->Pos(),
                                 requester->Pos(),
                                 requester,
                                 MaxPassingForce))
  {

    //tell the player to make the pass
    //let the receiver know a pass is coming 
    Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
                          requester->ID(),
                          ControllingPlayer()->ID(),
                          Msg_PassToMe,
                          requester); 

  }
}


//----------------------------- isOpponentWithinRadius ------------------------
//
//  returns true if an opposing player is within the radius of the position
//  given as a parameter
//-----------------------------------------------------------------------------
bool SoccerTeam::isOpponentWithinRadius(Vector2D pos, double rad)
{
  std::vector<PlayerBase*>::const_iterator end = Opponents()->Members().end();
  std::vector<PlayerBase*>::const_iterator it;

  for (it=Opponents()->Members().begin(); it !=end; ++it)
  {
    if (Vec2DDistanceSq(pos, (*it)->Pos()) < rad*rad)
    {
      return true;
    }
  }

  return false;
}
