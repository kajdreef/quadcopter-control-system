#ifndef FIXED_POINT_H
#define FIXED_POINT_H 

/* Fixed point macros
 * Author: Bastiaan Oosterhuis
 *	
 */

#define INT_PART	22	
#define FRACT_PART	10
#define FACTOR 		1024	//2^FRACT_PART

/*
	 Conversions
*/

#define I2FDP(A)  		 	(((int)(A)) << FRACT_PART) 
#define I2FDP_S(A,SIZE) 	(((int)(A)) << SIZE) 

#define FDP2I(A)			((int)((A) + 512) >> FRACT_PART)//((int)(A + 256) >> FRACT_PART)
#define FDP2I_S(A,SIZE) 	((int)((A)+ (1<<(SIZE-1))) >> SIZE)//((int)(A + 256) >> SIZE)

#define FP2FDP(A) 			((int)((A) * FACTOR))  
#define FP2FDP_S(A,SIZE) 	((int)((A) * (1<<SIZE)))  

#define FDP2FP(A) 			(((float)(A)) / FACTOR)   
#define FDP2FP_S(A,SIZE) 	(((float)(A)) / (1<<SIZE)) 

/*
	Operations
*/
#define MULT(A,B)  			((int)((A) * (B)) >> FRACT_PART)
#define MULT_S(A,B,SIZE) 	((int)((A) * (B)) >> SIZE)

#define DIV(A,B)  			((int)(((long) (A) << FRACT_PART)/(B)))
#define DIV_S(A,B,SIZE)  	((int)(((long) (A) << SIZE)/(B)))


#endif
