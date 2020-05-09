#include "goal.h"

Goal::Goal(Vector2D left, Vector2D right, Vector2D facing)
{
	mLeftPost = left;
        mRightPost = right;
        mCenter = (left+right) / 2.0;
        mNumGoalsScored = 0;
        mFacing = facing;
}

Vector2D Goal::getCenter()
{
	return mCenter;
}

Vector2D Goal::getFacing()const
{
	return mFacing;
}

Vector2D Goal::getLeftPost()const
{
	return mLeftPost;
}

Vector2D Goal::getRightPost()const
{
	return mRightPost;
}

int Goal::getNumGoalsScored()const
{
	return mNumGoalsScored;
}

void Goal::resetGoalsScored()
{
	mNumGoalsScored = 0;
}

bool Goal::isGoalScored(const SoccerBall*const ball)
{
	if (LineIntersection2D(ball->Pos(), ball->OldPos(), mLeftPost, mRightPost))
  	{
    		++mNumGoalsScored;

    		return true;
  	}

  	return false;
}
