#include "utility.h"
#include "2D/Vector2D.h"

Utility::Utility()
{

}

std::vector<std::string> Utility::split(std::string s, std::string delim)
{
        std::vector<std::string> stringVector;

        size_t pos = 0;
        std::string token;
       	while ((pos = s.find(delim)) != std::string::npos)
       	{
       		token = s.substr(0, pos);
                stringVector.push_back(token);
                s.erase(0, pos + delim.length());
                //printf("e:%s\n",token.c_str());
	}

	return stringVector;
}

