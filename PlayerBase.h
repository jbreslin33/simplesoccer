#ifndef PLAYERBASE_H
#define PLAYERBASE_H
//------------------------------------------------------------------------
//
//  Name: PlayerBase.h
//
//  Desc: Definition of a soccer player base class. The player inherits
//        from the autolist class so that any player created will be 
//        automatically added to a list that is easily accesible by any
//        other game objects. (mainly used by the steering behaviors and
//        player state classes)
//
//  Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
#include <string>
#include <cassert>
#include "misc/autolist.h"
#include "2D/Vector2D.h"
#include "Game/MovingEntity.h"

class SoccerTeam;
class FootballGame;
class SoccerBall;
class SteeringBehaviors;
class Region;
class Client;


class PlayerBase : public MovingEntity,
                   public AutoList<PlayerBase>
{

	public:
  
  		enum player_role{goal_keeper, attacker, defender};

  		Client* mClient;

  		std::string mStateName;

  		//kicks
  		int mKickCounterThreshold;
  		int mKickCounter;
		double PlayerKickingAccuracy;

  		double mPlayerMaxSpeedWithBall;
		Vector2D AddNoiseToKick(Vector2D BallPos, Vector2D BallTarget);

	protected:

  		//this player's role in the team
  		player_role             m_PlayerRole;

  		//a pointer to this player's team
  		SoccerTeam*             m_pTeam;
 
  		//the steering behaviors
  		SteeringBehaviors*      m_pSteering;

  		//the region that this player is assigned to.
  		int                     m_iHomeRegion;

  		//the region this player moves to before kickoff
  		int                     m_iDefaultRegion;

  		//the distance to the ball (in squared-space). This value is queried 
  		//a lot so it's calculated once each time-step and stored here.
  		double                   m_dDistSqToBall;

  		//the vertex buffer
  		std::vector<Vector2D>   m_vecPlayerVB;

  		//the buffer for the transformed vertices
  		std::vector<Vector2D>   m_vecPlayerVBTrans;

	public:
		PlayerBase
		(
        		int id, Vector2D position, double boundingRadius, //BaseGameEntity
        		Vector2D velocity, Vector2D heading, double mass, double maxSpeed, double maxForce, double maxTurnRate,         //MovingEntity
        		SoccerTeam* soccerTeam, int homeRegion, double maxSpeedWithBall, player_role role //PlayerBase 
		);

  		virtual ~PlayerBase();


		//returns true if there is an opponent within this player's 
  		//comfort zone
  		bool        isThreatened()const;

  		//rotates the player to face the ball or the player's current target
  		void        TrackBall();
  		void        TrackTarget();

  		//this messages the player that is closest to the supporting spot to
  		//change state to support the attacking player
  		void        FindSupport()const;

  		//returns true if the ball can be grabbed by the goalkeeper
  		bool        BallWithinKeeperRange()const;

  		//returns true if the ball is within kicking range
  		bool        BallWithinKickingRange()const;

  		//returns true if a ball comes within range of a receiver
  		bool        BallWithinReceivingRange()const;

  		//returns true if the player is located within the boundaries 
  		//of his home region
  		bool        InHomeRegion()const;

  		//returns true if this player is ahead of the attacker
  		bool        isAheadOfAttacker()const;
  
  		//returns true if a player is located at the designated support spot
  		bool        AtSupportSpot()const;

  		//returns true if the player is located at his steering target
  		bool        AtTarget()const;

  		//returns true if the player is the closest player in his team to
  		//the ball
  		bool        isClosestTeamMemberToBall()const;

  		//returns true if the point specified by 'position' is located in
  		//front of the player
  		bool        PositionInFrontOfPlayer(Vector2D position)const;

  		//returns true if the player is the closest player on the game to the ball
  		bool        isClosestPlayerOnGameToBall()const;

  		//returns true if this player is the controlling player
  		bool        isControllingPlayer()const;

  		//returns true if the player is located in the designated 'hot region' --
  		//the area close to the opponent's goal
  		bool        InHotRegion()const;

  		player_role Role()const{return m_PlayerRole;}

  		double       DistSqToBall()const{return m_dDistSqToBall;}
  		void        SetDistSqToBall(double val){m_dDistSqToBall = val;}

  		//calculate distance to opponent's/home goal. Used frequently by the passing
  		//methods
  		double       DistToOppGoal()const;
  		double       DistToHomeGoal()const;

  		void        SetDefaultHomeRegion(){m_iHomeRegion = m_iDefaultRegion;}

  		SoccerBall* const        getBall()const;
  		FootballGame* const       Game()const;
  		SteeringBehaviors*const  Steering()const{return m_pSteering;}
  		const Region* const      HomeRegion()const;
  		void                     SetHomeRegion(int NewRegion){m_iHomeRegion = NewRegion;}
  		SoccerTeam*const         Team()const{return m_pTeam;}

                double getChancePlayerAttemptsPotShot() const;
                double getChanceOfUsingArriveTypeReceiveBehavior() const;
                
		void setChancePlayerAttemptsPotShot(double d);
                void setChanceOfUsingArriveTypeReceiveBehavior(double d);


                /******       game play member variables ***************/

                //true if a goal keeper has possession
                bool mKeeperHasBall;

                double KeeperInBallRange;

                double PlayerInTargetRange;
                double PlayerKickingDistance;
                double PlayerKickFrequency;

                double PlayerComfortZone;

                double MaxDribbleForce;

                double WithinRangeOfHome;
                double WithinRangeOfSupportSpot;

                double MinPassDist;
                double mBallWithinReceivingRange;

                bool bStates;
                bool bIDs;
                bool bSupportSpots;
                bool bRegions;
                bool bShowControllingTeam;
                bool bViewTargets;
                bool bHighlightIfThreatened;

                double SeparationCoefficient;
                double ViewDistance;
                bool bNonPenetrationConstraint;

                //squared space
                double BallWithinReceivingRangeSq;
                double KeeperInBallRangeSq;
                double PlayerInTargetRangeSq;
                double PlayerKickingDistanceSq;
                double PlayerComfortZoneSq;
                double GoalKeeperInterceptRangeSq;
                double WithinRangeOfSupportSpotSq;

                bool  getKeeperHasBall()const{return mKeeperHasBall;}
                void  setKeeperHasBall(bool b){mKeeperHasBall = b;}


	private:
                double mChancePlayerAttemptsPotShot;
                double mChanceOfUsingArriveTypeReceiveBehavior;



  
};





#endif
