/*
 * gsPulse.cpp
 *
 *  Created on: Sep 15, 2014
 *      Author: Gowri Somanath (http://thepositronicbrain.blogspot.com/)
 *
* A library and test program I created for the Pulse Heart rate sensor.
* Developed using the Intel IoT Dev-Kit for the Galileo/Edison boards.
*
* The library is made  of gsPulseHeartRate.h and gsPulseHeartRate.cpp.
* See the header files for details on the functions and usage.
* gsPulse.cpp shows a test program demonstrating how the library can be used.
*
* The Intel IoT Dev Kit is available from https://software.intel.com/iot
*
* */


#define pulse_apin 0

#include "gsPulseHeartRate.h"

//Create the object.
gsPulseHeartRate pulse(pulse_apin);

/* The library function to read the pulse sensor and calcuate heart rate can be called in a loop,
or as shown below called through a regular interrupt service.
See the header for different functions
*/


// An Interrupt service routine or signal handler which calls the library.
void signal_handler(int a)
{
	pulse.getHeartSig(a);

}
// called as ISR every so often
void print_heartrate(int a)
{
	if(pulse.pulse_taken)
				std::cout<<"Pulse rate "<<pulse.currentPulseRate()<<" bpm"<<std::endl;
}

int main() {
//every program is allowed 3 timers. using 2 here. one for reading, one for display.
	struct itimerval tmr,ptmr;
	tmr.it_value.tv_sec = 0;
	tmr.it_value.tv_usec = 2;
	tmr.it_interval = tmr.it_value;

	ptmr.it_value.tv_sec = 0.5;
	ptmr.it_value.tv_usec = 0;
	ptmr.it_interval = ptmr.it_value;

	signal((int)SIGALRM, signal_handler);
	signal((int)SIGVTALRM, print_heartrate);
	setitimer(ITIMER_REAL, &tmr, NULL);
	setitimer(ITIMER_VIRTUAL, &ptmr, NULL);


	for (;;) {


	}
	return 0;

}
