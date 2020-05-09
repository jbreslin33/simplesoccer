#include "PlayerBase.h"
#include "SteeringBehaviors.h"
#include "2D/geometry.h"
#include "2D/C2DMatrix.h"
#include "Game/Region.h"
#include "Messaging/MessageDispatcher.h"
#include "SoccerMessages.h"
#include "SoccerTeam.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "footballGame.h"
#include "client.h"
#include "Game/MovingEntity.h"
#include "footballPitch.h"


using std::vector;


//----------------------------- dtor -------------------------------------
//------------------------------------------------------------------------
PlayerBase::~PlayerBase()
{
  delete m_pSteering;
}

//----------------------------- ctor -------------------------------------
//------------------------------------------------------------------------
//
//
PlayerBase::PlayerBase
(
        int id, Vector2D position, double boundingRadius, //BaseGameEntity
        Vector2D velocity, Vector2D heading, double mass, double maxSpeed, double maxForce, double maxTurnRate,       //MovingEntity
        SoccerTeam* soccerTeam, int homeRegion, double maxSpeedWithBall, player_role role //PlayerBase 
)

:

MovingEntity
(
        id, position, boundingRadius, //BaseGameEntity
        velocity, heading, mass, maxSpeed, maxForce, maxTurnRate         //MovingEntity
)	
{
	mClient = nullptr;

	//kicks
	mKickCounterThreshold = 4;
  	mKickCounter = 0;
	PlayerKickingAccuracy                   = 0.99;

	m_pTeam = soccerTeam;
   	m_dDistSqToBall = MaxFloat;
   	m_iHomeRegion = homeRegion;
   	m_iDefaultRegion = homeRegion;
   	m_PlayerRole = role;

	mPlayerMaxSpeedWithBall = maxSpeedWithBall;

  	//set up the steering behavior class
  	m_pSteering = new SteeringBehaviors(this,
                                      m_pTeam->Game(),
                                      getBall());  
  
  	//a player's start target is its start position (because it's just waiting)
  	m_pSteering->SetTarget(soccerTeam->Game()->mFootballPitch->getRegionFromIndex(homeRegion)->getCenter());

        mChancePlayerAttemptsPotShot             = 0.005;
        mChanceOfUsingArriveTypeReceiveBehavior  = 0.5;


        mKeeperHasBall = false;

        KeeperInBallRange                       = 10.0;
        PlayerInTargetRange                     = 10.0;
        PlayerKickingDistance                   = 6.0;
        PlayerKickFrequency                     = 8;

        PlayerComfortZone                       = 60.0;
        MaxDribbleForce                         = 1.5;
        WithinRangeOfHome                       = 15.0;

        mBallWithinReceivingRange                = 10.0;
        bShowControllingTeam                    = 1;
        SeparationCoefficient                   = 10.0;
        ViewDistance                            = 30.0;
        bNonPenetrationConstraint               = 0;


        //squared space
        BallWithinReceivingRangeSq = mBallWithinReceivingRange * mBallWithinReceivingRange;
        KeeperInBallRangeSq      = KeeperInBallRange * KeeperInBallRange;
        PlayerInTargetRangeSq    = PlayerInTargetRange * PlayerInTargetRange;
        PlayerKickingDistanceSq  = PlayerKickingDistance * PlayerKickingDistance;
        PlayerComfortZoneSq      = PlayerComfortZone * PlayerComfortZone;
        WithinRangeOfSupportSpotSq = WithinRangeOfSupportSpot * WithinRangeOfSupportSpot;


}




//----------------------------- TrackBall --------------------------------
//
//  sets the player's heading to point at the ball
//------------------------------------------------------------------------
void PlayerBase::TrackBall()
{
	RotateHeadingToFacePosition(getBall()->Pos());  
}

//----------------------------- TrackTarget --------------------------------
//
//  sets the player's heading to point at the current target
//------------------------------------------------------------------------
void PlayerBase::TrackTarget()
{
	SetHeading(Vec2DNormalize(Steering()->Target() - Pos()));
}


//------------------------------------------------------------------------
//
//binary predicates for std::sort (see CanPassForward/Backward)
//------------------------------------------------------------------------
bool  SortByDistanceToOpponentsGoal(const PlayerBase*const p1,
                                    const PlayerBase*const p2)
{
	return (p1->DistToOppGoal() < p2->DistToOppGoal());
}

bool  SortByReversedDistanceToOpponentsGoal(const PlayerBase*const p1,
                                            const PlayerBase*const p2)
{
	return (p1->DistToOppGoal() > p2->DistToOppGoal());
}


//------------------------- WithinFieldOfView ---------------------------
//
//  returns true if subject is within field of view of this player
//-----------------------------------------------------------------------
bool PlayerBase::PositionInFrontOfPlayer(Vector2D position)const
{
	Vector2D ToSubject = position - Pos();

  	if (ToSubject.Dot(Heading()) > 0) 
	{
    		return true;
	}
  	else
	{
    		return false;
	}
}

//------------------------- IsThreatened ---------------------------------
//
//  returns true if there is an opponent within this player's 
//  comfort zone
//------------------------------------------------------------------------
bool PlayerBase::isThreatened()const
{
	//check against all opponents to make sure non are within this
  	//player's comfort zone
  	std::vector<PlayerBase*>::const_iterator curOpp;  
  	curOpp = Team()->Opponents()->Members().begin();
 
  	for (curOpp; curOpp != Team()->Opponents()->Members().end(); ++curOpp)
  	{
    		//calculate distance to the player. if dist is less than our
    		//comfort zone, and the opponent is infront of the player, return true
    		if (PositionInFrontOfPlayer((*curOpp)->Pos()) &&
       			(Vec2DDistanceSq(Pos(), (*curOpp)->Pos()) < PlayerComfortZoneSq))
    		{        
      			return true;
    		}
  	}// next opp

  	return false;
}

