#include "filter.h"
#include "fixed_point.h"
#include "supervisor.h"
#include "logger.h"
#include "config.h"
#include "communication.h"
#include "led.h"

#define TEST_FILTERS 0
#define CALIBRATE_THRESHOLD 2048

#if TEST_FILTERS
#include "data.h" 
#endif

#define dXs 	0	// Sensed Rate
#define dXb		1	// Rate bias
#define dXk		2  	// Rate Kalman fitlered
#define Xs 		3	// Sensed position
#define Xb		4	// Bias
#define Xk		5	// Kalman filtered

int filtered_phi = 0; //Roll
int filtered_theta = 0;	//Pitch
int filtered_p = 0;	//Roll rate
int filtered_q = 0;	//Pitch rate
int filtered_r = 0;	//Yaw rate
int calibrated = 0;

extern int isr_filter_time;
extern int battery_voltage;
extern int last_sensor_isr_time;
extern enum QR mode;

/*------------------------------------------------------------------
 * kalman -- A Kalman filter implementation, from the example in the
 * IN4073 slides (Koen Langendoen).
 * Input :
 *			int p[]:			The measurement array
 * 			Filt_Param *Filt:	The filter parameters
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void kalman(int p[], Filt_Param *Filt){
	int e;
	p[dXk] = p[dXs]-p[dXb];
	p[Xk] += p[dXk]>>Filt->p2phi;
	e = p[Xk] - p[Xs] + p[Xb];
	p[Xk] -= e>>Filt->c1;
	p[dXb] += e>>Filt->c2;
	
}

/*------------------------------------------------------------------
 * calibrate -- Sets the bias of the gyro and accalerometer measurement.
 * Uses the low passed input signal to determine the bias.
 * Input :
 *			int p[]:			The measurement array
 * 			Filt_Param *Filt:	The filter parameters
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void calibrate(int p[], Filt_Param *Filt){
	// Calibrate the rate
	p[dXb] += (p[dXs]-p[dXb])>>Filt->lp;
	
	// Calibrate the position
	p[Xb] += (p[Xs]-p[Xb])>>Filt->lp;
}

/*------------------------------------------------------------------
 * calibrate_yaw -- Sets the bias of only the gyro measurement.
 * Uses the low passed input signal to determine the bias.
 * Input :
 *			int p[]:			The measurement array
 * 			Filt_Param *Filt:	The filter parameters
 * 	
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void calibrate_yaw(int p[],Filt_Param *Filt){
	p[dXb] += (p[dXs]-p[dXb])>>Filt->lp;
	
}

/*------------------------------------------------------------------
 * is_calibrated -- Determines if all the sensors are calibrated.
 * Input :
 *			int phi[]:			The roll measurement array
 *			int theta[]:		The pitch measurement array	
 * 			int psi[]:			The yaw measurement array
 * Returns:	
 *			int :	1 if the sensors are calibrated, otherwise 0.
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
int is_calibrated(int phi[], int theta[], int psi[]){
	int phi_err = phi[Xs] - phi[Xb];
	int theta_err = theta[Xs] - theta[Xb];

	int dphi_err = phi[dXs] - phi[dXb];
	int dtheta_err = theta[dXs] - theta[dXb];
	int dpsi_err = psi[dXs] - psi[dXb];
	

return (phi_err<CALIBRATE_THRESHOLD ) && (phi_err>-CALIBRATE_THRESHOLD ) && 
			(theta_err<CALIBRATE_THRESHOLD ) && (theta_err>-CALIBRATE_THRESHOLD ) && 
			(dphi_err<CALIBRATE_THRESHOLD ) && (dphi_err>-CALIBRATE_THRESHOLD ) &&
			(dtheta_err<CALIBRATE_THRESHOLD ) && (dtheta_err>-CALIBRATE_THRESHOLD ) &&
			(dpsi_err<CALIBRATE_THRESHOLD ) && (dpsi_err>-CALIBRATE_THRESHOLD );
}


/*------------------------------------------------------------------
 * filter_sensor -- The sensor interrupt, called at ~1270Hz. 
 * Uses measurement arrays to store (and shift) the sensor and filtered
 * data.
 *		- In CALIBRATION, the sensor data is fetched and the bias is set
 *		- In YAW_CONTROL, only the yaw data is fetched and is continuesly calibrated
 *		- In FULL_CONTROL, all sensor data is fetched and the Kalman filter
 *			is used to continuesly calibrate the pitch and yaw gyros.
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void filter_sensor(){
	
	static int phi[6] = {0,387072,0,0,519168,0};	// Roll 310, 507
	static int theta[6] = {0,-317440,0,0,510976,0}; // Pitch 378, 499
	static int psi[3] = {0,499712,0}; // Yaw 488
	int old = X32_clock_us;

	static int test_counter = 0;

	if (mode != FULL_CONTROL){
		log_data_sensor(X32_clock_us, SAX, SAY, SAZ, SP, SQ, SR); // Accel
	}

	switch(mode){
		case CALIBRATION:
			#if !TEST_FILTERS
			phi[Xs] = I2FDP(SAY);	
			phi[dXs]= I2FDP(SP);
			theta[Xs] = I2FDP(SAX);
			theta[dXs]= -1*I2FDP(SQ);
			psi[dXs]= I2FDP(SR);

			calibrate(phi,&Filt_phi);
			calibrate(theta,&Filt_thet);
			calibrate_yaw(psi,&Filt_r);
			
			calibrated = is_calibrated(phi,theta,psi);
			
			#else
			calibrated = 1;
			#endif
			
		
			break;

		case YAW_CONTROL:
			psi[dXs]= I2FDP(SR);

			calibrate_yaw(psi,&Filt_r);
			psi[dXk] = psi[dXs]-psi[dXb];

			filtered_r = psi[dXk];
			
			break;

		case FULL_CONTROL:
			phi[Xs] = I2FDP(SAY);
			phi[dXs]= I2FDP(SP);
		
			#if TEST_FILTERS
			log_start();
			theta[dXs] = -1*I2FDP(dx[test_counter]);
			theta[Xs] = I2FDP(x[test_counter]);
			test_counter++;
			#else
			theta[dXs]= -1*I2FDP(SQ);
			theta[Xs] = I2FDP(SAX);
			#endif
			
			psi[dXs]= I2FDP(SR);

			kalman(phi,&Filt_phi);
			kalman(theta,&Filt_thet);
			calibrate_yaw(psi,&Filt_r);
			psi[dXk] = psi[dXs]-psi[dXb];


			filtered_phi = phi[Xk];
			filtered_theta = theta[Xk];
			filtered_p = phi[dXk];
			filtered_q = theta[dXk];
			filtered_r = psi[dXk];
	
			break;
	}
	// Needed so the QR Link can be checked
	last_sensor_isr_time = X32_clock_us;

	battery_voltage = X32_QR_S6;
	
	if(calibrated)
	{
		set_led(1,7);
	}
	else
	{
		set_led(0,7);
	}

	isr_filter_time = X32_clock_us - old;
	#if TEST_FILTERS
	if(test_counter-1 < 2048){
		log_data_profile(FILTER, FDP2I(theta[Xk]),FDP2I(theta[dXk]));
	}	
	#else
	log_data_profile(FILTER, X32_clock_us,isr_filter_time);	
	#endif
	
}

/*------------------------------------------------------------------
 * setup_sensor_interrupts -- Sets up the sensor interrupt
 * Input :
 *			int prio:		The priority of this interrupt [0 999]
 * Author: Gijs Bruining
 *------------------------------------------------------------------
 */
void setup_sensor_interrupts(int prio){
	SET_INTERRUPT_VECTOR(INTERRUPT_XUFO, &filter_sensor);
	SET_INTERRUPT_PRIORITY(INTERRUPT_XUFO, prio);
	ENABLE_INTERRUPT(INTERRUPT_XUFO);

}



