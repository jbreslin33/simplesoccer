#ifndef MOVING_ENTITY
#define MOVING_ENTITY
//------------------------------------------------------------------------
//
//  Name:   MovingEntity.h
//
//  Desc:   A base class defining an entity that moves. The entity has 
//          a local coordinate system and members for defining its
//          mass and velocity.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------

#include <cassert>

#include "2D/Vector2D.h"
#include "Game/BaseGameEntity.h"



class MovingEntity : public BaseGameEntity
{
	protected:
  
  		Vector2D    m_vVelocity;
  
  		//a normalized vector pointing in the direction the entity is heading. 
  		Vector2D    m_vHeading;

  		//a vector perpendicular to the heading vector
  		Vector2D    m_vSide; 

  		double      m_dMass;
  
  		//the maximum speed this entity may travel at.
  		double      m_dMaxSpeed;

  		//the maximum force this entity can produce to power itself 
  		//(think rockets and thrust)
  		double      m_dMaxForce;
  
  		//the maximum rate (radians per second)this vehicle can rotate         
  		double      m_dMaxTurnRate;

	public: 

		MovingEntity
		(
        		int id, Vector2D position, Vector2D scale, double boundingRadius, //BaseGameEntity
        		Vector2D velocity, Vector2D heading, double mass, double maxSpeed, double maxForce, double maxTurnRate         //MovingEntity
		)

		:

		BaseGameEntity
		(
        		id, position, scale, boundingRadius //BaseGameEntity
		)

		{
    			m_dBoundingRadius = boundingRadius; 
    			m_vScale = scale;
                                 
    			m_vHeading = heading;
			m_vVelocity = velocity;
        		m_dMass = mass;
        		m_vSide = m_vHeading.Perp();
        		m_dMaxSpeed = maxSpeed;
        		m_dMaxTurnRate = maxTurnRate;
        		m_dMaxForce = maxForce;
		} 


  		virtual ~MovingEntity(){}

  		//accessors
  		Vector2D Velocity()const;
  		void SetVelocity(const Vector2D& NewVel);
  
  		double Mass()const;
  
  		Vector2D Side()const;

  		double MaxSpeed()const;                       
  		void SetMaxSpeed(double new_speed);
  
  		double MaxForce()const;
  		void SetMaxForce(double mf);

  		bool IsSpeedMaxedOut()const;
  		double Speed()const;
  		double SpeedSq()const;
  
  		Vector2D  Heading()const;
  		void      SetHeading(Vector2D new_heading);
  		bool      RotateHeadingToFacePosition(Vector2D target);

  		double    MaxTurnRate()const;
  		void      SetMaxTurnRate(double val);
};
#endif
