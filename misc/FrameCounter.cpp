#include "FrameCounter.h"


FrameCounter* FrameCounter::Instance()
{
  static FrameCounter instance;

  return &instance;
}
