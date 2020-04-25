#include "SupportSpotCalculator.h"
#include "PlayerBase.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "constants.h"
#include "SoccerTeam.h"
#include "ParamLoader.h"
#include "SoccerPitch.h"


//------------------------------- dtor ----------------------------------------
//-----------------------------------------------------------------------------
SupportSpotCalculator::~SupportSpotCalculator()
{
}


//------------------------------- ctor ----------------------------------------
//-----------------------------------------------------------------------------
SupportSpotCalculator::SupportSpotCalculator(int           numX,
                                             int           numY,
                                             SoccerTeam*   team):m_pBestSupportingSpot(NULL),
                                                                  m_pTeam(team)
{
  const Region* PlayingField = team->Pitch()->PlayingArea();

  //calculate the positions of each sweet spot, create them and 
  //store them in m_Spots
  double HeightOfSSRegion = PlayingField->Height() * 0.8;
  double WidthOfSSRegion  = PlayingField->Width() * 0.9;
  double SliceX = WidthOfSSRegion / numX ;
  double SliceY = HeightOfSSRegion / numY;

  double left  = PlayingField->Left() + (PlayingField->Width()-WidthOfSSRegion)/2.0 + SliceX/2.0;
  double right = PlayingField->Right() - (PlayingField->Width()-WidthOfSSRegion)/2.0 - SliceX/2.0;
  double top   = PlayingField->Top() + (PlayingField->Height()-HeightOfSSRegion)/2.0 + SliceY/2.0;

  for (int x=0; x<(numX/2)-1; ++x)
  {
    for (int y=0; y<numY; ++y)
    {      
      if (m_pTeam->Color() == SoccerTeam::blue)
      {
        m_Spots.push_back(SupportSpot(Vector2D(left+x*SliceX, top+y*SliceY), 0.0));
      }

      else
      {
        m_Spots.push_back(SupportSpot(Vector2D(right-x*SliceX, top+y*SliceY), 0.0));
      }
    }
  }
  
}


//--------------------------- DetermineBestSupportingPosition -----------------
//
//  see header or book for description
//-----------------------------------------------------------------------------
Vector2D SupportSpotCalculator::DetermineBestSupportingPosition()
{
	printf("SupportSpotCalculator::DetermineBestSupportingPosition()\n");
	//only update the spots every few frames                              
	//BRESLIN
  	/*
  	if (!m_pRegulator->isReady() && m_pBestSupportingSpot)
  	{
    		return m_pBestSupportingSpot->m_vPos;
  	}
  	*/

  	//reset the best supporting spot
  	m_pBestSupportingSpot = nullptr;
	printf("SupportSpotCalculator::DetermineBestSupportingPosition() 1\n");
 
  	double BestScoreSoFar = 0.0;
	printf("SupportSpotCalculator::DetermineBestSupportingPosition() 2\n");

  	std::vector<SupportSpot>::iterator curSpot;
	printf("SupportSpotCalculator::DetermineBestSupportingPosition() 3\n");

  	for (curSpot = m_Spots.begin(); curSpot != m_Spots.end(); ++curSpot)
  	{
		printf("SupportSpotCalculator::DetermineBestSupportingPosition() 4\n");
    		//first remove any previous score. (the score is set to one so that
    		//the viewer can see the positions of all the spots if he has the 
    		//aids turned on)
    		curSpot->m_dScore = 1.0;
		printf("SupportSpotCalculator::DetermineBestSupportingPosition() 5\n");

    		//Test 1. is it possible to make a safe pass from the ball's position 
    		//to this position?
    		if(m_pTeam->isPassSafeFromAllOpponents(m_pTeam->ControllingPlayer()->Pos(),
                                           curSpot->m_vPos,
                                           NULL,
                                           m_pTeam->Pitch()->MaxPassingForce))
    		{
      			curSpot->m_dScore += m_pTeam->Pitch()->Spot_PassSafeScore;
    		}
      
		printf("SupportSpotCalculator::DetermineBestSupportingPosition() 6\n");
   
    		//Test 2. Determine if a goal can be scored from this position.  
    		printf("SupportSpotCalculator::DetermineBestSupportingPosition() 7\n");
    		if( m_pTeam->CanShoot(curSpot->m_vPos,            
                          m_pTeam->Pitch()->MaxShootingForce))
    		{
      			curSpot->m_dScore += m_pTeam->Pitch()->Spot_CanScoreFromPositionScore;
    		}	   
    		printf("SupportSpotCalculator::DetermineBestSupportingPosition() 8\n");

    
    		//Test 3. calculate how far this spot is away from the controlling
    		//player. The further away, the higher the score. Any distances further
    		//away than OptimalDistance pixels do not receive a score.
    		if (m_pTeam->SupportingPlayer())
    		{
      			const double OptimalDistance = 200.0;
        
      			double dist = Vec2DDistance(m_pTeam->ControllingPlayer()->Pos(),
                                 curSpot->m_vPos);
      
      			double temp = fabs(OptimalDistance - dist);

      			if (temp < OptimalDistance)
      			{

        			//normalize the distance and add it to the score
        			curSpot->m_dScore += m_pTeam->Pitch()->Spot_DistFromControllingPlayerScore *
                             		(OptimalDistance-temp)/OptimalDistance;  
      			}
    		}
    
    		//check to see if this spot has the highest score so far
    		if (curSpot->m_dScore > BestScoreSoFar)
    		{
      			BestScoreSoFar = curSpot->m_dScore;

      			m_pBestSupportingSpot = &(*curSpot);
    		}    
    
  	}
    	printf("SupportSpotCalculator::DetermineBestSupportingPosition() END\n");

	//printf("m_pBestSupportingSpot->m_vPos.x:%f m_pBestSupportingSpot->m_vPos.y:%f \n", m_pBestSupportingSpot->m_vPos.x, m_pBestSupportingSpot->m_vPos.y);
	if (m_pBestSupportingSpot)
	{
		printf("we have m_pBestSupportingSpot\n");
  		return m_pBestSupportingSpot->m_vPos;
	}
	else
	{
		Vector2D v;
		return v;
		printf("we do not have m_pBestSupportingSpot\n");
	}
}





//------------------------------- GetBestSupportingSpot -----------------------
//-----------------------------------------------------------------------------
Vector2D SupportSpotCalculator::GetBestSupportingSpot()
{
	printf("SupportSpotCalculator::GetBestSupportingSpot()\n");
  	if (m_pBestSupportingSpot)
  	{
		printf("SupportSpotCalculator::GetBestSupportingSpot() if\n");
    		return m_pBestSupportingSpot->m_vPos;
  	}
  	else
  	{	 
		printf("SupportSpotCalculator::GetBestSupportingSpot() else\n");
    		return DetermineBestSupportingPosition();
  	}
}

