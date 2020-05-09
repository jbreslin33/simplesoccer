#include "Goal.h"

Goal::Goal(Vector2D left, Vector2D right, Vector2D facing)
{
	m_vLeftPost = left;
        m_vRightPost = right;
        m_vCenter = (left+right) / 2.0;
        m_iNumGoalsScored = 0;
        m_vFacing = facing;
}

Vector2D Goal::Center()
{
	return m_vCenter;
}

Vector2D Goal::Facing()const
{
	return m_vFacing;
}

Vector2D Goal::LeftPost()const
{
	return m_vLeftPost;
}

Vector2D Goal::RightPost()const
{
	return m_vRightPost;
}

int Goal::NumGoalsScored()const
{
	return m_iNumGoalsScored;
}

void Goal::ResetGoalsScored()
{
	m_iNumGoalsScored = 0;
}

bool Goal::Scored(const SoccerBall*const ball)
{
	if (LineIntersection2D(ball->Pos(), ball->OldPos(), m_vLeftPost, m_vRightPost))
  	{
    		++m_iNumGoalsScored;

    		return true;
  	}

  	return false;
}
