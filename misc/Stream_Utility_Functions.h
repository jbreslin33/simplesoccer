#ifndef STREAM_UTILITY_FUNCTIONS
#define STREAM_UTILITY_FUNCTIONS
//-----------------------------------------------------------------------------
//
//  Name:   Stream_Utility_Functions.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//  Desc:   various useful functions that operate on or with streams
//-----------------------------------------------------------------------------
#include <sstream>
#include <string>
#include <iomanip>


//------------------------------ ttos -----------------------------------------
//
//  convert a type to a string
//-----------------------------------------------------------------------------
template <class T>
inline std::string ttos(const T& t, int precision = 2)
{
  std::ostringstream buffer;

  buffer << std::fixed << std::setprecision(precision) << t;

  return buffer.str();
}

//------------------------------ ttos -----------------------------------------
//
//  convert a bool to a string
//-----------------------------------------------------------------------------
inline std::string btos(bool b)
{
  if (b) return "true";
  return "false";
}

//--------------------------- WriteBitsToStream ------------------------------------
//
// writes the value as a binary string of bits
//-----------------------------------------------------------------------------
template <typename T>
void WriteBitsToStream(std::ostream& stream, const T& val)
{
  int iNumBits = sizeof(T) * 8;

  while (--iNumBits >= 0)
  {
    if ((iNumBits+1) % 8 == 0) stream << " ";
    unsigned long mask = 1 << iNumBits;
    if (val & mask) stream << "1";
    else stream << "0";
  }
}



#endif
