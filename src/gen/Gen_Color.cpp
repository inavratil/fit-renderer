#include"Gen_Color.h"

glm::vec3 color_HueToRGB(float Angle){
	glm::vec3 Color;
#define ASTEP 60.0
	float ActAngle;//actual angle for one component of the color
	for(int c=2;c>=0;--c){//cycles over component of the color in BGR order
		ActAngle=fmodf(Angle+(2-c)*ASTEP*2,360)/ASTEP;
		if(ActAngle>=5)Color[c]=6-ActAngle;
		else if(ActAngle>=3)Color[c]=1;
		else if(ActAngle>=2)Color[c]=ActAngle-2;
		else Color[c]=0;
	}
#undef ASTEP
	return Color;
}

