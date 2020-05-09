#ifndef GOAL_H
#define GOAL_H
//------------------------------------------------------------------------
//
//Name:   Goal.h
//
//Desc:   class to define a goal for a soccer game. The goal is defined
//        by two 2D vectors representing the left and right posts.
//
//        Each time-step the method Scored should be called to determine
//        if a goal has been scored.
//
//Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include "SoccerBall.h"
#include "2D/Vector2D.h"
#include "2D/geometry.h"

class Goal 
{
	private:

  		Vector2D   m_vLeftPost;
  		Vector2D   m_vRightPost;

  		//a vector representing the facing direction of the goal
  		Vector2D   m_vFacing;

  		//the position of the center of the goal line
  		Vector2D   m_vCenter;

  		//each time Scored() detects a goal this is incremented
  		int        m_iNumGoalsScored;

	public:

  		Goal(Vector2D left, Vector2D right, Vector2D facing);

  		//Given the current ball position and the previous ball position,
  		//this method returns true if the ball has crossed the goal line 
  		//and increments m_iNumGoalsScored
  		bool Scored(const SoccerBall*const ball);

  		//-----------------------------------------------------accessor methods
 	 	Vector2D Center();
  		Vector2D Facing()const;
  		Vector2D LeftPost()const;
  		Vector2D RightPost()const;

  		int      NumGoalsScored()const;
  		void     ResetGoalsScored();
};
#endif
