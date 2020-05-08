#include "footballPitch.h"
#include "footballGame.h"
#include "Game/Region.h"
#include "Goal.h"

FootballPitch::FootballPitch(FootballGame* footballGame)
{

	mFootballGame = footballGame;

        //member variables
        NumRegionsHorizontal = 6;
        NumRegionsVertical   = 3;
        m_Regions.resize(NumRegionsHorizontal * NumRegionsVertical);
        GoalWidth                               = 100;
        NumSupportSpotsX = 13;
        NumSupportSpotsY = 6;

	        //define the playing area
        mPlayingArea = new Region(20, 20, mFootballGame->mScreenX - 20, mFootballGame->mScreenY - 20);

        //create the regions
        CreateRegions
        (
                getPlayingArea()->Width() / (double)NumRegionsHorizontal,
                getPlayingArea()->Height() / (double)NumRegionsVertical
        );
        
	mGoalVector.push_back(new Goal
        (
                Vector2D
                (
                        mPlayingArea->Right(), mFootballGame->mScreenY - GoalWidth/2
                ),
                Vector2D
                (
                        mPlayingArea->Right(), mFootballGame->mScreenY - (mFootballGame->mScreenY - GoalWidth/2)
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
                        mPlayingArea->Left(), mFootballGame->mScreenY - GoalWidth/2 //550
                ),
                Vector2D
                (
                        mPlayingArea->Left(), mFootballGame->mScreenY - (mFootballGame->mScreenY - GoalWidth/2) //600 - (600 - 50) = 50
                ),
                Vector2D(1,0)
        ));

	//create the walls
        Vector2D TopLeft(mPlayingArea->Left(), mPlayingArea->Top());
        Vector2D TopRight(mPlayingArea->Right(), mPlayingArea->Top());
        Vector2D BottomRight(mPlayingArea->Right(), mPlayingArea->Bottom());
        Vector2D BottomLeft(mPlayingArea->Left(), mPlayingArea->Bottom());

        m_vecWalls.push_back(Wall2D(BottomLeft, mGoalVector.at(1)->RightPost()));
        m_vecWalls.push_back(Wall2D(mGoalVector.at(1)->LeftPost(), TopLeft));
        m_vecWalls.push_back(Wall2D(TopLeft, TopRight));

        m_vecWalls.push_back(Wall2D(TopRight, mGoalVector.at(0)->LeftPost()));
        m_vecWalls.push_back(Wall2D(mGoalVector.at(0)->RightPost(), BottomRight));
        m_vecWalls.push_back(Wall2D(BottomRight, BottomLeft));
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

        for (unsigned int i=0; i<m_Regions.size(); ++i)
        {
                delete m_Regions[i];
        }
}

//------------------------- CreateRegions --------------------------------
void FootballPitch::CreateRegions(double width, double height)
{
        //index into the vector
        int idx = m_Regions.size()-1;

        for (int col=0; col<NumRegionsHorizontal; ++col)
        {
                for (int row=0; row<NumRegionsVertical; ++row)
                {
                        m_Regions[idx--] = new Region
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



