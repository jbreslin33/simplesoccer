#ifndef REGION_H
#define REGION_H
//------------------------------------------------------------------------
//
//  Name:   Region.h
//
//  Desc:   Defines a rectangular region. A region has an identifying
//          number, and four corners.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------

#include <math.h>

#include "2D/Vector2D.h"
#include "misc/utils.h"
#include "misc/Stream_Utility_Functions.h"
#include <algorithm>

class Region
{
	public:
  		Region();
		Region(double left, double top, double right, double bottom);
		Region(double left, double top, double right, double bottom, int id);

		virtual ~Region(){}
	protected:

  		double        mTop;
  		double        mLeft;
  		double        mRight;
  		double        mBottom;

  		double        mWidth;
  		double        mHeight;

  		Vector2D     mCenter;
  
  		int          mId;

	public:

  		//returns true if the given position lays inside the region. The
  		//region modifier can be used to contract the region bounderies
  		bool     getIsInside(Vector2D pos, int size)const;

  		//returns a vector representing a random location
  		//within the region
  		inline Vector2D GetRandomPosition()const;

  		//-------------------------------
  		double     getTop()const;
  		double     getBottom()const;
		double     getLeft()const;
  
		double     getRight()const;
  		double     getWidth()const;
  		double     getHeight()const;
  		double     getLength()const;
  		double     getBreadth()const;

  		Vector2D  getCenter()const;
  		int       getId()const;
		Vector2D getRandomPosition()const;
};

#endif
