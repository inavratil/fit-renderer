#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Box.h"

//ViewFrustum
class ViewFrustum : public Box
{
public:

	ViewFrustum(void);
	~ViewFrustum(void);

private:

	glm::vec3 position;
	glm::vec3 direction;
	float nearClip;
	float farClip;
	float angle, ratio;
	float nearWidth, nearHeight, farWidth, farHeight;

public:
	void setCamParams(float angle, float ratio, float nearClipDist, float farClipDist, glm::vec3& pos, glm::vec3& focusPoint, glm::vec3& up);
};

