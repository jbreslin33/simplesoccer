#include "FieldPlayer.h"
#include "PlayerBase.h"
#include "SteeringBehaviors.h"
#include "2D/geometry.h"
#include "2D/C2DMatrix.h"
#include "Goal.h"
#include "Game/Region.h"
#include "Game/EntityFunctionTemplates.h"
#include "SoccerTeam.h"
#include "SoccerPitch.h"

#include <limits>

using std::vector;

//------------------------------- dtor ---------------------------------------
//----------------------------------------------------------------------------
FieldPlayer::~FieldPlayer()
{
  delete m_pStateMachine;
}

//----------------------------- ctor -------------------------------------
//------------------------------------------------------------------------
FieldPlayer::FieldPlayer(SoccerTeam* home_team,
                      int   home_region,
                      State<FieldPlayer>* start_state,
                      Vector2D  heading,
                      Vector2D velocity,
                      double    mass,
                      double    max_force,
                      double    max_speed,
                      double    max_speed_with_ball,
                      double    max_turn_rate,
                      double    scale,
                      player_role role): PlayerBase(home_team,
                                                    home_region,
                                                    heading,
                                                    velocity,
                                                    mass,
                                                    max_force,
                                                    max_speed,
                                                    max_speed_with_ball,
                                                    max_turn_rate,
                                                    scale,
                                                    role)                                    
{
  //set up the state machine
  m_pStateMachine =  new StateMachine<FieldPlayer>(this);

  if (start_state)
  {    
    m_pStateMachine->SetCurrentState(start_state);
    m_pStateMachine->SetPreviousState(start_state);
    m_pStateMachine->SetGlobalState(GlobalPlayerState::Instance());

    m_pStateMachine->CurrentState()->Enter(this);
  }    

  m_pSteering->SeparationOn();

}

//------------------------------ Update ----------------------------------
//
//  
//------------------------------------------------------------------------
void FieldPlayer::Update()
{ 
  
	//run the logic for the current state
  	m_pStateMachine->Update();

  	//calculate the combined steering force
  	m_pSteering->Calculate();

  	//if no steering force is produced decelerate the player by applying a
  	//braking force
  	if (m_pSteering->Force().isZero())
  	{
    		const double BrakingRate = 0.8; 

    		m_vVelocity = m_vVelocity * BrakingRate;                                     
  	}
  
  	//the steering force's side component is a force that rotates the 
  	//player about its axis. We must limit the rotation so that a player
  	//can only turn by PlayerMaxTurnRate rads per update.
  	double TurningForce =   m_pSteering->SideComponent();

  	Clamp(TurningForce, -MaxTurnRate(), MaxTurnRate());

  	//rotate the heading vector
  	Vec2DRotateAroundOrigin(m_vHeading, TurningForce);

  	//make sure the velocity vector points in the same direction as
  	//the heading vector
  	m_vVelocity = m_vHeading * m_vVelocity.Length();

  	//and recreate m_vSide
  	m_vSide = m_vHeading.Perp();


  	//now to calculate the acceleration due to the force exerted by
  	//the forward component of the steering force in the direction
  	//of the player's heading
  	Vector2D accel = m_vHeading * m_pSteering->ForwardComponent() / m_dMass;

  	m_vVelocity += accel;

  	//make sure player does not exceed maximum velocity
  	m_vVelocity.Truncate(m_dMaxSpeed);

  	//update the position
  	m_vPosition += m_vVelocity;


  	//enforce a non-penetration constraint if desired
  	if(Pitch()->bNonPenetrationConstraint)
  	{
    		EnforceNonPenetrationContraint(this, AutoList<PlayerBase>::GetAllMembers());
  	}
}

//-------------------- HandleMessage -------------------------------------
//
//  routes any messages appropriately
//------------------------------------------------------------------------
bool FieldPlayer::HandleMessage(const Telegram& msg)
{
  return m_pStateMachine->HandleMessage(msg);
}



