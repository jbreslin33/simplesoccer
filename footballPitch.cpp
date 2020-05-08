#include "footballPitch.h"
#include "footballGame.h"
#include "Game/Region.h"
#include "Goal.h"

FootballPitch::FootballPitch(FootballGame* footballGame)
{

	mFootballGame = footballGame;

        //member variables
        mNumRegionsHorizontal = 6;
        mNumRegionsVertical   = 3;
        mRegionVector.resize(mNumRegionsHorizontal * mNumRegionsVertical);
        mGoalWidth                               = 100;
        mNumSupportSpotsX = 13;
        mNumSupportSpotsY = 6;

	        //define the playing area
        mPlayingArea = new Region(20, 20, mFootballGame->mScreenX - 20, mFootballGame->mScreenY - 20);

        //create the regions
        createRegions
        (
                getPlayingArea()->Width() / (double)mNumRegionsHorizontal,
                getPlayingArea()->Height() / (double)mNumRegionsVertical
        );
        
	mGoalVector.push_back(new Goal
        (
                Vector2D
                (
                        mPlayingArea->Right(), mFootballGame->mScreenY - mGoalWidth/2
                ),
                Vector2D
                (
                        mPlayingArea->Right(), mFootballGame->mScreenY - (mFootballGame->mScreenY - mGoalWidth/2)
                ),
                Vector2D
                (
                        -1,0
                )
        ));

        //Goals
        mGoalVector.push_back(new Goal
        (
                Vector2D
                (
                        mPlayingArea->Left(), mFootballGame->mScreenY - mGoalWidth/2 //550
                ),
                Vector2D
                (
                        mPlayingArea->Left(), mFootballGame->mScreenY - (mFootballGame->mScreenY - mGoalWidth/2) //600 - (600 - 50) = 50
                ),
                Vector2D(1,0)
        ));

	//create the walls
        Vector2D TopLeft(mPlayingArea->Left(), mPlayingArea->Top());
        Vector2D TopRight(mPlayingArea->Right(), mPlayingArea->Top());
        Vector2D BottomRight(mPlayingArea->Right(), mPlayingArea->Bottom());
        Vector2D BottomLeft(mPlayingArea->Left(), mPlayingArea->Bottom());

        mWallVector.push_back(Wall2D(BottomLeft, mGoalVector.at(1)->RightPost()));
        mWallVector.push_back(Wall2D(mGoalVector.at(1)->LeftPost(), TopLeft));
        mWallVector.push_back(Wall2D(TopLeft, TopRight));

        mWallVector.push_back(Wall2D(TopRight, mGoalVector.at(0)->LeftPost()));
        mWallVector.push_back(Wall2D(mGoalVector.at(0)->RightPost(), BottomRight));
        mWallVector.push_back(Wall2D(BottomRight, BottomLeft));
}

//-------------------------------- dtor ----------------------------------
//------------------------------------------------------------------------
FootballPitch::~FootballPitch()
{
	for (int i = 0; i < mGoalVector.size(); i++)
	{
        	delete mGoalVector.at(i);
	}

        delete mPlayingArea;

        for (unsigned int i=0; i<mRegionVector.size(); ++i)
        {
                delete mRegionVector[i];
        }
}

const std::vector<Wall2D>& FootballPitch::getWallVector()
{	
	return mWallVector;
}

//------------------------- CreateRegions --------------------------------
void FootballPitch::createRegions(double width, double height)
{
        //index into the vector
        int idx = mRegionVector.size()-1;

        for (int col=0; col<mNumRegionsHorizontal; ++col)
        {
                for (int row=0; row<mNumRegionsVertical; ++row)
                {
                        mRegionVector[idx--] = new Region
                        (
                                getPlayingArea()->Left()+col*width,
                                getPlayingArea()->Top()+row*height,
                                getPlayingArea()->Left()+(col+1)*width,
                                getPlayingArea()->Top()+(row+1)*height,
                                idx
                        );
                }
        }
}

const Region* const FootballPitch::getPlayingArea()const
{
	return mPlayingArea;
}

//pass in an id and get back a region of the playing field
const Region* const FootballPitch::getRegionFromIndex(int idx)
{
	assert ( (idx >= 0) && (idx < (int)mRegionVector.size()) );

       	return mRegionVector[idx];
}


