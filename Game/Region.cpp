#include "Game/Region.h"

Region::Region()
{
	mTop = 0;
	mRight = 0;
	mBottom = 0;
}	

Region::Region(double left, double top, double right, double bottom)
{

	mTop = top;
	mRight = right;
	mBottom = bottom;
	//mId = id;

	//calculate center of region
        mCenter = Vector2D( (left+right)*0.5, (top+bottom)*0.5 );
        mWidth  = fabs(right-left);
        mHeight = fabs(bottom-top);
}

Region::Region(double left, double top, double right, double bottom, int id)
{

	mTop = top;
	mRight = right;
	mBottom = bottom;
	mId = id;

	//calculate center of region
        mCenter = Vector2D( (left+right)*0.5, (top+bottom)*0.5 );
        mWidth  = fabs(right-left);
        mHeight = fabs(bottom-top);
}
