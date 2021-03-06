#ifndef SOCCERBALL_H
#define SOCCERBALL_H
//------------------------------------------------------------------------
//
//  Name: SoccerBall.h
//
//  Desc: Class to implement a soccer ball. This class inherits from
//        MovingEntity and provides further functionality for collision
//        testing and position prediction.
//
//  Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>

#include "Game/MovingEntity.h"
#include "constants.h"


class Wall2D;
class PlayerBase;
class FootballGame;


class SoccerBall : public MovingEntity
{
	private:

  		//keeps a record of the ball's position at the last update
  		Vector2D                  m_vOldPos;

  		//a local reference to the Walls that make up the game boundary

	public:

      		double Friction;

    		//tests to see if the ball has collided with a ball and reflects 
		//the ball's velocity accordingly
  		void TestCollisionWithWalls(const std::vector<Wall2D>& walls);
		
		SoccerBall
		(
        		int id, Vector2D position, double boundingRadius, //BaseGameEntity
        		Vector2D velocity, Vector2D heading, double mass, double maxSpeed, double maxForce, double maxTurnRate,       //MovingEntity
			FootballGame* footballGame
		);

  		//implement base class Update
  		void      Update();

  		//a soccer ball doesn't need to handle messages
  		bool      HandleMessage(const Telegram& msg){return false;}

  		//this method applies a directional force to the ball (kicks it!)
  		void      Kick(Vector2D direction, double force);

  		//given a kicking force and a distance to traverse defined by start
  		//and finish points, this method calculates how long it will take the
  		//ball to cover the distance.
  		double    TimeToCoverDistance(Vector2D from,
                               Vector2D to,
                               double     force)const;

  		
		//this method calculates where the ball will in 'time' seconds
  		Vector2D FuturePosition(double time)const;

  		//this is used by players and goalkeepers to 'trap' a ball -- to stop
  		//it dead. That player is then assumed to be in possession of the ball
  		//and m_pOwner is adjusted accordingly
  		void      Trap(){m_vVelocity.Zero();}  

  		Vector2D  OldPos()const{return m_vOldPos;}
  
  		//this places the ball at the desired location and sets its velocity to zero
  		void      PlaceAtPosition(Vector2D NewPos);

		FootballGame* const       Game()const;


		FootballGame* m_pGame;
};




#endif
