#ifndef FIXED_POINT_H
#define FIXED_POINT_H 

/* Fixed point macros
 * Author: Bastiaan Oosterhuis
 *	
 */

/*
	Qm.n format
	m bits in total(32)
	n bits for the fraction part
	Resolution is 2^-Fract_part
*/
#define INT_PART	8	
#define FRACT_PART	8
#define FACTOR 		256	//2^FRACT_PART

/*
	Conversions:
*/

#define INT_TO_FIXED(A)   (((int)A) << FRACT_PART) 
#define FIXED_TO_INT(A)   ((int)(A >> 8))
#define FP_TO_FIXED(A) ((int)(A * FACTOR))  
#define FIXED_TO_FP(A) (((float)A) / FACTOR)   

/*
	Operations
*/
#define MULT_FIXED(A,B)  ((int)(A * B) >> FRACT_PART)
#define DIV_FIXED(A,B)  (int)((A << FRACT_PART)/B)


#endif
