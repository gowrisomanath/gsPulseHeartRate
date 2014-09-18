/*
 * gsPulseHeartRate.cpp
 *
 *  Created on: Sep 15, 2014
 *      Author: Gowri Somanath (http://thepositronicbrain.blogspot.com/)
 */
/*See the header for usage and details*/
#include "gsPulseHeartRate.h"

/*Uncommenting the DEBUG_PULSE will print out intermediate results*/
//#define DEBUG_PULSE

 gsPulseHeartRate::gsPulseHeartRate(int pulse_apin) {

	a0 = new mraa::Aio(pulse_apin);
	pulse_taken=false;
	pulse_rate = 0;
	fail_cnt=0;
	resetStats();

}
//gsPulseHeartRate

 /*get the signal mean for thresholding. This is important since analog reads vary based on if its connected to 3.3V or 5V.*/
 void gsPulseHeartRate::getSignalMean()
 {

	//throw out a few reads
	uint16_t adc_value;
	signal_max=0;
	int i;
	for( i=0;i<100;i++)
		adc_value = a0->read();

	long sigsum=0;
	for( i=0;i<2000;i++)
	{
		adc_value = a0->read();
		sigsum+=adc_value;
		if(adc_value>signal_max)
			signal_max=adc_value;

	}
	signal_mean=sigsum/i;
	pulse_lower_threshold=signal_mean;
	pulse_upper_threshold=(signal_max*0.85+signal_mean*0.15);
#ifdef DEBUG_PULSE
	std::cout<<"pulse_lower_threshold:"<<pulse_lower_threshold<<std::endl;
	std::cout<<"pulse_upper_threshold:"<<pulse_upper_threshold<<std::endl;
	std::cout<<"signal_max:"<<signal_max<<std::endl;
#endif
 }

void  gsPulseHeartRate::getHeartSig(int sig) {
	uint16_t adc_value;
	adc_value = a0->read();
//std::cout<<"a0:"<<adc_value<<std::endl;

	struct timespec tme;
	clock_gettime(CLOCK_REALTIME, &tme);
    // if its a trough
	if (adc_value < pulse_lower_threshold) {
		PULSE_DOWN = true;
		pulse_down_cnt++;
		PULSE_UP = false;
		pulse_up_cnt = 0;
	}
	//its just some noise with down-spike
	if (PULSE_DOWN == true && pulse_down_cnt < 3 && adc_value > pulse_lower_threshold) {
		pulse_down_cnt = 0;
		pulse_up_cnt = 0;
		PULSE_DOWN = false;
	}
	//if its a realible crest save the time
	if (pulse_cnt < MAX_PULSE_UP && PULSE_DOWN == true && pulse_down_cnt > 3
			&& adc_value > pulse_upper_threshold) {
		PULSE_UP = true;
		PULSE_DOWN = false;
		pulse_down_cnt = 0;
		pulse_up_time[pulse_cnt] = (tme.tv_nsec-starttme.tv_nsec) * 0.001f + (tme.tv_sec-starttme.tv_sec) * 1000000.0f; //microseconds
		pulse_cnt++;

	}
	if (pulse_cnt == MAX_PULSE_UP)
		calculatePulseRate();

} //getHeartSig

void  gsPulseHeartRate::calculatePulseRate() {

	/*basic logic is to measure the time between the crests in the pulse signal.
	 * Each such period is a heart beat. Measure a few to ensure robustness.
	 * Then use the following to do some noise removal and calculate the bpm.*/
#ifdef DEBUG_PULSE
	std::cout<<std::endl<<"in calculate "<<std::endl;
#endif
	float psum = 0.0f, tmp, maxp=0.0f, meanp=0.0f;
	int i, cnt = 0;
	//get the mean of time between crests
	for (i = 0; i < pulse_cnt - 1; i++) {
		tmp =  (pulse_up_time[i + 1] - pulse_up_time[i]);
#ifdef DEBUG_PULSE
	std::cout<<"tmp: "<<i<<","<<tmp<<std::endl;
#endif
		psum = psum + tmp;
	}
	psum = psum / i;
#ifdef DEBUG_PULSE
	std::cout<<"psum:"<<psum<<std::endl;
#endif
	//eliminate noise based on mean
	maxp = 0.0f;
	for (i = 0; i < pulse_cnt - 1; i++) {
		tmp =  (pulse_up_time[i + 1] - pulse_up_time[i]);
		if (tmp < 1.2 * psum && tmp > 0.75 * psum) {
			if (tmp > maxp)
				maxp = tmp;
			meanp += tmp;
			cnt++;
#ifdef DEBUG_PULSE
	std::cout<<"used: "<<i<<","<<tmp<<std::endl;
#endif
		}

	}
	if (cnt > 1)
	{
		meanp = meanp / (cnt - 1);
#ifdef DEBUG_PULSE
	std::cout<<"meanp:"<<meanp<<std::endl;
#endif
	//to get the beats per minute
	pulse_rate =(int)(60000000.0f / meanp);
	pulse_taken=true;
	fail_cnt=0;
#ifdef DEBUG_PULSE
	std::cout<<"=========================== pulse:"<<pulse_rate<<std::endl;
#endif
	}
	else
	{
		fail_cnt++;
		if(fail_cnt>3)
			pulse_rate=-1;
	}
	//reset to take next set of measurements
	resetStats();

}
//calculatePulseRate

void  gsPulseHeartRate::resetStats()
{


	pulse_up_cnt = 0;
	pulse_down_cnt = 0;
	pulse_cnt = 0;
	PULSE_UP = false;
	PULSE_DOWN = false;
	getSignalMean();
    clock_gettime(CLOCK_REALTIME, &starttme);

}    //resetStatus

int  gsPulseHeartRate::currentPulseRate() {

	return pulse_rate;
}
