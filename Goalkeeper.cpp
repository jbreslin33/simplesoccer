#include "Goalkeeper.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "SoccerPitch.h"
#include "2D/Transformations.h"
#include "GoalKeeperStates.h"
#include "Goal.h"
#include "Game/EntityFunctionTemplates.h"

//----------------------------- ctor ------------------------------------
//-----------------------------------------------------------------------
GoalKeeper::GoalKeeper(SoccerTeam*        home_team,
                       int                home_region,
                       State<GoalKeeper>* start_state,
                       Vector2D           heading,
                       Vector2D           velocity,
                       double              mass,
                       double              max_force,
                       double              max_speed,
                       double              max_turn_rate,
                       double              scale): PlayerBase(home_team,
                                                             home_region,
                                                             heading,
                                                             velocity,
                                                             mass,
                                                             max_force,
                                                             max_speed,
                                                             max_turn_rate,
                                                             scale,
                                                             PlayerBase::goal_keeper)
                                         
                                        
{   
   	//set up the state machine
  	m_pStateMachine = new StateMachine<GoalKeeper>(this);
  	m_pStateMachine->SetCurrentState(start_state);
  	m_pStateMachine->SetPreviousState(start_state);
  	m_pStateMachine->SetGlobalState(GlobalKeeperState::Instance());
  	m_pStateMachine->CurrentState()->Enter(this);        
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
  	if(Pitch()->bNonPenetrationConstraint)
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
  	if (!Pitch()->GoalKeeperHasBall())
  	{
   		m_vLookAt = Vec2DNormalize(Ball()->Pos() - Pos());
  	}
}


bool GoalKeeper::BallWithinRangeForIntercept()const
{
  return (Vec2DDistanceSq(Team()->HomeGoal()->Center(), Ball()->Pos()) <=
          Pitch()->GoalKeeperInterceptRangeSq);
}

bool GoalKeeper::TooFarFromGoalMouth()const
{
  return (Vec2DDistanceSq(Pos(), GetRearInterposeTarget()) >
          Pitch()->GoalKeeperInterceptRangeSq);
}

Vector2D GoalKeeper::GetRearInterposeTarget()const
{
	double xPosTarget = Team()->HomeGoal()->Center().x;

  	double yPosTarget = Pitch()->PlayingArea()->Center().y - 
                     Pitch()->GoalWidth*0.5 + (Ball()->Pos().y*Pitch()->GoalWidth) /
                     Pitch()->PlayingArea()->Height();

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

