#include"Gen_BlendFce.h"

float blendfce_Hold(float){
	return 0;
}

float blendfce_Linear(float t){
	return t;
}

float blendfce_Hermite(float t){
	return t*t*(3-2*t);
}

float blendfce_FifthDegreePoly(float t){
	return t*t*t*(10+t*(-15+6*t));
}

