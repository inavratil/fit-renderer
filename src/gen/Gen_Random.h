#ifndef _GEN_RANDOM_H_
#define _GEN_RANDOM_H_

/**
 * @brief This function generates random number
 *
 * @param Min Minimum number
 * @param Max Maximum number
 *
 * @return pseudo-random number with uniform distribution in interval [Min,Max]
 */
float gen_Uniform(float Min,float Max);

/**
 * @brief This function sets seed for myrandom_Uniform function
 *
 * @param Seed seed
 */
void gen_Seed(unsigned Seed);


#endif//_GEN_RANDOM_H_
