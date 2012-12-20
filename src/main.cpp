#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
#ifdef TARGET_OSX
	window.setGlutDisplayString("rgba double samples>=8 depth");
#endif

	//ofSetupOpenGL(&window, 1024, 768, OF_FULLSCREEN);
	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
	//ofSetupOpenGL(&window, 1680, 1050, OF_GAME_MODE);
	ofRunApp(new testApp());
}
