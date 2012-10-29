#ifndef _GEN_BLENDFCE_H_
#define _GEN_BLENDFCE_H_

/**
 * @brief Holds value at 0
 *
 * @param t useless param
 *
 * @return always return 0
 */
float blendfce_Hold(float t);

/**
 * @brief Returns 
 *
 * @param t input parameter in interval [0,1]
 *
 * @return Return param t
 */
float blendfce_Linear(float t);

/**
 * @brief Hermite blending function
 *
 * @param t input in interval [0,1]
 *
 * @return It returns 3t^2 - 2t^3
 */
float blendfce_Hermite(float t);

/**
 * @brief Fifth degree polynom blending funcion
 *
 * @param t input in interval [0,1]
 *
 * @return It returns 6t^5 - 15t^4 + 10t^3
 */
float blendfce_FifthDegreePoly(float t);

#endif//_GEN_BLENDFCE_H_
