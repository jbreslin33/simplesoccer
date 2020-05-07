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

  		enum region_modifier{halfsize, normal};
  
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


  		Region();
		Region(double left, double top, double right, double bottom);
		Region(double left, double top, double right, double bottom, int id);

		virtual ~Region(){}

  		//returns true if the given position lays inside the region. The
  		//region modifier can be used to contract the region bounderies
  		bool     getIsInside(Vector2D pos, int size)const;

  		//returns a vector representing a random location
  		//within the region
  		inline Vector2D GetRandomPosition()const;

  		//-------------------------------
  		double     Top()const{return mTop;}
  		double     Bottom()const{return mBottom;}
		double     Left()const
  		{
			return mLeft;
		}
  
		double     Right()const{return mRight;}
  		double     Width()const{return fabs(mRight - mLeft);}
  		double     Height()const{return fabs(mTop - mBottom);}
  		double     Length()const{return std::max(Width(), Height());}
  		double     Breadth()const{return std::min(Width(), Height());}

  		Vector2D  Center()const{return mCenter;}
  		int       ID()const{return mId;}
		Vector2D getRandomPosition()const;

};



#endif
