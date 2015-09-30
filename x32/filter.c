#include "filter.h"
#include "fixed_point.h"
#include "supervisor.h"

int filtered_phi = 0;
int filtered_thet = 0;
int filtered_p = 0;
int filtered_q = 0;
int filtered_r = 0;
int calibrated = 0;
extern int isr_filter_time;
extern int battery_voltage;
extern enum QR mode;

/*
 * A second order butterworth filter
 * xy is an array [x(n) x(n-1) x(n-2) y(n) y(n-1) y(n-2)],
 *  at the beginning the array is the same as the previous method left it
 *
 * Author: Gijs Bruining
 */
int BF_2nd(int x,int *xy, Filt_Param *Filt){
	// Shift the x values
	xy[2] = xy[1];
	xy[1] = xy[0];
	xy[0] = x;

	//Shift the y values and calculate the new value for y(n)
	xy[5] = xy[4];
	xy[4] = xy[3];
	xy[3] = MULT_FIXED(Filt->a0,xy[0]) + MULT_FIXED(Filt->a1,xy[1]) + MULT_FIXED(Filt->a2,xy[2]) - MULT_FIXED(Filt->b1,xy[4]) - MULT_FIXED(Filt->b2,xy[5]);

	return xy[3];
}

int F_1st(int x, int prev_out, Filt_Param *Filt){
	//Shift the y values and calculate the new value for y(n)
	int y = prev_out + MULT_FIXED(Filt->alph,(x-prev_out));

	return y; 
}

int rem_absurd_val(int x, int prev_x, Filt_Param *Filt){
	if((x < Filt->min) | (x > Filt->max))
		return prev_x;

	return x;
}


/*
 * 
 *
 * 		int sphi 			:	Measured angle
 *		int sp 				:	Measured angle rate
 *		int *bias 			:	Signal bias (from previous calculation)
 *		int *phi    		:	Resulting filtered angle
 *		int *p 				:	Resulting filtered angle rate
 *		Filt_Param *Filt 	:	Filter parameters
 */ 
void kalman(int sphi, int sp, int *bias, int *phi, int *p, Filt_Param *Filt){
	int e;
	*p = sp - *bias;
	*phi = *phi + MULT_FIXED(*p,Filt->dt);
	e = *phi-sphi;
	*phi = *phi-DIV_FIXED(e,Filt->C1);
	*bias = *bias + DIV_FIXED(e,Filt->C2);
}

void setup_sensor_interrupts(int prio){
	SET_INTERRUPT_VECTOR(INTERRUPT_XUFO, &isr_sensor);
	SET_INTERRUPT_PRIORITY(INTERRUPT_XUFO, prio);
	ENABLE_INTERRUPT(INTERRUPT_XUFO);
}

void isr_sensor(){
	int old = X32_clock_us;

	static int phi=0;	// Roll, fixed point
	static int thet=0;	// Pitch, fixed point
	static int p = 0; 	// Roll speed, fixed point
	static int q = 0;	// Pitch speed, fixed point
	static int r = 0;	// Yaw speed, fixed point

	static int xy_phi[6]  = {0,0,0,0,0,0};
	static int xy_thet[6] = {0,0,0,0,0,0};
	static int prev_x_p = 0;
	static int prev_x_q = 0;
	static int prev_x_r = 0;
	static int bias_phi = 0;
	static int bias_thet = 0;
	static int prev_lp_r = 0;

	static int phi_bias = 0;
	static int thet_bias= 0;

	static int r_lp =0;

	battery_voltage = X32_QR_S6;

	switch(mode){
		case CALIBRATION:
			// Get the latest and greatest sensor values AND remove absurd values
			r = rem_absurd_val( INT_TO_FIXED(X32_QR_S5) ,prev_x_r,&Filt_r); 

			// Anti-drift the gyro values
			r_lp = F_1st(r, prev_lp_r, &Filt_r);
			calibrated = r>(r_lp-16) && r<(r_lp+16);
			prev_lp_r = r_lp;

			
			break;

		case YAW_CONTROL:
			// Get the latest and greatest sensor values AND remove absurd values
			r = rem_absurd_val( INT_TO_FIXED(X32_QR_S5) ,prev_x_r,&Filt_r); 

			// Anti-drift the gyro values
			r_lp = F_1st(r, prev_lp_r, &Filt_r);
			prev_lp_r = r_lp;
			filtered_r = r - r_lp;

			break;


		case FULL_CONTROL:
			// Get the latest and greatest sensor values AND remove absurd values
			phi = rem_absurd_val( INT_TO_FIXED(X32_QR_S0) ,xy_phi[0],&Filt_phi);
			thet = rem_absurd_val( INT_TO_FIXED(X32_QR_S1) ,xy_thet[0],&Filt_thet);
			p = rem_absurd_val( INT_TO_FIXED(X32_QR_S3) ,prev_x_p,&Filt_phi); // NOT COMPLETE!! Filter params
			q = rem_absurd_val( INT_TO_FIXED(X32_QR_S4) ,prev_x_q,&Filt_thet); // NOT COMPLETE!! Filter params
			r = rem_absurd_val( INT_TO_FIXED(X32_QR_S5) ,prev_x_r,&Filt_r); // NOT COMPLETE!! Filter params

			// Filter the accelerometer values
			phi = BF_2nd(phi,xy_phi,&Filt_phi);
			thet = BF_2nd(thet,xy_thet,&Filt_thet);

			// Anti-drift the gyro values
			

			// Kalman filter OWYEAH
			kalman(phi,p, &phi_bias,&filtered_phi,&filtered_p,&Filt_phi);
			kalman(thet,q,&thet_bias,&filtered_thet,&filtered_q,&Filt_thet);

			break;

	}
	prev_x_p = p;
	prev_x_q = q;
	prev_x_r = r;

	isr_filter_time = X32_clock_us - old;
	
 
}