//----------------------------- FindSupport -----------------------------------
//
//  determines the player who is closest to the SupportSpot and messages him
//  to tell him to change state to SupportAttacker
//-----------------------------------------------------------------------------
void PlayerBase::FindSupport()const
{    
  
	//if there is no support we need to find a suitable player.
  	if (Team()->SupportingPlayer() == NULL)
  	{
    		PlayerBase* BestSupportPly = Team()->DetermineBestSupportingAttacker();

    		Team()->SetSupportingPlayer(BestSupportPly);

    		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
                            ID(),
                            Team()->SupportingPlayer()->ID(),
                            Msg_SupportAttacker,
                            NULL);
  	}
    
  	PlayerBase* BestSupportPly = Team()->DetermineBestSupportingAttacker();
    
  	//if the best player available to support the attacker changes, update
  	//the pointers and send messages to the relevant players to update their
  	//states
  	if (BestSupportPly && (BestSupportPly != Team()->SupportingPlayer()))
  	{
    
    		if (Team()->SupportingPlayer())
    		{
      			Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
                              ID(),
                              Team()->SupportingPlayer()->ID(),
                              Msg_GoHome,
                              NULL);
    		}
    
    		Team()->SetSupportingPlayer(BestSupportPly);

    		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
                            ID(),
                            Team()->SupportingPlayer()->ID(),
                            Msg_SupportAttacker,
                            NULL);
  	}
}


//calculate distance to opponent's goal. Used frequently by the passing//methods
double PlayerBase::DistToOppGoal()const
{
	return fabs(Pos().x - Team()->OpponentsGoal()->getCenter().x);
}

double PlayerBase::DistToHomeGoal()const
{
	return fabs(Pos().x - Team()->HomeGoal()->getCenter().x);
}

bool PlayerBase::isControllingPlayer()const
{
	return Team()->ControllingPlayer()==this;
}

bool PlayerBase::BallWithinKeeperRange()const
{
	return (Vec2DDistanceSq(Pos(), getBall()->Pos()) < KeeperInBallRange);
}

bool PlayerBase::BallWithinReceivingRange()const
{
	return (Vec2DDistanceSq(Pos(), getBall()->Pos()) < BallWithinReceivingRangeSq);
}

bool PlayerBase::BallWithinKickingRange()const
{
	return (Vec2DDistanceSq(getBall()->Pos(), Pos()) < PlayerKickingDistanceSq);
}


bool PlayerBase::InHomeRegion()const
{
	if (m_PlayerRole == goal_keeper)
  	{
    		return Game()->mFootballPitch->getRegionFromIndex(m_iHomeRegion)->getIsInside(Pos(), 1);
  	}
  	else
  	{
    		return Game()->mFootballPitch->getRegionFromIndex(m_iHomeRegion)->getIsInside(Pos(), 0);
  	}
}

bool PlayerBase::AtTarget()const
{
	return (Vec2DDistanceSq(Pos(), Steering()->Target()) < PlayerInTargetRangeSq);
}

bool PlayerBase::isClosestTeamMemberToBall()const
{
	return Team()->PlayerClosestToBall() == this;
}

bool PlayerBase::isClosestPlayerOnGameToBall()const
{
	return isClosestTeamMemberToBall() && 
         (DistSqToBall() < Team()->Opponents()->ClosestDistToBallSq());
}

bool PlayerBase::InHotRegion()const
{
	return fabs(Pos().y - Team()->OpponentsGoal()->getCenter().y ) <
         Game()->mFootballPitch->getPlayingArea()->getLength()/3.0;
}

bool PlayerBase::isAheadOfAttacker()const
{
	return fabs(Pos().x - Team()->OpponentsGoal()->getCenter().x) <
         fabs(Team()->ControllingPlayer()->Pos().x - Team()->OpponentsGoal()->getCenter().x);
}

SoccerBall* const PlayerBase::getBall()const
{
	return Team()->Game()->getBall();
}

FootballGame* const PlayerBase::Game()const
{
	return Team()->Game();
}

const Region* const PlayerBase::HomeRegion()const
{
	return Game()->mFootballPitch->getRegionFromIndex(m_iHomeRegion);
}

double PlayerBase::getChancePlayerAttemptsPotShot() const
{
	return mChancePlayerAttemptsPotShot;
}
void PlayerBase::setChancePlayerAttemptsPotShot(double d) 
{
        mChancePlayerAttemptsPotShot = d;
}

double PlayerBase::getChanceOfUsingArriveTypeReceiveBehavior() const
{
       return mChanceOfUsingArriveTypeReceiveBehavior;
}
void PlayerBase::setChanceOfUsingArriveTypeReceiveBehavior(double d)
{
       mChanceOfUsingArriveTypeReceiveBehavior = d;
}

//----------------------------- AddNoiseToKick --------------------------------
//
//  this can be used to vary the accuracy of a player's kick. Just call it 
//  prior to kicking the ball using the ball's position and the ball target as
//  parameters.
//-----------------------------------------------------------------------------
Vector2D PlayerBase::AddNoiseToKick(Vector2D BallPos, Vector2D BallTarget)
{
        double displacement = (Pi - Pi * PlayerKickingAccuracy) * RandomClamped();

        Vector2D toTarget = BallTarget - BallPos;

        Vec2DRotateAroundOrigin(toTarget, displacement);

        return toTarget + BallPos;
}

