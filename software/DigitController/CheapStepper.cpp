/*  CheapStepper.cpp - 
	v0.2
	Library for the 28BYJ-48 stepper motor, using ULN2003 driver board
	https://arduino-info.wikispaces.com/SmallSteppers

	Library written by Tyler Henry, 6/2016

	uses 8-step sequence: A-AB-B-BC-C-CD-D-DA

	motor has gear ratio of either:
		64:1 (per manufacturer specs)
		or 
		63.68395:1 measured
			(see: http://forum.arduino.cc/index.php?topic=71964.15)
	* 64 steps per internal motor rev
	= 

	4096 total mini-steps / revolution
	or ~4076 (4075.7728) depending on exact gear ratio

	assumes 5v power source for rpm calc
*/


#include "Arduino.h"
#include "CheapStepper.h"


CheapStepper::CheapStepper () {
	for (int pin=0; pin<4; pin++){
		pinMode(pins[pin], OUTPUT);
	}
}

CheapStepper::CheapStepper (int in1, int in2, int in3, int in4) {

	pins[0] = in1;
	pins[1] = in2;
	pins[2] = in3;
	pins[3] = in4;
	CheapStepper();
}


void CheapStepper::move (bool clockwise, int numSteps){

	for (int n=0; n<numSteps; n++){
		step(clockwise);
	}
}


void CheapStepper::step(bool clockwise){

	if (clockwise) seqCW();
	else seqCCW();
}

void CheapStepper::stopCurrent() {
  seq(8);
}


/////////////
// PRIVATE //
/////////////



void CheapStepper::seqCW (){
	seqN++;
	if (seqN > 7) seqN = 0; // roll over to A seq
	seq(seqN);

	stepN++; // track miniSteps
	if (stepN >= totalSteps){
		stepN -=totalSteps; // keep stepN within 0-(totalSteps-1)
	}
}

void CheapStepper::seqCCW (){
	seqN--;
	if (seqN < 0) seqN = 7; // roll over to DA seq
	seq(seqN);

	stepN--; // track miniSteps
	if (stepN < 0){
		stepN +=totalSteps; // keep stepN within 0-(totalSteps-1)
	}
}

void CheapStepper::seq (int seqNum){

	int pattern[4];
	// A,B,C,D HIGH/LOW pattern to write to driver board
	
	switch(seqNum){
		case 0:
		{
			pattern[0] = 1;
			pattern[1] = 0;
			pattern[2] = 0;
			pattern[3] = 0;
			break;
		}
		case 1:
		{
			pattern[0] = 1;
			pattern[1] = 1;
			pattern[2] = 0;
			pattern[3] = 0;
			break;
		}
		case 2:
		{
			pattern[0] = 0;
			pattern[1] = 1;
			pattern[2] = 0;
			pattern[3] = 0;
			break;
		}
		case 3:
		{
			pattern[0] = 0;
			pattern[1] = 1;
			pattern[2] = 1;
			pattern[3] = 0;
			break;
		}	
		case 4:
		{
			pattern[0] = 0;
			pattern[1] = 0;
			pattern[2] = 1;
			pattern[3] = 0;
			break;
		}
		case 5:
		{
			pattern[0] = 0;
			pattern[1] = 0;
			pattern[2] = 1;
			pattern[3] = 1;
			break;
		}
		case 6:
		{
			pattern[0] = 0;
			pattern[1] = 0;
			pattern[2] = 0;
			pattern[3] = 1;
			break;
		}
		case 7:
		{
			pattern[0] = 1;
			pattern[1] = 0;
			pattern[2] = 0;
			pattern[3] = 1;
			break;
		}
		default:
		{
			pattern[0] = 0;
			pattern[1] = 0;
			pattern[2] = 0;
			pattern[3] = 0;
			break;
		}
	}

	// write pattern to pins
	for (int p=0; p<4; p++){
		digitalWrite(pins[p], pattern[p]);
	}
}
