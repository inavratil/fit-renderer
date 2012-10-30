#include "BoundingVolume.h"


BoundingVolume::~BoundingVolume()
{
	if(vbo)
		glDeleteBuffers(1, &vbo);

	if(ebo)
		glDeleteBuffers(1, &ebo);

	if(vao)
		glDeleteVertexArrays(1, &vao);
}

//axis 2 is ignored, calculated as cross product of first two
//BoundingVolume::BoundingVolume(glm::vec3 center, glm::vec3 axis0, glm::vec3 axis1, glm::vec3 axis2, glm::vec3 extents)
BoundingVolume::BoundingVolume(float* vertices, int num)
{
	DiTO::Vector<float>* dVertices = new DiTO::Vector<float>[num];
	DiTO::OBB<float> o;

	//copy vertices into compatible structure
	for(unsigned curr_vert = 0; curr_vert < num; curr_vert++)
	{
		//vertices
		memcpy(&dVertices[curr_vert], vertices+(3*curr_vert), 3*sizeof(float));
	}

	DiTO::DiTO_14(dVertices, num, o);
	delete dVertices;

	glm::vec3 extents = glm::vec3(o.ext.x, o.ext.y, o.ext.z);
	glm::vec3 axis0 = glm::vec3(o.v0.x, o.v0.y, o.v0.z);
	glm::vec3 axis1 = glm::vec3(o.v1.x, o.v1.y, o.v1.z);
	glm::vec3 axis2;
	glm::vec3 center = glm::vec3(o.mid.x, o.mid.y, o.mid.z);

	if(extents.x<10e-05)
		extents.x = 10e-05;
	if(extents.y<10e-05)
		extents.y = 10e-05;
	if(extents.z<10e-05)
		extents.z = 10e-05;

	//generates points
	glm::vec3 points[8];
	float pts[24];
	//CHECK: nie naopak?
	//DANE NAOPAK!
	axis2 = glm::cross(axis0,axis1);
	
	//-x -y -z
	points[0] = center - (extents.x*axis0) - (extents.y*axis1) - (extents.z*axis2);
	obb.setPoint(Box::NBL, points[0]);
	memcpy(pts, glm::value_ptr(points[0]), 3*sizeof(float));

	//+x -y -z
	points[1] = center + (extents.x*axis0) - (extents.y*axis1) - (extents.z*axis2);
	obb.setPoint(Box::NBR, points[1]);
	memcpy(pts+3, glm::value_ptr(points[1]), 3*sizeof(float));
	
	//-x +y -z
	points[2] = center - (extents.x*axis0) + (extents.y*axis1) - (extents.z*axis2);
	obb.setPoint(Box::NTL, points[2]);
	memcpy(pts+6, glm::value_ptr(points[2]), 3*sizeof(float));

	//+x +y -z
	points[3] = center + (extents.x*axis0) + (extents.y*axis1) - (extents.z*axis2);
	obb.setPoint(Box::NTR, points[3]);
	memcpy(pts+9, glm::value_ptr(points[3]), 3*sizeof(float));

	//-x -y +z
	points[4] = center - (extents.x*axis0) - (extents.y*axis1) + (extents.z*axis2);
	obb.setPoint(Box::FBL, points[4]);
	memcpy(pts+12, glm::value_ptr(points[4]), 3*sizeof(float));

	//+x -y +z
	points[5] = center + (extents.x*axis0) - (extents.y*axis1) + (extents.z*axis2);
	obb.setPoint(Box::FBR, points[5]);
	memcpy(pts+15, glm::value_ptr(points[5]), 3*sizeof(float));

	//-x +y +z
	points[6] = center - (extents.x*axis0) + (extents.y*axis1) + (extents.z*axis2);
	obb.setPoint(Box::FTL, points[6]);
	memcpy(pts+18, glm::value_ptr(points[6]), 3*sizeof(float));

	//+x +y +z
	points[7] = center + (extents.x*axis0) + (extents.y*axis1) + (extents.z*axis2);
	obb.setPoint(Box::FTR, points[7]);
	memcpy(pts+21, glm::value_ptr(points[7]), 3*sizeof(float));

	//Calculates planes
	obb.calcPlanes();

	//Indices
	GLubyte indices[36] = {
		0, 3, 1, //front
		0, 2, 3,
		4, 5, 7, //back
		4, 7, 6,
		7, 5, 1,//right
		7, 1, 3,
		4, 2, 0, //left
		4, 6, 2,
		2, 7, 3, //top
		2, 6, 7,
		0, 1, 5, //bottom
		0, 5, 4
	};

	vao = 0;
	vbo = 0;
	ebo = 0;
	
	//Create VBO, EBO, VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24* sizeof(float), pts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLubyte), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}


Box BoundingVolume::getOBB()
{
	return obb;
}

void BoundingVolume::drawBV()
{
	if(vao)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, NULL);
		glBindVertexArray(0);
	}
}