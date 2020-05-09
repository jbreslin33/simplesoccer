#include "footballPitch.h"
#include "footballGame.h"
#include "Game/Region.h"
#include "goal.h"

FootballPitch::FootballPitch(FootballGame* footballGame)
{

	mFootballGame = footballGame;

        //Regions of pitch
        mNumRegionsHorizontal = 6;
        mNumRegionsVertical   = 3;
        mRegionVector.resize(mNumRegionsHorizontal * mNumRegionsVertical);

        mGoalWidth                               = 100;
        mNumSupportSpotsX = 13;
        mNumSupportSpotsY = 6;

        //define the playing area
        mPlayingArea = new Region(20, 20, mFootballGame->mScreenX - 20, mFootballGame->mScreenY - 20);

        //create the regions
	double widthOfRegions = getPlayingArea()->getWidth() / (double)mNumRegionsHorizontal;
	double lengthOfRegions = getPlayingArea()->getHeight() / (double)mNumRegionsVertical;
        createRegions(widthOfRegions, lengthOfRegions);

	//away goal
	Vector2D leftPostAway(mPlayingArea->getRight(), mFootballGame->mScreenY - mGoalWidth/2);
	Vector2D rightPostAway(mPlayingArea->getRight(), mFootballGame->mScreenY - (mFootballGame->mScreenY - mGoalWidth/2));
	Vector2D facingAway(-1,0);
	mGoalVector.push_back(new Goal(leftPostAway, rightPostAway, facingAway));

	//home goal
	Vector2D leftPostHome(mPlayingArea->getLeft(), mFootballGame->mScreenY - mGoalWidth/2); 
	Vector2D rightPostHome(mPlayingArea->getLeft(), mFootballGame->mScreenY - (mFootballGame->mScreenY - mGoalWidth/2));
	Vector2D facingHome(1,0);
	mGoalVector.push_back(new Goal(leftPostHome, rightPostHome, facingHome));

	//create the walls
        Vector2D topLeftWall(mPlayingArea->getLeft(), mPlayingArea->getTop());
        Vector2D topRightWall(mPlayingArea->getRight(), mPlayingArea->getTop());
        Vector2D bottomRightWall(mPlayingArea->getRight(), mPlayingArea->getBottom());
        Vector2D bottomLeftWall(mPlayingArea->getLeft(), mPlayingArea->getBottom());

        mWallVector.push_back(Wall2D(bottomLeftWall, mGoalVector.at(1)->getRightPost()));
        mWallVector.push_back(Wall2D(mGoalVector.at(1)->getLeftPost(), topLeftWall));
        mWallVector.push_back(Wall2D(topLeftWall, topRightWall));

        mWallVector.push_back(Wall2D(topRightWall, mGoalVector.at(0)->getLeftPost()));
        mWallVector.push_back(Wall2D(mGoalVector.at(0)->getRightPost(), bottomRightWall));
        mWallVector.push_back(Wall2D(bottomRightWall, bottomLeftWall));
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
        int id = mRegionVector.size()-1;

        for (int col=0; col<mNumRegionsHorizontal; ++col)
        {
                for (int row=0; row<mNumRegionsVertical; ++row)
                {
                        mRegionVector[id--] = new Region
                        (
                                getPlayingArea()->getLeft()+col*width,
                                getPlayingArea()->getTop()+row*height,
                                getPlayingArea()->getLeft()+(col+1)*width,
                                getPlayingArea()->getTop()+(row+1)*height,
                                id
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


