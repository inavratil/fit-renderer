#include"Gen_Random.h"

#define MYRAND_MAX 2147483647
unsigned myrandom_seed=1;

void gen_Seed(unsigned Seed){
	myrandom_seed=Seed;
}

/*
 * http://ftp.gnu.org/gnu/glibc/glibc-2.9.tar.gz
 * stdlib/rand_r.c - int rand_r(unsigned int*seed)
 * @return vraci pseoudonahodne cislo
 */
int randrand (){
	unsigned int next = myrandom_seed;
	int result;
	
	next *= 1103515245;
	next += 12345;
	result = (unsigned int) (next / 65536) % 2048;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (unsigned int) (next / 65536) % 1024;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (unsigned int) (next / 65536) % 1024;

	myrandom_seed = next;

	return result;
}


float gen_Uniform(float Min,float Max){
	return Min+(Max-Min)*randrand()/(float)MYRAND_MAX;
}

