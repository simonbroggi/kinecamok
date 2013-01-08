#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxAssimpModelLoader.h"
#include "ofxProCamToolkit.h"
#include "ofxAutoControlPanel.h"
#include "LineArt.h"

#include "faceReceiver.h"
#include "soundsynt.h"

class testApp : public ofBaseApp {

public:
	void setb(string name, bool value);
	void seti(string name, int value);
	void setf(string name, float value);
	bool getb(string name);
	int geti(string name);
	float getf(string name);

	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	soundsynt *synt;
	void audioOut(float * input, int bufferSize, int nChannels);

	void setupControlPanel();
	void setupMesh();
	void constructMesh();

	int currentMovie;
	void loadNewMovie();

	void drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg = ofColor::black, ofColor fg = ofColor::white);
	void updateRenderMode();
	void drawSelectionMode();
	void drawRenderMode();
	void render();
	void multiplyLightConeFalloff();
	void saveCalibration();
	void preSwitchToRenderMode();

	void updateTrackingAge();
	void toggleSelectionMode();

	ofxAssimpModelLoader model;
	ofEasyCam cam;

	ofVboMesh objectMesh; // the model.dae mesh
	//ofImage texture;
	ofVideoPlayer myPlayer;

	faceReceiver *fReceiver;

	ofMesh imageMesh;
	ofLight light;
	ofxAutoControlPanel panel;

    int maxCalibPoints;
    int currCalibPoints;
	vector<cv::Point3f> objectPoints;
	vector<cv::Point2f> imagePoints;
	//vector<bool> referencePoints;

	cv::Mat rvec, tvec;
	ofMatrix4x4 modelMatrix;
	ofxCv::Intrinsics intrinsics;
	bool calibrationReady;

	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
	ofShader shader;
	float howStrong;//shader attribute
};
