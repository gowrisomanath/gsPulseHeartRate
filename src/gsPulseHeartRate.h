/*
 * gsPulseHeartRate.h
 *
 *  Created on: Sep 15, 2014
 *      Author: Gowri Somanath (http://thepositronicbrain.blogspot.com/)
 */
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>

#include "mraa/aio.hpp"

#define MAX_PULSE_UP 10
class gsPulseHeartRate {
public:
	/*constructor function. Input the analog pin to which the pulse sensor is connected*/
	gsPulseHeartRate(int apin) ;
	/*This function takes in the analog reading, making sure MAX_PULSE_UP number of crests are seen.
	 It can be manually called in a loop till a pulse is obtained (see pulse_taken ),
	 or called using a interrupt service routine as shown in sample gsPulse.cpp
	 In practice,it should be called at least once every 10 microseconds to ensure robust reading.*/
	void getHeartSig(int);
	/*This is automatically called once there are enough samples. It calculates pulse rates in beats per minute (bpm)*/
	void calculatePulseRate();
	/*This is set to true once a pulse reading is available*/
	bool pulse_taken;
	/*Can be used after pulse_taken is true. Returns the last calculated pulse reading in beats per min.
	 * It will return -1 if its failed to get a robust reading in X attempts.*/
	int currentPulseRate();


private:
	void resetStats();
	void getSignalMean();

	struct timespec starttme;

	mraa::Aio* a0;

	int pulse_rate;
	float pulse_up_time[MAX_PULSE_UP];
	int pulse_up_cnt;
	int pulse_down_cnt;
	int pulse_cnt;
	bool PULSE_UP;
	bool PULSE_DOWN;

	int signal_mean;
	int signal_max;

	int pulse_lower_threshold;
	int pulse_upper_threshold;

	int fail_cnt;

};
