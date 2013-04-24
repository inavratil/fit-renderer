#include "StringUtil.h"

//-----------------------------------------------------------------------------

int StringUtil::ParseInt(const string& val, int defaultValue)
{
	stringstream str(val);
	int ret = defaultValue;
	if( !(str >> ret) )
		return defaultValue;

	return ret;
}

//-----------------------------------------------------------------------------

float StringUtil::ParseFloat(const string& val, float defaultValue)
{

	// Use istringstream for direct correspondence with toString
	stringstream str(val);
	float ret = defaultValue;
	if( !(str >> ret) )
		return defaultValue;
	return ret;
}

//-----------------------------------------------------------------------------

glm::vec3 StringUtil::ParseVector3(const string& val, const glm::vec3& defaultValue)
{
	// Split on space
	vector<string> vec = StringUtil::Split(val);

	if (vec.size() != 3)
	{
		return defaultValue;
	}
	else
	{
		return glm::vec3(ParseFloat(vec[0]),ParseFloat(vec[1]),ParseFloat(vec[2]));
	}
}
//-----------------------------------------------------------------------
glm::vec4 StringUtil::ParseVector4(const string& val, const glm::vec4& defaultValue)
{
	// Split on space
	vector<string> vec = StringUtil::Split(val);

	if (vec.size() != 4)
	{
		return defaultValue;
	}
	else
	{
		return glm::vec4(ParseFloat(vec[0]),ParseFloat(vec[1]),ParseFloat(vec[2]),ParseFloat(vec[3]));
	}
}

//-----------------------------------------------------------------------------

vector<string> StringUtil::Split( const string& str, const string& delims, unsigned int maxSplits, bool preserveDelims)
    {
        vector<string> ret;
        // Pre-allocate some space for performance
        ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

        unsigned int numSplits = 0;

        // Use STL methods 
        size_t start, pos;
        start = 0;
        do 
        {
            pos = str.find_first_of(delims, start);
            if (pos == start)
            {
                // Do nothing
                start = pos + 1;
            }
            else if (pos == string::npos || (maxSplits && numSplits == maxSplits))
            {
                // Copy the rest of the string
                ret.push_back( str.substr(start) );
                break;
            }
            else
            {
                // Copy up to delimiter
                ret.push_back( str.substr(start, pos - start) );

                if(preserveDelims)
                {
                    // Sometimes there could be more than one delimiter in a row.
                    // Loop until we don't find any more delims
                    size_t delimStart = pos, delimPos;
                    delimPos = str.find_first_not_of(delims, delimStart);
                    if (delimPos == string::npos)
                    {
                        // Copy the rest of the string
                        ret.push_back( str.substr(delimStart) );
                    }
                    else
                    {
                        ret.push_back( str.substr(delimStart, delimPos - delimStart) );
                    }
                }

                start = pos + 1;
            }
            // parse up to next real data
            start = str.find_first_not_of(delims, start);
            ++numSplits;

        } while (pos != string::npos);



        return ret;
    }
