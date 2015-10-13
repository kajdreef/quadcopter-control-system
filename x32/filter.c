#include "filter.h"
#include "fixed_point.h"
#include "supervisor.h"
#include "logger.h"
#include "config.h"

#define dXs 	0	// Sensed Rate
#define dXb		1	// Rate bias
#define dXk		2  	// Rate Kalman fitlered
#define Xs 		3	// Sensed position
#define Xb		4	// Bias
#define Xk		5	// Kalman filtered

/*
	Flow diagram:

    ---------       ---------       ---------                       ---------
    |       |   dXm |       |   dXa |       |           dXaz        |       |   dXk
    |Giro   |	->  |Absurd |	->  |  AD   |   ->      ->      ->  |       |   ->
    |       |       |       |       |       |                       |       |
    ---------       ---------       ---------                       |       |
                                                                    |Kalman |
                                                                    |       |
    ---------       ---------       ---------       ---------       |       |
    |       |   Xm  |       |   Xa  |       |   Xad |       |   Xbf |       |   Xk
    |Acc    |   ->  |Absurd |   ->  |   AD  |   ->  |   BF  |   ->  |       |   ->
    |       |       |       |       |       |       |       |       |       |
    ---------       ---------       ---------       ---------       ---------

*/

int filtered_phi = 0; //Roll
int filtered_theta = 0;	//Pitch
int filtered_p = 0;	//Roll rate
int filtered_q = 0;	//Pitch rate
int filtered_r = 0;	//Yaw rate
int calibrated = 0;

extern int isr_filter_time;
extern int battery_voltage;
extern enum QR mode;


void kalman(int p[], Filt_Param *Filt){
	int e;
	
	p[dXk] = p[dXs]-p[dXb];
	p[Xk] = p[Xk] + p[dXk]>>Filt->p2phi;
	e = p[Xk] - p[Xs] + p[Xb];
	p[Xk] = p[Xk] - e>>Filt->c1;
	p[dXb]= p[dXb] + e>>Filt->c2;
}


void calibrate(int p[], Filt_Param *Filt){
	// Calibrate the rate
	p[dXb] = p[dXb] + (p[dXs]-p[dXb])>>Filt->lp;

	// Calibrate the position
	p[Xb] = p[Xb] + (p[Xs]-p[Xb])>>Filt->lp;
}

void calibrate_yaw(int p[],Filt_Param *Filt){
	p[dXb] = p[dXb] + (p[dXs]-p[dXb])>>Filt->lp;
}

bool isCalibrated(int phi[], int theta[], int psi[]){
	int phi_err = phi[Xs] - phi[Xb];
	int theta_err = theta[Xs] - theta[Xb];

	int dphi_err = phi[dXs] - phi[dXb];
	int dtheta_err = theta[dXs] - theta[dXb];
	int dpsi_err = psi[dXs] - psi[dXb];

	return (phi_err<5) && (phi_err>-5) && 
			(theta_err<5) && (theta_err>-5) && 
			(dphi_err<5) && (dphi_err>-5) &&
			(dtheta_err<5) && (dtheta_err>-5) &&
			(dpsi_err<5) && (dpsi_err>-5);
}

void filter_sensor(){
	
	//static int phi[6] = {0,317440,0,0,519168,0};	// Roll 310, 507
	//static int theta[6] = {0,387072,0,0,510976,0}; // Pitch 378, 499
	//static int psi[3] = {0,499712,0}; // Yaw 488

	static int phi[6] = {0,317440,0,0,0,0};	// Roll
	static int theta[6] = {0,387072,0,0,0,0}; // Pitch
	static int psi[3] = {0,499712,0}; // Yaw


	//battery_voltage = X32_QR_S6;

	switch(mode){
		case CALIBRATION:
			phi[Xs] = INT_TO_FIXED(SAY);
			phi[dXs]= INT_TO_FIXED(SP);
			theta[Xs] = INT_TO_FIXED(SAX);
			theta[dXs]= INT_TO_FIXED(SQ);
			psi[dXs]= INT_TO_FIXED(SR);

			calibrate(phi,Filt_phi);
			calibrate(theta,Filt_thet);
			calibrate_yaw(psi,Filt_r);
			psi[dXk] = psi[dXs]-psi[dXb];

			calibrated = isCalibrated(phi,theta,psi);
			break;

		case YAW_CONTROL:
			psi[dXs]= INT_TO_FIXED(SR);

			calibrate_yaw(psi,Filt_r);
			psi[dXk] = psi[dXs]-psi[dXb];

			filtered_r = psi[dXk];
			break;

		case FULL_CONTROL:
			phi[Xs] = INT_TO_FIXED(SAY);
			phi[dXs]= INT_TO_FIXED(SP);
			theta[Xs] = INT_TO_FIXED(SAX);
			theta[dXs]= INT_TO_FIXED(SQ);
			psi[dXs]= INT_TO_FIXED(SR);

			kalman(phi,Filt_phi);
			kalman(theta,Filt_thet);
			calibrate_yaw(psi,Filt_r);
			psi[dXk] = psi[dXs]-psi[dXb];

			filtered_phi = phi[Xk];
			filtered_theta = thet[Xk];
			filtered_p = phi[dXk];
			filtered_q = theta[dXk];
			filtered_r = psi[dXk];
			break;
	}

}


void setup_sensor_interrupts(int prio){
	SET_INTERRUPT_VECTOR(INTERRUPT_XUFO, &isr_qr_link);
	SET_INTERRUPT_PRIORITY(INTERRUPT_XUFO, prio);
	ENABLE_INTERRUPT(INTERRUPT_XUFO);
}
