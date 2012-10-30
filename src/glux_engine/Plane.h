#pragma once

#include <glm/glm.hpp>

class Plane
{
public:

	static enum{INTERSECT, NO_INTERSECT};

	Plane()
	{
		A=B=C=D = 0.0f;
	}

	void setPoints(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2)
	{
		glm::vec3 u,v, n;

		v = p1-p0;
		u = p2-p0;

		n = glm::normalize(glm::cross(v, u));

		A = n.x;
		B = n.y;
		C = n.z;
		D = -1.0f * (A * p0.x + B*p0.y + C*p0.z);
	}

	void setNormalPoint(glm::vec3& normal, glm::vec3& point)
	{
		A = normal.x;
		B = normal.y;
		C = normal.z;
		D = -1.0f * (A*point.x + B*point.y + C*point.z);
	}

	//Vypocita vzdialenost bodu od roviny
	inline float distancePoint(glm::vec3& point)
	{
		return A*point.x + B*point.y + C*point.z + D; 
	}

	//Vypocita, ci usecka zadana 2-ma bodmi pretina rovinu
	//V praxi - oba body musia lezat na opacnej strane roviny - jeden z bodov ma zapornu vzdialenost, druhy kladnu
	int testLine(glm::vec3& p0, glm::vec3& p1)
	{
		float d0 = distancePoint(p0);
		float d1 = distancePoint(p1);

		if((d0*d1)<=0)
			return INTERSECT;
		else
			return NO_INTERSECT;
	}
	
	glm::vec4 getPlane()
	{
		return glm::vec4(A, B, C, D);
	}


private:
	//Parametre rovnice roviny
	float A, B, C, D;
};