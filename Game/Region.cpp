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

Vector2D Region::getRandomPosition()const
{
        return Vector2D(RandInRange(mLeft, mRight),
                   RandInRange(mTop, mBottom));
}

bool Region::getIsInside(Vector2D pos, int size)const
{
	//half size
        if (size == 0)
        {
                const double marginX = mWidth * 0.25;
                const double marginY = mHeight * 0.25;

                return ((pos.x > (mLeft+marginX)) && (pos.x < (mRight-marginX)) &&
                        (pos.y > (mTop+marginY)) && (pos.y < (mBottom-marginY)));
        }
        else 
        {
                return ((pos.x > mLeft) && (pos.x < mRight) &&
                (pos.y > mTop) && (pos.y < mBottom));
        }
}


