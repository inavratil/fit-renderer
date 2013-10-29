#ifndef _CAMERAANIMATION_H_
#define _CAMERAANIMATION_H_

#include "globals.h"

/*
 * Trieda TCam
 * Instancia kamery, z ktorej je mozne vyrobit View maticu
 */
class TCam
{
public:
	TCam() {}
	~TCam(){}

	glm::vec3 pos;
	glm::vec3 up;
	glm::vec3 lookAT;

	void buildView(glm::vec3 p, glm::vec3 l, glm::vec3 u)
	{
		pos = p;
		lookAT = l;
		up = u;

		mView = glm::lookAt( pos, pos+lookAT, up );
	}

	inline glm::mat4 getView()
	{
		return mView;
	}

private:
	glm::mat4 mView;
};

/*
 * Trieda Cubic
 * Predpis kubickeho polynomu
 */
class Cubic
{
private: glm::vec3 A, B, C, D; // a + b*s + c*s^2 +d*s^3 

public: 
	Cubic(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
	{
		A = a;
		B = b;
		C = c;
		D = d;
	}

	//Na zaklade polohy na spline vrati suradnicu bodu
	glm::vec3 GetPointOnSpline(float s)
	{	
		return (((D * s) + C) * s + B) * s + A;
	}
};

/*
 * Trieda CameraAnimation
 * Samotna animovana kamera
 * Na zaklade mnoziny kontrolnych bodov vyrobi metodou BuildPath vektor TCam, z ktoreho sa vyberaju instancie
 * podla uplynuteho casu animacie
 */
class CameraAnimation
{
public:
	CameraAnimation(void);
	~CameraAnimation(void);

	void BuildPath(std::vector<TCam> keyFrames);
	void Update(unsigned int miliseconds);
	TCam getCurrentCam();
	
	std::vector<TCam> getPath()
	{
		return camPath;
	}

private:
	std::vector<TCam> camPath;
	unsigned int pathSteps;
	unsigned int pathIndex;
	std::vector<Cubic> calculateCubicSpline(int n, glm::vec3* v);
};

#endif

