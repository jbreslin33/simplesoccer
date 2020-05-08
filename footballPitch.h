#ifndef FOOTBALLPITCH_H
#define FOOTBALLPITCH_H

#include "2D/Wall2D.h"

class FootballGame;
class Region;
class Goal;

class FootballPitch
{
	public:
		FootballPitch(FootballGame* footballGame);
		~FootballPitch();

		FootballGame* mFootballGame;

		//params
               	double GoalWidth;

                int NumRegionsHorizontal;
                int NumRegionsVertical;

                int NumSupportSpotsX;
                int NumSupportSpotsY;

                /******       Teams   ***************/
                std::vector<Goal*>  mGoalVector;

		/******       Bounds   ***************/
                const Region* const getPlayingArea()const;

                const std::vector<Wall2D>& Walls(){return m_vecWalls;}

                //walls
                //container for the boundary walls
                std::vector<Wall2D>  m_vecWalls;

                //defines the dimensions of the playing area
                Region*              mPlayingArea;

                //the playing field is broken up into regions that the team
                //can make use of to implement strategies.
                std::vector<Region*> m_Regions;

                //this instantiates the regions the players utilize to  position
                //themselves
                void createRegions(double width, double height);

		//pass in an id and get back a region of the playing field
                const Region* const getRegionFromIndex(int idx);

};
#endif
