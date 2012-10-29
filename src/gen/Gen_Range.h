#ifndef _GEN_RANGE_H_
#define _GEN_RANGE_H_

enum ERandRangeType{
	RR_HOLD=0,
	RR_LINEAR,
	RR_HERMITE,
	RR_FIFTHDEGREEPOLY
};

class CRandRange{
	public:
		/**
		 * @brief Constructor of CRandRange
		 *
		 * @param Mean mean value
		 * @param Deviation deviation
		 * @param Type type of interpolation
		 */
		CRandRange(float Mean,float Deviation,enum ERandRangeType Type);
		/**
		 * @brief This function returns value in interval 
		 *
		 * @return value in interval [Mean-Deviation,Mean+Deviation]
		 */
		float Value();
	private:
		float Mean;/// mean value
		float Deviation;/// deviation
		float(*Fce)(float A);/// interpolation function
};

#endif//_GEN_RANGE_H_
