#include <stdio.h>
#include "fixed_point.h"

int main(int argc, char* argv[])
{
	float fp,g;
	int fixed_fp,fixed_g,fixed_h;
	int h;
	printf("Fixed point test program\n");
	
	printf("Enter a floating point value\n");

	scanf("%f", &fp);
		
	printf("Fixed point representation: %d\n", fixed_fp = FP_TO_FIXED(fp));
	
	printf("Enter an float and int\n");
	scanf("%f%d", &g,&h);
	
	fixed_g = FP_TO_FIXED(g);
	fixed_h = INT_TO_FIXED(h);

	printf("Results after fixed point addition:\n");
	printf("%f + %f = %f\n%f + %d = %f\n", FIXED_TO_FP(fixed_fp), FIXED_TO_FP(fixed_g), FIXED_TO_FP(fixed_fp+fixed_g),FIXED_TO_FP(fixed_fp), FIXED_TO_INT(fixed_h), FIXED_TO_FP(fixed_fp+fixed_h));

	printf("Results after fixed point subtraction:\n");
	printf("%f - %f = %f\n%f - %d = %f\n", fp, g, FIXED_TO_FP(fixed_fp-fixed_g),fp, h, FIXED_TO_FP(fixed_fp-fixed_h));
	
	printf("Result after fixed point multiplication:\n");
	printf("%f x %f = %f\n%f x %d = %f\n", fp,g,FIXED_TO_FP(MULT_FIXED(fixed_fp,fixed_g)),fp, h,FIXED_TO_FP(MULT_FIXED(fixed_fp,fixed_h)));	
			
	printf("Result after fixed point division:\n");	
	printf("%f \\ %f = %f\n%f \\ %d = %f\n", fp,g,FIXED_TO_FP(DIV_FIXED(fixed_fp,fixed_g)),fp, h,FIXED_TO_FP(DIV_FIXED(fixed_fp,fixed_h)));	
			
	printf("Result after fixed point division:\n");	



	return 0;


}
