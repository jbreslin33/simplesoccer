#include "2D/Vector2D.h"
#include "MovingEntity.h"
  
//accessors
Vector2D MovingEntity::Velocity()const
{
	return m_vVelocity;
}

void MovingEntity::SetVelocity(const Vector2D& NewVel)
{
	m_vVelocity = NewVel;
}

double MovingEntity::Mass()const
{
	return m_dMass;
}

Vector2D MovingEntity::Side()const
{
	return m_vSide;
}

double MovingEntity::MaxSpeed()const
{
	return m_dMaxSpeed;
}

void MovingEntity::SetMaxSpeed(double new_speed)
{
	m_dMaxSpeed = new_speed;
}

double MovingEntity::MaxForce()const
{
	return m_dMaxForce;
}

void MovingEntity::SetMaxForce(double mf)
{
	m_dMaxForce = mf;
}

bool MovingEntity::IsSpeedMaxedOut()const
{
	return m_dMaxSpeed*m_dMaxSpeed >= m_vVelocity.LengthSq();
}

double MovingEntity::Speed()const
{
	return m_vVelocity.Length();
}

double MovingEntity::SpeedSq()const
{
	return m_vVelocity.LengthSq();
}

Vector2D MovingEntity::Heading()const
{
	return m_vHeading;
}


double MovingEntity::MaxTurnRate()const
{
	return m_dMaxTurnRate;
}

void MovingEntity::SetMaxTurnRate(double val)
{
	m_dMaxTurnRate = val;
}


//--------------------------- RotateHeadingToFacePosition ---------------------
//
//  given a target position, this method rotates the entity's heading and
//  side vectors by an amount not greater than m_dMaxTurnRate until it
//  directly faces the target.
//
//  returns true when the heading is facing in the desired direction
//-----------------------------------------------------------------------------
bool MovingEntity::RotateHeadingToFacePosition(Vector2D target)
{
	Vector2D toTarget = Vec2DNormalize(target - m_vPosition);

  	double dot = m_vHeading.Dot(toTarget);

  	//some compilers lose acurracy so the value is clamped to ensure it
  	//remains valid for the acos
  	Clamp(dot, -1, 1);

  	//first determine the angle between the heading vector and the target
  	double angle = acos(dot);

  	//return true if the player is facing the target
  	if (angle < 0.00001) 
	{
		return true;
	}

  	//clamp the amount to turn to the max turn rate
  	if (angle > m_dMaxTurnRate) 
	{
		angle = m_dMaxTurnRate;
	}

  	//The next few lines use a rotation matrix to rotate the player's heading
  	//vector accordingly
        C2DMatrix RotationMatrix;

  	//notice how the direction of rotation has to be determined when creating
  	//the rotation matrix
        RotationMatrix.Rotate(angle * m_vHeading.Sign(toTarget));
  	RotationMatrix.TransformVector2Ds(m_vHeading);
  	RotationMatrix.TransformVector2Ds(m_vVelocity);

  	//finally recreate m_vSide
 	m_vSide = m_vHeading.Perp();

  	return false;
}


//------------------------- SetHeading ----------------------------------------
//
//  first checks that the given heading is not a vector of zero length. If the
//  new heading is valid this fumction sets the entity's heading and side 
//  vectors accordingly
//-----------------------------------------------------------------------------
void MovingEntity::SetHeading(Vector2D new_heading)
{
	assert( (new_heading.LengthSq() - 1.0) < 0.00001);

  	m_vHeading = new_heading;

  	//the side vector must always be perpendicular to the heading
  	m_vSide = m_vHeading.Perp();
}

