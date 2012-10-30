#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Plane.h"
#include "BoundingSphere.h"

namespace COORDS
{
	enum 
	{
		TOP = 0, BOTTOM, LEFT, RIGHT, NEARP, FARP
	};
}

class Box
{
public:
	enum pos {NTL=0, NTR, NBL, NBR, FTL, FTR, FBL, FBR};

	Box(void);
	~Box(void);

	enum boxTest {OUTSIDE, INTERSECT, INSIDE};
	
	boxTest testPoint(glm::vec3& point);
	bool testLine(glm::vec3& p0, glm::vec3& p1);  //Returns true if line intersects/is in box
	boxTest testSphere(BoundingSphere& sphere);
	bool testBox(Box& box);

	//returns normalized length of vector's intersection with plane
	float rayPlaneIntersect(glm::vec4& p, glm::vec3& org, glm::vec3& dir );

	//Returns points
	//can be indexed with pos enum
	std::vector<glm::vec3> getPoints();

	void setPoint(pos position, glm::vec3& val)
	{
		switch (position)
		{
			case NTL:
				ntl = val;
				break;
			case NTR:
				ntr = val;
				break;
			case NBL:
				nbl = val;
				break;
			case NBR:
				nbr = val;
				break;
			case FTL:
				ftl = val;
				break;
			case FTR:
				ftr = val;
				break;
			case FBL:
				fbl = val;
				break;
			case FBR:
				fbr = val;
				break;
		}	
	}
	
	void calcPlanes();

protected:
	Plane pl[6];
	glm::vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
};