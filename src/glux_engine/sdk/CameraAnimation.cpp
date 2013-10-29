#include "CameraAnimation.h"


CameraAnimation::CameraAnimation(void)
{
	pathSteps = 2000;
	pathIndex = 0;
}


CameraAnimation::~CameraAnimation(void)
{
}

//Podla kubickeho splinu
void CameraAnimation::BuildPath(std::vector<TCam> keyFrames)
{
	//Zostavenie sustavy rovnic
	glm::vec3* looks = new glm::vec3[keyFrames.size()];
	glm::vec3* positions = new glm::vec3[keyFrames.size()];
	glm::vec3* ups = new glm::vec3[keyFrames.size()];

	for (unsigned int i = 0; i < keyFrames.size(); i++)
	{
		looks[i] = keyFrames[i].lookAT;
		positions[i] = keyFrames[i].pos;
		ups[i] = keyFrames[i].up;
	}

	std::vector<Cubic> pos_cubic = calculateCubicSpline(keyFrames.size() - 1, positions);
	std::vector<Cubic> look_cubic = calculateCubicSpline(keyFrames.size() - 1, looks);
	std::vector<Cubic> up_cubic = calculateCubicSpline(keyFrames.size() - 1, ups);

	for (unsigned int i = 0; i < keyFrames.size() - 1; i++)
	{
		for (unsigned int j = 0; j < pathSteps; j++)
		{
			float k = (float)j / (float)(pathSteps - 1);

			glm::vec3 center, up, look;
			center = pos_cubic[i].GetPointOnSpline(k);
			up = up_cubic[i].GetPointOnSpline(k);
			look = look_cubic[i].GetPointOnSpline(k);

			TCam cam;
			cam.buildView(center, look, up);

			camPath.push_back(cam);
		}
	}

	delete looks;
	delete positions;
	delete ups;
}

std::vector<Cubic> CameraAnimation::calculateCubicSpline(int n, glm::vec3* v)
{
	std::vector<glm::vec3> gamma;
	std::vector<glm::vec3> delta;
	std::vector<glm::vec3> D;

	glm::vec3 tmp;
	glm::vec3 tV;
	/*
	std::vector<XMVECTOR> v;
	for(int i=0; i<n; ++i)
	{
		XMVECTOR tmp;
		tmp = XMLoadFloat3(&vF[i]);
		v.push_back(tmp);
	}*/
	int i;
	/* Riesime:
	 * Prebrate z : http://mathworld.wolfram.com/CubicSpline.html
	   [2 1       ] [D[0]]   [3(v[1] - v[0])  ]
	   |1 4 1     | |D[1]|   |3(v[2] - v[0])  |
	   |  1 4 1   | | .  | = |      .         |
	   |    ..... | | .  |   |      .         |
	   |     1 4 1| | .  |   |3(v[n] - v[n-2])|
	   [       1 2] [D[n]]   [3(v[n] - v[n-1])]

	   D[i]  - derivacie v kontrolnych bodoch
	*/

	//koeficienty lavej matice
	gamma.push_back(glm::vec3(1.0f/2.0f));

	glm::vec3 one = glm::vec3(1.0f);

	for (i = 1; i < n; i++)
	{
		glm::vec3 t;
		t = one / ((4.0f * one) - gamma[i - 1]);
		gamma.push_back(t);
	}

	tmp =  one / ((2.0f * one) - gamma[n - 1]);
	gamma.push_back(tmp);

	tmp = 3.0f * (v[1] - v[0]) * gamma[0];
	delta.clear();
	delta.push_back(tmp);

	for (i = 1; i < n; i++)
	{
		tmp = (3.0f * (v[i + 1] - v[i - 1]) - delta[i - 1]) * gamma[i];
		delta.push_back(tmp);
	}

	tmp = (3.0f * (v[n] - v[n - 1]) - delta[n - 1]) * gamma[n];
	delta.push_back(tmp);

	//--
	//Dirty hack
	for(int q = 0; q<=n; ++q)
		D.push_back(glm::vec3(0.0f));
	//--

	D[n] = delta[n];
	for (i = n - 1; i >= 0; i--)
	{
		tmp = delta[i] - gamma[i] * D[i + 1];
		D[i] = tmp;
	}

	// vypocet koeficientov kubik
	std::vector<Cubic> C;
	glm::vec3 a,b,c,d;
	for (i = 0; i < n; i++)
	{
		a = v[i];
		b = D[i];
		c = 3.0f * (v[i + 1] - v[i]) - 2.0f * D[i] - D[i + 1];
		d = 2.0f * (v[i] - v[i + 1]) + D[i] + D[i + 1];

		C.push_back(Cubic(a,b,c,d));
	}

	return C;
}

void CameraAnimation::Update(unsigned int miliseconds)
{
	//pathIndex += miliseconds;
	pathIndex = miliseconds;
}

TCam CameraAnimation::getCurrentCam()
{
	if(pathIndex >= camPath.size())
		return camPath[camPath.size()-1];
	else
		return camPath[pathIndex];
}
