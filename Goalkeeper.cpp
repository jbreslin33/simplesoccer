#include "Goalkeeper.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "footballGame.h"
#include "GoalKeeperStates.h"
#include "goal.h"
#include "Game/EntityFunctionTemplates.h"
#include "footballPitch.h"

//----------------------------- ctor ------------------------------------
//-----------------------------------------------------------------------
GoalKeeper::GoalKeeper
(
        int id, Vector2D position, double boundingRadius, //BaseGameEntity
        Vector2D velocity, Vector2D heading, double mass, double maxSpeed, double maxForce, double maxTurnRate,         //MovingEntity
        SoccerTeam* soccerTeam, int homeRegion, double maxSpeedWithBall, player_role role, //PlayerBase 
        State<GoalKeeper>* startState //GoalKeeper
)

:

PlayerBase
(
        id, position, boundingRadius, //BaseGameEntity
        velocity, heading, mass, maxSpeed, maxForce, maxTurnRate,         //MovingEntity
        soccerTeam, homeRegion, maxSpeedWithBall, role //PlayerBase
)
{   
   	//set up the state machine
  	m_pStateMachine = new StateMachine<GoalKeeper>(this);
  	m_pStateMachine->SetCurrentState(startState);
  	m_pStateMachine->SetPreviousState(startState);
  	m_pStateMachine->SetGlobalState(GlobalKeeperState::Instance());
  	m_pStateMachine->CurrentState()->Enter(this);        

        GoalKeeperTendingDistance               = 20.0;
        GoalKeeperInterceptRange                = 100.0;

        GoalKeeperInterceptRangeSq     = GoalKeeperInterceptRange * GoalKeeperInterceptRange;


}

//-------------------------- Update --------------------------------------

void GoalKeeper::Update()
{ 
  
	//run the logic for the current state
  	m_pStateMachine->Update();
	

  	//calculate the combined force from each steering behavior 
  	Vector2D SteeringForce = m_pSteering->Calculate();
	


	//Acceleration = Force/Mass
  	Vector2D Acceleration = SteeringForce / m_dMass;

  	//update velocity
  	m_vVelocity += Acceleration;

  	//make sure player does not exceed maximum velocity
  	m_vVelocity.Truncate(m_dMaxSpeed);

  	//update the position
  	m_vPosition += m_vVelocity;


  	//enforce a non-penetration constraint if desired
  	if(bNonPenetrationConstraint)
  	{
    		EnforceNonPenetrationContraint(this, AutoList<PlayerBase>::GetAllMembers());
  	}

 	//update the heading if the player has a non zero velocity
  	if ( !m_vVelocity.isZero())
  	{    
    		m_vHeading = Vec2DNormalize(m_vVelocity);

    		m_vSide = m_vHeading.Perp();
  	}

  	//look-at vector always points toward the ball
  	if (!getKeeperHasBall())
  	{
   		m_vLookAt = Vec2DNormalize(getBall()->Pos() - Pos());
  	}
}

bool GoalKeeper::BallWithinRangeForIntercept()const
{
	return (Vec2DDistanceSq(Team()->HomeGoal()->getCenter(), getBall()->Pos()) <=
          GoalKeeperInterceptRangeSq);
}

bool GoalKeeper::TooFarFromGoalMouth()const
{
	return (Vec2DDistanceSq(Pos(), GetRearInterposeTarget()) >
          GoalKeeperInterceptRangeSq);
}

Vector2D GoalKeeper::GetRearInterposeTarget()const
{
	double xPosTarget = Team()->HomeGoal()->getCenter().x;

  	double yPosTarget = Game()->mFootballPitch->getPlayingArea()->getCenter().y - 
                     Game()->mFootballPitch->mGoalWidth*0.5 + (getBall()->Pos().y*Game()->mFootballPitch->mGoalWidth) /
                     Game()->mFootballPitch->getPlayingArea()->getHeight();

  	return Vector2D(xPosTarget, yPosTarget); 
}

//-------------------- HandleMessage -------------------------------------
//
//  routes any messages appropriately
//------------------------------------------------------------------------
bool GoalKeeper::HandleMessage(const Telegram& msg)
{
	return m_pStateMachine->HandleMessage(msg);
}

