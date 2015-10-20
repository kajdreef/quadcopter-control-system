#ifndef FIXED_POINT_H
#define FIXED_POINT_H 

/*------------------------------------------------------------------
 * fixed_point macros -- Contains both macros for variable size and 
 * macros for 10bit precision.
 *  	
 * Author: Bastiaan Oosterhuis
 *------------------------------------------------------------------
 */
#define INT_PART	22	
#define FRACT_PART	10
#define FACTOR 		1024	//2^FRACT_PART

/*
	 Conversions
*/
//Convert integer to fixed point
#define I2FDP(A)  		 	(((int)(A)) << FRACT_PART) 
#define I2FDP_S(A,SIZE) 	(((int)(A)) << SIZE) 

//convert fixed point to integer
#define FDP2I(A)			((int)((A) + 512) >> FRACT_PART)
#define FDP2I_S(A,SIZE) 	((int)((A)+ (1<<(SIZE-1))) >> SIZE)

//convert floating point to fixed point
#define FP2FDP(A) 			((int)((A) * FACTOR))  
#define FP2FDP_S(A,SIZE) 	((int)((A) * (1<<SIZE)))  

//convert fixed point to floating point
#define FDP2FP(A) 			(((float)(A)) / FACTOR)   
#define FDP2FP_S(A,SIZE) 	(((float)(A)) / (1<<SIZE)) 

/*
	Operations
*/
//multiplication
#define MULT(A,B)  			((int)((A) * (B)) >> FRACT_PART)
#define MULT_S(A,B,SIZE) 	((int)((A) * (B)) >> SIZE)

//division
#define DIV(A,B) 			 (int)(((A) << FRACT_PART)/(B))
#define DIV_S(A,B,SIZE)  	((int)(((A) << SIZE)/(B)))


#endif
