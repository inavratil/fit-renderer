#pragma once

#include "globals.h"
#include "Box.h"
#include "dito.h"


//TODO - shader pre kreslenie staticky!

class BoundingVolume
{
public:	
	//BoundingVolume(glm::vec3 center, glm::vec3 axis0, glm::vec3 axis1, glm::vec3 axis2, glm::vec3 extents);
	BoundingVolume(float* vertices, int num);
	~BoundingVolume();
	Box getOBB();
	void drawBV();

private:
	Box obb;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};