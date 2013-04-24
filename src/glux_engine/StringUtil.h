#ifndef _STRINGUTIL_H_
#define _STRINGUTIL_H_

#include "globals.h"

class StringUtil
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	static int			ParseInt(const string& val, int defaultValue = 0);
	static float		ParseFloat(const string& val, float defaultValue = 0.0f );
	static glm::vec3	ParseVector3(const string& val, const glm::vec3& defaultValue = glm::vec3(0) );
	static glm::vec4	ParseVector4(const string& val, const glm::vec4& defaultValue = glm::vec4(0) );

	static vector<string> Split( const string& str, const string& delims = "\t\n ", unsigned int maxSplits = 0, bool preserveDelims = false);

};

#endif

