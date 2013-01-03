#include "soundsynt.h"


soundsynt::soundsynt(ofBaseApp *app){
    setup(app);
}

soundsynt::~soundsynt(){

}

void soundsynt::setup(ofBaseApp *app){
	// 2 output channels,
	// 0 input channels
	// 22050 samples per second
	// 512 samples per buffer
	// 4 num buffers (latency)

	int bufferSize		= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.1f;
	bNoise 				= false;
	pan                 = 0.5;


	//soundStream.listDevices();

	//if you want to set the device id to be different than the default
	//soundStream.setDeviceID(1); 	//note some devices are input only and some are output only

	soundStream.setup(app, 2, 0, sampleRate, bufferSize, 4);


	//set target frequency
	targetFrequency = 1000.0f;
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
}


