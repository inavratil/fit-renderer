#include "ViewFrustum.h"


ViewFrustum::ViewFrustum(void) : Box()
{
}


ViewFrustum::~ViewFrustum(void)
{
}

void ViewFrustum::setCamParams(float _angle, float _ratio, float nearClipDist, float farClipDist, glm::vec3& pos, glm::vec3& lookAt, glm::vec3& up)
{
	ratio = _ratio;
	angle = _angle;
	nearClip = nearClipDist;
	farClip = farClipDist;

	//Vyska a sirka orezovych rovin
	float tang = (float)tan(angle * 0.5) ;
	nearHeight = nearClip * tang;
	nearWidth = nearHeight * ratio;
	farHeight = farClip  * tang;
	farWidth = farHeight * ratio;

	glm::vec3 X, Y, Z, nearClipCenter, farClipCenter;
	
	//Base vectors calculation
	//Z
	Z = glm::normalize(pos - lookAt);

	// X = UP x Z
	X = glm::normalize(glm::cross(up, Z));//upV * ZV;

	//Y = Z x X
	Y = glm::normalize(glm::cross(Z, X));

	//Vypocet stredov near/far plane
	nearClipCenter = pos - (Z * nearClip);
	farClipCenter = pos - (Z * farClip);

	//Vypocet rohov VF - near plane
	ntl = nearClipCenter + (Y * nearHeight) - (X * nearWidth);
	ntr = nearClipCenter + (Y * nearHeight) + (X * nearWidth);
	nbl = nearClipCenter - (Y * nearHeight) - (X * nearWidth);
	nbr = nearClipCenter - (Y * nearHeight) + (X * nearWidth);

	//Far plane
	ftl = farClipCenter + (Y * farHeight) - (X * farWidth);
	ftr = farClipCenter + (Y * farHeight) + (X * farWidth);
	fbl = farClipCenter - (Y * farHeight) - (X * farWidth);
	fbr = farClipCenter - (Y * farHeight) + (X * farWidth);

	//Vypocet rovin
	calcPlanes();
}
