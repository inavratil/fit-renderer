#include"Gen_Range.h"
#include"Gen_BlendFce.h"
#include"Gen_Random.h"

CRandRange::CRandRange(float Mean,float Deviation,enum ERandRangeType Type){
	this->Mean=Mean;
	this->Deviation=Deviation;
	switch(Type){
		case RR_HOLD:
			this->Fce=blendfce_Hold;
			break;
		case RR_LINEAR:
			this->Fce=blendfce_Linear;
			break;
		case RR_HERMITE:
			this->Fce=blendfce_Hermite;
			break;
		case RR_FIFTHDEGREEPOLY:
			this->Fce=blendfce_FifthDegreePoly;
			break;
	}
}

float CRandRange::Value(){
	return this->Mean+this->Deviation*(2*this->Fce(gen_Uniform(0,1))-1);
}

