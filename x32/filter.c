#include "filter.h"
#include "fixed_point.h"
#include "supervisor.h"
#include "logger.h"
#include "config.h"

#define dXm 	0	// Rate measurement
#define dXa 	1	// Rate absurd values removed
#define dXlp 	2	// Rate low-passed
#define dXad 	3	// Rate anti-drifted
#define Xm 		4	// Measurement
#define Xa 		5	// Absurd values removed
#define Xlp 	6	// Low-passed
#define Xad 	7	// Anti-drift
#define Xad1 	8	// Anti-drift n-1
#define Xad2 	9	// Anti-drift n-2
#define Xbf2 	10	// Butterworth filtered n-2
#define Xbf1 	11	// Butterworth filtered n-1
#define Xbf 	12	// Butterworth filtered
#define Xbias	13	// Kalman filter bias
#define dXk		14  // Rate Kalman fitlered
#define Xk		15	// Kalman filtered

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

int filtered_phi = 0;
int filtered_thet = 0;
int filtered_p = 0;	//Roll
int filtered_q = 0;	//Pitch
int filtered_r = 0;	//Yaw
int calibrated = 0;

extern int isr_filter_time;
extern int battery_voltage;
extern enum QR mode;


void kalman(int p[], Filt_Param *Filt){
	int e;
	p[dXk] = p[dXad] - p[Xbias];
	p[Xk] = p[Xk] + MULT_FIXED(p[dXk],Filt->dt);
	e = p[Xk] - p[Xbf];
	p[Xk] = p[Xk] - DIV_FIXED(e,Filt->C1);
	p[Xbias] = p[Xbias] + DIV_FIXED(e,Filt->C2);
}

void BF_2nd(int p[], Filt_Param *Filt){
	p[Xbf2] = p[Xbf1];
	p[Xbf1] = p[Xbf];
	p[Xbf] = MULT_FIXED(Filt->a0,p[Xad]) + MULT_FIXED(Filt->a1,p[Xad1]) + 
				MULT_FIXED(Filt->a2,p[Xad2]) - MULT_FIXED(Filt->b1,p[Xbf1]) - 
				MULT_FIXED(Filt->b2,p[Xbf2]);
}

void F_1st(int p[], Filt_Param *Filt){
	p[dXlp] = p[dXlp] + MULT_FIXED(Filt->alph,(p[dXa]-p[dXlp]));
}

void anti_drift(int p[], Filt_Param *Filt){
	F_1st(p,Filt);
	p[dXad] = p[dXa] - p[dXlp];
}

void rem_absurd_val(int p[], Filt_Param *Filt){
	if((p[dXm] >= Filt->min) && (p[dXm] <= Filt->max)){
		p[dXa] = p[dXm];
	}
}

void process_roll(int phi[]){
	
	phi[dXm] = INT_TO_FIXED(X32_QR_S3);
	rem_absurd_val(phi, &Filt_phi);
	anti_drift(phi, &Filt_phi);
	
	phi[Xm] = INT_TO_FIXED(X32_QR_S0);
	rem_absurd_val(phi+Xm, &Filt_phi);
	phi[Xad2] = phi[Xad1];
	phi[Xad1] = phi[Xad];
	anti_drift(phi+Xm, &Filt_phi);
	BF_2nd(phi, &Filt_phi);
	kalman(phi, &Filt_phi);
	
	filtered_phi = phi[Xk];
	filtered_p = phi[dXk];
}

void process_pitch(int thet[]){
	
	thet[dXm] = INT_TO_FIXED(X32_QR_S4);
	rem_absurd_val(thet, &Filt_thet);
	anti_drift(thet, &Filt_thet);
	
	thet[Xm] = INT_TO_FIXED(X32_QR_S1);
	rem_absurd_val(thet+Xm, &Filt_thet);
	thet[Xad2] = thet[Xad1];
	thet[Xad1] = thet[Xad];
	anti_drift(thet+Xm, &Filt_thet);
	BF_2nd(thet, &Filt_thet);
	kalman(thet, &Filt_thet);
	
	filtered_thet = thet[Xk];
	filtered_q = thet[dXk];
}

void process_yaw(int yaw[]){
		
	yaw[dXm] = INT_TO_FIXED(X32_QR_S5);
	rem_absurd_val(yaw, &Filt_r);
	anti_drift(yaw, &Filt_r);
	filtered_r = yaw[dXad];
	
}

void calibrate_sensors(int phi[], int thet[], int yaw[]){
	// Calibrate the roll rate
	phi[dXm] = INT_TO_FIXED(X32_QR_S3);
	rem_absurd_val(phi, &Filt_phi);
	anti_drift(phi, &Filt_phi);


	// Calibrate the roll
	phi[Xm] = INT_TO_FIXED(X32_QR_S0);
	rem_absurd_val(phi+Xm, &Filt_phi);
	phi[Xad2] = phi[Xad1];
	phi[Xad1] = phi[Xad];
	anti_drift(phi+Xm, &Filt_phi);


	// Calibrate the pitch rate
	thet[dXm] = INT_TO_FIXED(X32_QR_S4);
	rem_absurd_val(thet, &Filt_thet);
	anti_drift(thet, &Filt_thet);

	// Calibrate the pitch
	thet[Xm] = INT_TO_FIXED(X32_QR_S1);
	rem_absurd_val(thet+Xm, &Filt_thet);
	thet[Xad2] = thet[Xad1];
	thet[Xad1] = thet[Xad];
	anti_drift(thet+Xm, &Filt_thet);

	// Calibrate the yaw-rate
	yaw[dXm] = INT_TO_FIXED(X32_QR_S5);
	rem_absurd_val(yaw, &Filt_r);
	anti_drift(yaw, &Filt_r);
}

void setup_sensor_interrupts(int prio){
	SET_INTERRUPT_VECTOR(INTERRUPT_XUFO, &isr_sensor);
	SET_INTERRUPT_PRIORITY(INTERRUPT_XUFO, prio);
	ENABLE_INTERRUPT(INTERRUPT_XUFO);
}

void isr_sensor(){
	static int phi[14];	// Roll
	static int thet[14]; // Pitch
	static int yaw[4]; // Yaw

	int old = X32_clock_us;

	// Logging data
	log_data(ACCEL, X32_clock_us, X32_QR_S0, X32_QR_S1, X32_QR_S2 ); // Accel
	log_data(GYRO, X32_clock_us, X32_QR_S3, X32_QR_S4, X32_QR_S5); // gyro
	log_data(BATTERY, X32_clock_us, X32_QR_S6, 0, 0); // battery

	battery_voltage = X32_QR_S6;

	switch(mode){
		case CALIBRATION:
			calibrate_sensors(phi,thet,yaw);
			calibrated = yaw[dXad]>(yaw[dXlp]-16) && yaw[dXad]<(yaw[dXlp]+16);
			break;
		
		case YAW_CONTROL:
			process_yaw(yaw);
			break;
		
		case FULL_CONTROL:
			process_roll(phi);
			process_pitch(thet);
			process_yaw(yaw);
			break;
	}

	isr_filter_time = X32_clock_us - old;
}
