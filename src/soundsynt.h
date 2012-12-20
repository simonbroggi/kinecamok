#include "ofMain.h"

class soundsynt{

	public:

        soundsynt(ofBaseApp *app);
        ~soundsynt();
		void setup(ofBaseApp *app);

		ofSoundStream soundStream;

		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;


		//------------------- for the simple sine wave synthesis
		float 	targetFrequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;
};

