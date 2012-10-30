#pragma once

#include <glm/glm.hpp>

/*
 * Trieda BoundingSphere
 */
class BoundingSphere
{
public:
	BoundingSphere(void);
	~BoundingSphere(void);

	void setRadius(float r)
	{
		radius = r;
	}

	void setCenter(glm::vec3 c)
	{
		center = c;
	}

	float getRadius()
	{
		return radius;
	}

	glm::vec3 getCenter()
	{
		return center;
	}

private:
	float		radius;
	glm::vec3	center;
};

