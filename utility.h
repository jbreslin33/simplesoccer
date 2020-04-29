#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>

class Vector2D;

class Utility
{
	public:
		Utility();
		std::vector<std::string> split(const std::string str, const std::string delim);
		double getRotationInDegreesFromVector(Vector2D v);

};

#endif

