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
        m_pPlayingArea = new Region(20, 20, mFootballGame->mScreenX - 20, mFootballGame->mScreenY - 20);

        //create the regions
        CreateRegions
        (
                PlayingArea()->Width() / (double)NumRegionsHorizontal,
                PlayingArea()->Height() / (double)NumRegionsVertical
        );

        //Goals
        m_pRedGoal  = new Goal
        (
                Vector2D
                (
                        m_pPlayingArea->Left(), mFootballGame->mScreenY - GoalWidth/2 //550
                ),
                Vector2D
                (
                        m_pPlayingArea->Left(), mFootballGame->mScreenY - (mFootballGame->mScreenY - GoalWidth/2) //600 - (600 - 50) = 50
                ),
                Vector2D(1,0)
        );

        m_pBlueGoal = new Goal
        (
                Vector2D
                (
                        m_pPlayingArea->Right(), mFootballGame->mScreenY - GoalWidth/2
                ),
                Vector2D
                (
                        m_pPlayingArea->Right(), mFootballGame->mScreenY - (mFootballGame->mScreenY - GoalWidth/2)
                ),
                Vector2D
                (
                        -1,0
                )
        );

	//create the walls
        Vector2D TopLeft(m_pPlayingArea->Left(), m_pPlayingArea->Top());
        Vector2D TopRight(m_pPlayingArea->Right(), m_pPlayingArea->Top());
        Vector2D BottomRight(m_pPlayingArea->Right(), m_pPlayingArea->Bottom());
        Vector2D BottomLeft(m_pPlayingArea->Left(), m_pPlayingArea->Bottom());

        m_vecWalls.push_back(Wall2D(BottomLeft, m_pRedGoal->RightPost()));
        m_vecWalls.push_back(Wall2D(m_pRedGoal->LeftPost(), TopLeft));
        m_vecWalls.push_back(Wall2D(TopLeft, TopRight));
        m_vecWalls.push_back(Wall2D(TopRight, m_pBlueGoal->LeftPost()));
        m_vecWalls.push_back(Wall2D(m_pBlueGoal->RightPost(), BottomRight));
        m_vecWalls.push_back(Wall2D(BottomRight, BottomLeft));
}

//-------------------------------- dtor ----------------------------------
//------------------------------------------------------------------------
FootballPitch::~FootballPitch()
{
        delete m_pRedGoal;
        delete m_pBlueGoal;

        delete m_pPlayingArea;

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
                                PlayingArea()->Left()+col*width,
                                PlayingArea()->Top()+row*height,
                                PlayingArea()->Left()+(col+1)*width,
                                PlayingArea()->Top()+(row+1)*height,
                                idx
                        );
                }
        }
}


