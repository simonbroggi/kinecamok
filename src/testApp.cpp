#include "testApp.h"

//middle mouse to switch doesnt break other mouse interaction

#define MOUSE_SELECT_BUTTON 0
#define MOUSE_SWITCH_BUTTON 1


using namespace ofxCv;
using namespace cv;

void testApp::setb(string name, bool value) {
	panel.setValueB(name, value);
}
void testApp::seti(string name, int value) {
	panel.setValueI(name, value);
}
void testApp::setf(string name, float value) {
	panel.setValueF(name, value);
}
bool testApp::getb(string name) {
	return panel.getValueB(name);
}
int testApp::geti(string name) {
	return panel.getValueI(name);
}
float testApp::getf(string name) {
	return panel.getValueF(name);
}

void testApp::setup() {
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
	calibrationReady = false;

	cout<<"going to sett up mesh"<<endl;
	setupMesh();
	cout<<"setting up mesh done"<<endl;

	setupControlPanel();

	cout<<"goan init face reciever"<<endl;
	fReceiver = new faceReceiver();
	cout<<"face receiver init done"<<endl;

    cout<<"gona setup soundsynth"<<endl;
	//synt = new soundsynt(this);
    cout<<"setting up soundsynth done"<<endl;
	currentMovie = 0;

    loadNewMovie();

    //myPlayer.closeMovie(); //Unloads video resources

}

void testApp::loadNewMovie(){
    /*if( myPlayer.isLoaded() ){
        cout<<"closing movie "<<currentMovie<<endl;
        myPlayer.closeMovie();
    }*/
    cout<<"setting new current movie number "<<endl;
    currentMovie = (currentMovie+1) % 2;
    //currentMovie = ofRandom(2); //load random movie
    cout<<"loading movie "<<currentMovie<<endl;
    switch(currentMovie){
        case 0:
            myPlayer.loadMovie("schnauz1.avi");
            break;
        case 1:
            myPlayer.loadMovie("schnauz2.avi");
            break;
        default:
            cout<<"strange, but loading schnauz1"<<endl;
            myPlayer.loadMovie("schnauz1.avi");
    }
    cout<<"loaded "<<endl;

	//myPlayer.loadMovie("schnauz1.avi"); //Loads video resources
	myPlayer.setLoopState(OF_LOOP_NONE);
    myPlayer.play();
    cout<<"now playing "<<endl;
    //myPlayer.stop();
    myPlayer.setPaused(true);
    myPlayer.firstFrame();
}


void testApp::update() {
    if ( fReceiver->update( objectMesh.getVertices() ) ){
        //if ( myPlayer.isLoaded() && !myPlayer.isPlaying() ){
        if ( myPlayer.isLoaded() && myPlayer.isPaused() ){
            //myPlayer.play();
            myPlayer.setPaused(false);
        }
    }
    else{
        //if ( myPlayer.isPlaying() ){
        if ( !myPlayer.isPaused() ){
            float trackingAge = ofGetElapsedTimef() - fReceiver->lastFaceReceivedTime;
            if ( trackingAge > 2.0 ){ //if movie is playing and no tracking for 2 secs stop the movie and choose a new one

                loadNewMovie();
            }
        }
    }

	ofSetWindowTitle("mapamok");
	if(getb("randomLighting")) {
		setf("lightX", ofSignedNoise(ofGetElapsedTimef(), 1, 1) * 1000);
		setf("lightY", ofSignedNoise(1, ofGetElapsedTimef(), 1) * 1000);
		setf("lightZ", ofSignedNoise(1, 1, ofGetElapsedTimef()) * 1000);
	}
	light.setPosition(getf("lightX"), getf("lightY"), getf("lightZ"));

	if(getb("selectionMode") ) {
		cam.enableMouseInput();
	} else {
		updateRenderMode();
		cam.disableMouseInput();
	}

	myPlayer.update();

}

void testApp::exit(){
    delete fReceiver;
}

void testApp::draw() {
	//ofBackground(geti("backgroundColor"));
	ofBackground(geti("backgroundColorR"), geti("backgroundColorG"), geti("backgroundColorB"));

	if(getb("saveCalibration")) {
		saveCalibration();
		setb("saveCalibration", false);
	}
	if(getb("selectionMode")) {
		drawSelectionMode();
	} else {
		drawRenderMode();
	}
	if(getb("multiplyGradient")) {
        multiplyLightConeFalloff();
	}


	if(!getb("validShader")) {
		ofPushStyle();
		ofSetColor(magentaPrint);
		ofSetLineWidth(8);
		ofLine(0, 0, ofGetWidth(), ofGetHeight());
		ofLine(ofGetWidth(), 0, 0, ofGetHeight());
		string message = "Shader failed to compile.";
		ofVec2f center(ofGetWidth(), ofGetHeight());
		center /= 2;
		center.x -= message.size() * 8 / 2;
		center.y -= 8;
		drawHighlightString(message, center);
		ofPopStyle();
	}
}

void testApp::keyPressed(int key) {
	if(key == OF_KEY_BACKSPACE) { // delete selected
		if(getb("selected")) {
			setb("selected", false);
			//int choice = geti("selectionChoice");
			//referencePoints[choice] = false;
			//imagePoints[choice] = Point2f();
			if(!getb("selectionMode")){
                currCalibPoints--;
			}
			//imagePoints[currCalibPoints] = ; //ok?

		}
	}
	/*if(key == '\n') { // deselect
		setb("selected", false);
	}*/
	if(key == 'n'){
        if(getb("selectionMode")){
            cout<<"selecting vertex no 5, the tip of the nose"<<endl;
            seti("selectionChoice", 5);
            setb("selected", true);
	    }
	}
	if(key == 'e') {
        if(getb("selectionMode")){
            cout<<"selecting vertex no 3, between the eyebrows"<<endl;
            seti("selectionChoice", 3);
            setb("selected", true);
	    }
	}
	if(key == ' ') { // toggle render/select mode
        toggleSelectionMode();
	}

}

void testApp::toggleSelectionMode(){
    if(getb("selectionMode")){
        preSwitchToRenderMode();
        setb("selectionMode", false);
    }
    else{
        setb("selectionMode", true);
    }
}

void testApp::preSwitchToRenderMode(){
    if(getb("selected")){
        currCalibPoints++;
        objectPoints[currCalibPoints-1] = toCv(objectMesh.getVertex(geti("selectionChoice"))); //is the objectMesh projected right?
    }
}

void testApp::mousePressed(int x, int y, int button) {
	if(getb("selectionMode")){
        if(button == MOUSE_SELECT_BUTTON){
            setb("selected", getb("hoverSelected"));
            seti("selectionChoice", geti("hoverChoice"));
        }
	}

    if(button == MOUSE_SWITCH_BUTTON){

        cout<<" -!toggled selectionMode!- ";
        toggleSelectionMode();
    }


}

void testApp::mouseReleased(int x, int y, int button) {

}
void testApp::constructMesh() {
    objectMesh.clear();

    objectMesh.addVertex(ofVec3f(-0.1188913, 0.1447522, 1.462958));
    objectMesh.addVertex(ofVec3f(-0.1669641, 0.1243923, 1.440056));
    objectMesh.addVertex(ofVec3f(-0.1653069, 0.09110554, 1.417094));
    objectMesh.addVertex(ofVec3f(-0.1747767, 0.06070167, 1.414868));
    objectMesh.addVertex(ofVec3f(-0.1753025, 0.0526522, 1.417076));
    objectMesh.addVertex(ofVec3f(-0.1969763, 0.003514421, 1.404488));
    objectMesh.addVertex(ofVec3f(-0.1925595, -0.003052444, 1.41313));
    objectMesh.addVertex(ofVec3f(-0.1977634, -0.01342062, 1.409583));
    objectMesh.addVertex(ofVec3f(-0.203329, -0.03421411, 1.409347));
    objectMesh.addVertex(ofVec3f(-0.2013743, -0.04063132, 1.414459));
    objectMesh.addVertex(ofVec3f(-0.2067069, -0.07270461, 1.418686));
    objectMesh.addVertex(ofVec3f(-0.1406784, 0.1476383, 1.477912));
    objectMesh.addVertex(ofVec3f(-0.1708823, 0.1391136, 1.490123));
    objectMesh.addVertex(ofVec3f(-0.1918906, 0.1089484, 1.466779));
    objectMesh.addVertex(ofVec3f(-0.1971128, 0.09933487, 1.500647));
    objectMesh.addVertex(ofVec3f(-0.2110181, 0.07031526, 1.473013));
    objectMesh.addVertex(ofVec3f(-0.2037887, 0.07903966, 1.448481));
    objectMesh.addVertex(ofVec3f(-0.1859964, 0.06119379, 1.422397));
    objectMesh.addVertex(ofVec3f(-0.2053831, 0.072906, 1.448478));
    objectMesh.addVertex(ofVec3f(-0.1987096, 0.06023414, 1.446773));
    objectMesh.addVertex(ofVec3f(-0.209952, 0.05344999, 1.469165));
    objectMesh.addVertex(ofVec3f(-0.1996564, 0.0571395, 1.44657));
    objectMesh.addVertex(ofVec3f(-0.2020011, 0.04811954, 1.446566));
    objectMesh.addVertex(ofVec3f(-0.1842749, 0.04679134, 1.434576));
    objectMesh.addVertex(ofVec3f(-0.202553, 0.04599634, 1.446565));
    objectMesh.addVertex(ofVec3f(-0.194051, 0.01092772, 1.429393));
    objectMesh.addVertex(ofVec3f(-0.2030426, 0.002307454, 1.433868));
    objectMesh.addVertex(ofVec3f(-0.2138822, 0.02324783, 1.456797));
    objectMesh.addVertex(ofVec3f(-0.215467, 0.004752806, 1.49647));
    objectMesh.addVertex(ofVec3f(-0.209004, 0.05318919, 1.500557));
    objectMesh.addVertex(ofVec3f(-0.218415, -0.03855217, 1.490937));
    objectMesh.addVertex(ofVec3f(-0.2123564, -0.02092517, 1.433953));
    objectMesh.addVertex(ofVec3f(-0.2180457, -0.06425991, 1.437085));
    objectMesh.addVertex(ofVec3f(-0.2027698, -0.01094443, 1.410876));
    objectMesh.addVertex(ofVec3f(-0.1331062, 0.1156022, 1.416073));
    objectMesh.addVertex(ofVec3f(-0.1653069, 0.09110554, 1.417094));
    objectMesh.addVertex(ofVec3f(-0.1747767, 0.06070167, 1.414868));
    objectMesh.addVertex(ofVec3f(-0.1753025, 0.0526522, 1.417076));
    objectMesh.addVertex(ofVec3f(-0.1969763, 0.003514421, 1.404488));
    objectMesh.addVertex(ofVec3f(-0.1925595, -0.003052444, 1.41313));
    objectMesh.addVertex(ofVec3f(-0.1984017, -0.02503305, 1.412938));
    objectMesh.addVertex(ofVec3f(-0.202296, -0.0333868, 1.410502));
    objectMesh.addVertex(ofVec3f(-0.2014214, -0.04180128, 1.414821));
    objectMesh.addVertex(ofVec3f(-0.2064896, -0.07335234, 1.41923));
    objectMesh.addVertex(ofVec3f(-0.09845325, 0.136676, 1.448002));
    objectMesh.addVertex(ofVec3f(-0.08195658, 0.1160271, 1.427132));
    objectMesh.addVertex(ofVec3f(-0.1072458, 0.08697331, 1.406821));
    objectMesh.addVertex(ofVec3f(-0.07841545, 0.06851916, 1.416568));
    objectMesh.addVertex(ofVec3f(-0.1091497, 0.04386861, 1.400854));
    objectMesh.addVertex(ofVec3f(-0.1303961, 0.05998575, 1.396493));
    objectMesh.addVertex(ofVec3f(-0.1647358, 0.0556742, 1.407337));
    objectMesh.addVertex(ofVec3f(-0.1319905, 0.0538521, 1.39649));
    objectMesh.addVertex(ofVec3f(-0.1395555, 0.0448768, 1.404871));
    objectMesh.addVertex(ofVec3f(-0.1184967, 0.02970674, 1.404383));
    objectMesh.addVertex(ofVec3f(-0.1405024, 0.04178217, 1.404668));
    objectMesh.addVertex(ofVec3f(-0.142847, 0.0327622, 1.404664));
    objectMesh.addVertex(ofVec3f(-0.1589788, 0.04022406, 1.416657));
    objectMesh.addVertex(ofVec3f(-0.1433989, 0.030639, 1.404663));
    objectMesh.addVertex(ofVec3f(-0.1728412, 0.005421309, 1.414369));
    objectMesh.addVertex(ofVec3f(-0.1691847, -0.006482601, 1.409885));
    objectMesh.addVertex(ofVec3f(-0.1385775, 0.003697534, 1.403455));
    objectMesh.addVertex(ofVec3f(-0.1084448, -0.02303185, 1.42066));
    objectMesh.addVertex(ofVec3f(-0.09050132, 0.022424, 1.416616));
    objectMesh.addVertex(ofVec3f(-0.1269597, -0.06229542, 1.426155));
    objectMesh.addVertex(ofVec3f(-0.1742378, -0.03082136, 1.406952));
    objectMesh.addVertex(ofVec3f(-0.1841877, -0.07304996, 1.413102));
    objectMesh.addVertex(ofVec3f(-0.1944026, -0.01311668, 1.404949));
    objectMesh.addVertex(ofVec3f(-0.2020169, 0.05742072, 1.452618));
    objectMesh.addVertex(ofVec3f(-0.2044959, 0.04788379, 1.452614));
    objectMesh.addVertex(ofVec3f(-0.134301, 0.03984061, 1.404651));
    objectMesh.addVertex(ofVec3f(-0.1367801, 0.03030368, 1.404647));
    objectMesh.addVertex(ofVec3f(-0.1939416, 0.05532424, 1.446898));
    objectMesh.addVertex(ofVec3f(-0.1964206, 0.04578732, 1.446893));
    objectMesh.addVertex(ofVec3f(-0.1423763, 0.04193709, 1.410372));
    objectMesh.addVertex(ofVec3f(-0.1448554, 0.03240016, 1.410367));
    objectMesh.addVertex(ofVec3f(-0.2006971, 0.00472815, 1.416032));
    objectMesh.addVertex(ofVec3f(-0.1851302, 0.0006867451, 1.405006));
    objectMesh.addVertex(ofVec3f(-0.1759758, 0.0528255, 1.420804));
    objectMesh.addVertex(ofVec3f(-0.1716949, 0.05171411, 1.417771));
    objectMesh.addVertex(ofVec3f(-0.2028627, -0.01349918, 1.423524));
    objectMesh.addVertex(ofVec3f(-0.1838034, -0.01844728, 1.410023));
    objectMesh.addVertex(ofVec3f(-0.2019339, -0.02156483, 1.423434));
    objectMesh.addVertex(ofVec3f(-0.1868085, -0.02549164, 1.41272));
    objectMesh.addVertex(ofVec3f(-0.2018776, -0.02404924, 1.424424));
    objectMesh.addVertex(ofVec3f(-0.1867522, -0.02797605, 1.41371));
    objectMesh.addVertex(ofVec3f(-0.2065161, -0.03025499, 1.424507));
    objectMesh.addVertex(ofVec3f(-0.1874568, -0.03520309, 1.411007));
    objectMesh.addVertex(ofVec3f(-0.1982741, -0.02184098, 1.411949));
    objectMesh.addVertex(ofVec3f(-0.2062033, -0.02156012, 1.434158));
    objectMesh.addVertex(ofVec3f(-0.1759523, -0.02941374, 1.41273));
    objectMesh.addVertex(ofVec3f(-0.2209009, -0.01990652, 1.45144));
    objectMesh.addVertex(ofVec3f(-0.1557669, -0.03681634, 1.405303));
    objectMesh.addVertex(ofVec3f(-0.1849647, 0.03186398, 1.425064));
    objectMesh.addVertex(ofVec3f(-0.1723167, 0.02858034, 1.416105));
    objectMesh.addVertex(ofVec3f(-0.1807487, 0.03580943, 1.41556));
    objectMesh.addVertex(ofVec3f(-0.2039195, 0.05856111, 1.457919));
    objectMesh.addVertex(ofVec3f(-0.1286149, 0.03901081, 1.404577));
    objectMesh.addVertex(ofVec3f(-0.2043795, 0.05679178, 1.457919));
    objectMesh.addVertex(ofVec3f(-0.1290748, 0.03724149, 1.404577));
    objectMesh.addVertex(ofVec3f(-0.2062191, 0.04971447, 1.457915));
    objectMesh.addVertex(ofVec3f(-0.1309145, 0.03016418, 1.404573));
    objectMesh.addVertex(ofVec3f(-0.2057615, 0.04846168, 1.459021));
    objectMesh.addVertex(ofVec3f(-0.1304568, 0.02891139, 1.405679));
    objectMesh.addVertex(ofVec3f(-0.1902474, 0.05501451, 1.441881));
    objectMesh.addVertex(ofVec3f(-0.1480223, 0.0440522, 1.411971));
    objectMesh.addVertex(ofVec3f(-0.1907073, 0.05324519, 1.44188));
    objectMesh.addVertex(ofVec3f(-0.1484822, 0.04228288, 1.41197));
    objectMesh.addVertex(ofVec3f(-0.192547, 0.04616788, 1.441877));
    objectMesh.addVertex(ofVec3f(-0.1503219, 0.03520557, 1.411967));
    objectMesh.addVertex(ofVec3f(-0.1920894, 0.04491509, 1.442982));
    objectMesh.addVertex(ofVec3f(-0.1498643, 0.03395278, 1.413072));
    objectMesh.addVertex(ofVec3f(-0.2026341, -0.0004385458, 1.423813));
    objectMesh.addVertex(ofVec3f(-0.1792838, -0.006500653, 1.407272));
    objectMesh.addVertex(ofVec3f(-0.08071751, -0.0302555, 1.455986));
    objectMesh.addVertex(ofVec3f(-0.06267674, 0.01517509, 1.451872));
    objectMesh.addVertex(ofVec3f(-0.05193704, 0.0604865, 1.452582));
    objectMesh.addVertex(ofVec3f(-0.04030832, 0.009344185, 1.487743));
    objectMesh.addVertex(ofVec3f(-0.1935772, -0.0009553147, 1.53593));
    objectMesh.addVertex(ofVec3f(-0.1872116, 0.04750633, 1.540086));
    objectMesh.addVertex(ofVec3f(-0.1745261, 0.09231257, 1.539418));
    objectMesh.addVertex(ofVec3f(-0.1628973, 0.04117024, 1.574579));

    float texWidth = 1024.0;
    float texHeight = 1024.0;
    objectMesh.addTexCoord(ofVec2f(0.510309636592865 * texWidth, 0.8387101888656616 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5539760589599609 * texWidth, 0.7021119594573975 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5049493908882141 * texWidth, 0.6237664818763733 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5041682124137878 * texWidth, 0.5502831339836121 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0, 0));
    objectMesh.addTexCoord(ofVec2f(0.5039656162261963 * texWidth, 0.3991791903972626 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.503989577293396 * texWidth, 0.38172560930252075 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5043075084686279 * texWidth, 0.3094716966152191 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5046930909156799 * texWidth, 0.2676190733909607 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.504898190498352 * texWidth, 0.24521425366401672 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5063571333885193 * texWidth, 0.1489759385585785 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5883102416992188 * texWidth, 0.8257706165313721 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6650840044021606 * texWidth, 0.7692070007324219 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6332414746284485 * texWidth, 0.6620619893074036 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.7167770266532898 * texWidth, 0.6518232822418213 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6667811274528503 * texWidth, 0.60102778673172 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5949824452400208 * texWidth, 0.5999851226806641 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5164838433265686 * texWidth, 0.5342773795127869 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5956381559371948 * texWidth, 0.5859565138816833 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5950102806091309 * texWidth, 0.5418456792831421 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6537569761276245 * texWidth, 0.5210034847259521 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5967782139778137 * texWidth, 0.530368447303772 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5996527075767517 * texWidth, 0.5011851191520691 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5532241463661194 * texWidth, 0.5102176666259766 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5999793410301208 * texWidth, 0.4979189336299896 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5492759346961975 * texWidth, 0.4188147783279419 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5651370286941528 * texWidth, 0.39037150144577026 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6335221529006958 * texWidth, 0.43914663791656494 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.7435698509216309 * texWidth, 0.39984455704689026 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.7367715239524841 * texWidth, 0.53080815076828 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.7408248782157898 * texWidth, 0.2723451256752014 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5697627067565918 * texWidth, 0.29673510789871216 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5882582068443298 * texWidth, 0.16878268122673035 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5132738351821899 * texWidth, 0.3238661289215088 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.45849621295928955 * texWidth, 0.7046968340873718 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0, 0));
    objectMesh.addTexCoord(ofVec2f(0, 0));
    objectMesh.addTexCoord(ofVec2f(0, 0));
    objectMesh.addTexCoord(ofVec2f(0, 0));
    objectMesh.addTexCoord(ofVec2f(0, 0));
    objectMesh.addTexCoord(ofVec2f(0.504499077796936 * texWidth, 0.29073745012283325 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0, 0));
    objectMesh.addTexCoord(ofVec2f(0, 0));
    objectMesh.addTexCoord(ofVec2f(0, 0));
    objectMesh.addTexCoord(ofVec2f(0.42924875020980835 * texWidth, 0.8356434106826782 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.3462582528591156 * texWidth, 0.7787892818450928 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.3762294352054596 * texWidth, 0.6667187809944153 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.29018670320510864 * texWidth, 0.6579574942588806 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.34066036343574524 * texWidth, 0.6049576997756958 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.4137619137763977 * texWidth, 0.6023330688476562 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.49151504039764404 * texWidth, 0.5343296527862549 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.41286057233810425 * texWidth, 0.5881300568580627 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.4127095639705658 * texWidth, 0.5425248146057129 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.35294315218925476 * texWidth, 0.5220999717712402 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.4108029305934906 * texWidth, 0.5309192538261414 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.40768906474113464 * texWidth, 0.5014103651046753 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.4545511305332184 * texWidth, 0.5099305510520935 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.40736889839172363 * texWidth, 0.49810972809791565 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.45849865674972534 * texWidth, 0.4189603328704834 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.4426240921020508 * texWidth, 0.3903966546058655 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.3734146058559418 * texWidth, 0.4397892355918884 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.2613128423690796 * texWidth, 0.39965391159057617 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.2683982253074646 * texWidth, 0.5337236523628235 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.265094131231308 * texWidth, 0.26912224292755127 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.43870818614959717 * texWidth, 0.2960324287414551 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.42267900705337524 * texWidth, 0.16618216037750244 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.49517813324928284 * texWidth, 0.32378754019737244 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6102035641670227 * texWidth, 0.5275691151618958 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6119749546051025 * texWidth, 0.5084928274154663 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.397220253944397 * texWidth, 0.5282546281814575 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.39525458216667175 * texWidth, 0.5089247822761536 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5882201194763184 * texWidth, 0.523379921913147 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5901689529418945 * texWidth, 0.5072464942932129 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.41941896080970764 * texWidth, 0.5237345099449158 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.41729313135147095 * texWidth, 0.5074490904808044 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5230320692062378 * texWidth, 0.39671292901039124 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.48487868905067444 * texWidth, 0.3967296779155731 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5107394456863403 * texWidth, 0.5254592299461365 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.49740248918533325 * texWidth, 0.5255225896835327 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5421306490898132 * texWidth, 0.31406864523887634 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.46633464097976685 * texWidth, 0.31372907757759094 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5364420413970947 * texWidth, 0.29432693123817444 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.47225522994995117 * texWidth, 0.29394716024398804 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5352165699005127 * texWidth, 0.29086780548095703 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.47364774346351624 * texWidth, 0.2905750870704651 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5401625633239746 * texWidth, 0.26892316341400146 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.4690127670764923 * texWidth, 0.26838549971580505 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5043967962265015 * texWidth, 0.29596659541130066 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5557450652122498 * texWidth, 0.2933984398841858 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.4528862535953522 * texWidth, 0.2928259074687958 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6232001781463623 * texWidth, 0.3134484887123108 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.3844582438468933 * texWidth, 0.31236353516578674 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5301335453987122 * texWidth, 0.47008588910102844 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.47762954235076904 * texWidth, 0.47032132744789124 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5039123296737671 * texWidth, 0.47778788208961487 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6224626898765564 * texWidth, 0.5349976420402527 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.38484901189804077 * texWidth, 0.5359857082366943 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6225872039794922 * texWidth, 0.530196487903595 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.3846780061721802 * texWidth, 0.5310977101325989 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6249498128890991 * texWidth, 0.5106076002120972 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.38216066360473633 * texWidth, 0.5112110376358032 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.6279229521751404 * texWidth, 0.5069273710250854 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.3791465759277344 * texWidth, 0.5075022578239441 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5759710073471069 * texWidth, 0.5278387665748596 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.4318109452724457 * texWidth, 0.5280867218971252 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5765642523765564 * texWidth, 0.5242598652839661 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.4311807155609131 * texWidth, 0.52447110414505 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5779321789741516 * texWidth, 0.5055827498435974 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.42965245246887207 * texWidth, 0.5055953860282898 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5774850249290466 * texWidth, 0.5020829439163208 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.4300956130027771 * texWidth, 0.5020501017570496 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.5391964316368103 * texWidth, 0.3828592598438263 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.46871957182884216 * texWidth, 0.38281580805778503 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.12939292192459106 * texWidth, 0.408208966255188 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.14490535855293274 * texWidth, 0.5513002276420593 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.16891244053840637 * texWidth, 0.6848285794258118 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.021442867815494537 * texWidth, 0.5687134861946106 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.8719887137413025 * texWidth, 0.40840235352516174 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.8570339679718018 * texWidth, 0.5472267866134644 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.8344606161117554 * texWidth, 0.6765055656433105 * texHeight));
    objectMesh.addTexCoord(ofVec2f(0.976693868637085 * texWidth, 0.5637580156326294 * texHeight));

    objectMesh.addTriangle(11, 0, 1);
    objectMesh.addTriangle(1, 0, 34);
    objectMesh.addTriangle(34, 0, 44);
    objectMesh.addTriangle(12, 11, 1);
    objectMesh.addTriangle(34, 44, 45);
    objectMesh.addTriangle(12, 1, 13);
    objectMesh.addTriangle(13, 1, 2);
    objectMesh.addTriangle(2, 1, 34);
    objectMesh.addTriangle(46, 2, 34);
    objectMesh.addTriangle(46, 34, 45);
    objectMesh.addTriangle(14, 12, 13);
    objectMesh.addTriangle(14, 13, 15);
    objectMesh.addTriangle(15, 13, 16);
    objectMesh.addTriangle(13, 2, 16);
    objectMesh.addTriangle(16, 2, 17);
    objectMesh.addTriangle(17, 2, 3);
    objectMesh.addTriangle(3, 2, 50);
    objectMesh.addTriangle(50, 2, 49);
    objectMesh.addTriangle(49, 2, 46);
    objectMesh.addTriangle(49, 46, 48);
    objectMesh.addTriangle(48, 46, 47);
    objectMesh.addTriangle(46, 45, 47);
    objectMesh.addTriangle(29, 14, 15);
    objectMesh.addTriangle(29, 15, 20);
    objectMesh.addTriangle(15, 18, 19);
    objectMesh.addTriangle(16, 18, 15);
    objectMesh.addTriangle(18, 16, 17);
    objectMesh.addTriangle(18, 17, 19);
    objectMesh.addTriangle(23, 17, 77);
    objectMesh.addTriangle(17, 3, 77);
    objectMesh.addTriangle(78, 3, 50);
    objectMesh.addTriangle(56, 78, 50);
    objectMesh.addTriangle(52, 50, 51);
    objectMesh.addTriangle(50, 49, 51);
    objectMesh.addTriangle(49, 48, 51);
    objectMesh.addTriangle(51, 48, 52);
    objectMesh.addTriangle(53, 48, 62);
    objectMesh.addTriangle(48, 47, 62);
    objectMesh.addTriangle(28, 29, 27);
    objectMesh.addTriangle(29, 20, 27);
    objectMesh.addTriangle(26, 24, 25);
    objectMesh.addTriangle(58, 57, 59);
    objectMesh.addTriangle(60, 53, 62);
    objectMesh.addTriangle(60, 62, 61);
    objectMesh.addTriangle(26, 111, 33);
    objectMesh.addTriangle(26, 75, 111);
    objectMesh.addTriangle(25, 75, 26);
    objectMesh.addTriangle(59, 76, 58);
    objectMesh.addTriangle(112, 76, 59);
    objectMesh.addTriangle(66, 112, 59);
    objectMesh.addTriangle(33, 6, 7);
    objectMesh.addTriangle(7, 6, 66);
    objectMesh.addTriangle(32, 9, 10);
    objectMesh.addTriangle(10, 9, 65);
    objectMesh.addTriangle(76, 6, 5);
    objectMesh.addTriangle(5, 6, 75);
    objectMesh.addTriangle(77, 3, 78);
    objectMesh.addTriangle(33, 7, 79);
    objectMesh.addTriangle(79, 7, 81);
    objectMesh.addTriangle(81, 7, 87);
    objectMesh.addTriangle(80, 7, 66);
    objectMesh.addTriangle(82, 7, 80);
    objectMesh.addTriangle(87, 7, 82);
    objectMesh.addTriangle(82, 80, 89);
    objectMesh.addTriangle(89, 80, 64);
    objectMesh.addTriangle(88, 79, 81);
    objectMesh.addTriangle(31, 79, 88);
    objectMesh.addTriangle(79, 26, 33);
    objectMesh.addTriangle(31, 26, 79);
    objectMesh.addTriangle(66, 59, 80);
    objectMesh.addTriangle(80, 59, 64);
    objectMesh.addTriangle(88, 83, 85);
    objectMesh.addTriangle(88, 85, 31);
    objectMesh.addTriangle(83, 8, 85);
    objectMesh.addTriangle(83, 40, 8);
    objectMesh.addTriangle(8, 40, 84);
    objectMesh.addTriangle(8, 84, 86);
    objectMesh.addTriangle(89, 86, 84);
    objectMesh.addTriangle(64, 86, 89);
    objectMesh.addTriangle(9, 85, 8);
    objectMesh.addTriangle(9, 8, 86);
    objectMesh.addTriangle(85, 32, 31);
    objectMesh.addTriangle(9, 32, 85);
    objectMesh.addTriangle(65, 86, 64);
    objectMesh.addTriangle(65, 9, 86);
    objectMesh.addTriangle(26, 27, 24);
    objectMesh.addTriangle(30, 90, 32);
    objectMesh.addTriangle(32, 90, 31);
    objectMesh.addTriangle(90, 30, 28);
    objectMesh.addTriangle(90, 26, 31);
    objectMesh.addTriangle(90, 27, 26);
    objectMesh.addTriangle(90, 28, 27);
    objectMesh.addTriangle(60, 59, 57);
    objectMesh.addTriangle(65, 91, 63);
    objectMesh.addTriangle(64, 91, 65);
    objectMesh.addTriangle(91, 61, 63);
    objectMesh.addTriangle(91, 64, 59);
    objectMesh.addTriangle(91, 59, 60);
    objectMesh.addTriangle(91, 60, 61);
    objectMesh.addTriangle(77, 92, 23);
    objectMesh.addTriangle(23, 92, 25);
    objectMesh.addTriangle(25, 92, 75);
    objectMesh.addTriangle(56, 93, 78);
    objectMesh.addTriangle(76, 93, 58);
    objectMesh.addTriangle(58, 93, 56);
    objectMesh.addTriangle(77, 94, 92);
    objectMesh.addTriangle(92, 94, 75);
    objectMesh.addTriangle(75, 94, 5);
    objectMesh.addTriangle(5, 94, 76);
    objectMesh.addTriangle(76, 94, 93);
    objectMesh.addTriangle(93, 94, 78);
    objectMesh.addTriangle(78, 94, 77);
    objectMesh.addTriangle(95, 20, 15);
    objectMesh.addTriangle(97, 20, 95);
    objectMesh.addTriangle(101, 20, 99);
    objectMesh.addTriangle(27, 20, 101);
    objectMesh.addTriangle(19, 95, 15);
    objectMesh.addTriangle(21, 95, 19);
    objectMesh.addTriangle(97, 95, 21);
    objectMesh.addTriangle(27, 101, 24);
    objectMesh.addTriangle(24, 101, 22);
    objectMesh.addTriangle(22, 101, 99);
    objectMesh.addTriangle(23, 103, 17);
    objectMesh.addTriangle(23, 105, 103);
    objectMesh.addTriangle(23, 109, 107);
    objectMesh.addTriangle(23, 25, 109);
    objectMesh.addTriangle(17, 103, 19);
    objectMesh.addTriangle(19, 103, 21);
    objectMesh.addTriangle(21, 103, 105);
    objectMesh.addTriangle(107, 109, 22);
    objectMesh.addTriangle(22, 109, 24);
    objectMesh.addTriangle(24, 109, 25);
    objectMesh.addTriangle(104, 56, 50);
    objectMesh.addTriangle(106, 56, 104);
    objectMesh.addTriangle(110, 56, 108);
    objectMesh.addTriangle(58, 56, 110);
    objectMesh.addTriangle(52, 104, 50);
    objectMesh.addTriangle(54, 104, 52);
    objectMesh.addTriangle(106, 104, 54);
    objectMesh.addTriangle(55, 110, 108);
    objectMesh.addTriangle(57, 110, 55);
    objectMesh.addTriangle(58, 110, 57);
    objectMesh.addTriangle(48, 53, 96);
    objectMesh.addTriangle(53, 98, 96);
    objectMesh.addTriangle(100, 53, 102);
    objectMesh.addTriangle(102, 53, 60);
    objectMesh.addTriangle(48, 96, 52);
    objectMesh.addTriangle(52, 96, 54);
    objectMesh.addTriangle(54, 96, 98);
    objectMesh.addTriangle(100, 102, 55);
    objectMesh.addTriangle(55, 102, 57);
    objectMesh.addTriangle(57, 102, 60);
    objectMesh.addTriangle(6, 111, 75);
    objectMesh.addTriangle(33, 111, 6);
    objectMesh.addTriangle(76, 112, 6);
    objectMesh.addTriangle(6, 112, 66);
    objectMesh.addTriangle(74, 73, 70);
    objectMesh.addTriangle(70, 73, 69);
    objectMesh.addTriangle(68, 67, 72);
    objectMesh.addTriangle(72, 67, 71);
    objectMesh.addTriangle(69, 53, 70);
    objectMesh.addTriangle(74, 56, 73);
    objectMesh.addTriangle(71, 23, 72);
    objectMesh.addTriangle(68, 20, 67);
    objectMesh.addTriangle(69, 98, 53);
    objectMesh.addTriangle(69, 54, 98);
    objectMesh.addTriangle(73, 54, 69);
    objectMesh.addTriangle(106, 54, 73);
    objectMesh.addTriangle(56, 106, 73);
    objectMesh.addTriangle(108, 56, 74);
    objectMesh.addTriangle(74, 55, 108);
    objectMesh.addTriangle(70, 55, 74);
    objectMesh.addTriangle(100, 55, 70);
    objectMesh.addTriangle(53, 100, 70);
    objectMesh.addTriangle(67, 20, 97);
    objectMesh.addTriangle(67, 97, 21);
    objectMesh.addTriangle(67, 21, 71);
    objectMesh.addTriangle(71, 21, 105);
    objectMesh.addTriangle(71, 105, 23);
    objectMesh.addTriangle(99, 20, 68);
    objectMesh.addTriangle(99, 68, 22);
    objectMesh.addTriangle(72, 22, 68);
    objectMesh.addTriangle(107, 22, 72);
    objectMesh.addTriangle(23, 107, 72);
    objectMesh.addTriangle(63, 61, 113);
    objectMesh.addTriangle(61, 62, 113);
    objectMesh.addTriangle(113, 62, 114);
    objectMesh.addTriangle(62, 47, 114);
    objectMesh.addTriangle(114, 47, 115);
    objectMesh.addTriangle(47, 45, 115);
    objectMesh.addTriangle(113, 114, 116);
    objectMesh.addTriangle(114, 115, 116);
    objectMesh.addTriangle(28, 30, 117);
    objectMesh.addTriangle(29, 28, 117);
    objectMesh.addTriangle(29, 117, 118);
    objectMesh.addTriangle(14, 29, 118);
    objectMesh.addTriangle(14, 118, 119);
    objectMesh.addTriangle(12, 14, 119);
    objectMesh.addTriangle(118, 117, 120);
    objectMesh.addTriangle(119, 118, 120);
    objectMesh.addTriangle(88, 81, 83);
    objectMesh.addTriangle(81, 40, 83);
    objectMesh.addTriangle(82, 89, 84);
    objectMesh.addTriangle(87, 82, 84);
    objectMesh.addTriangle(81, 87, 40);
    objectMesh.addTriangle(87, 84, 40);

}
void testApp::setupMesh() {
    //texture.loadImage("faltenTest.png");
    constructMesh();
    /*
	model.loadModel("KinectFace.obj", false);
	objectMesh = model.getMesh(0);

	cout<<"numVerts:"<<objectMesh.getNumVertices()<<endl;

	vector<ofVec3f>& verts = objectMesh.getVertices();
	cout<<"verts:"<<endl;
	for(int i = 0; i < verts.size(); i++){
        cout<<verts[i].x<<" "<<verts[i].y<<" "<<verts[i].z<<endl;
	}
	cout<<endl;
	//objectMesh.get
    */
    maxCalibPoints = 42;
    currCalibPoints = 0;
    objectPoints.resize(maxCalibPoints);
    imagePoints.resize(maxCalibPoints);

}

void testApp::updateTrackingAge(){
    howStrong = 1.0;
    float ttiv = getf("timeTrackingIsValid");
    if(ttiv > 0.0){
        float trackingAge = ofGetElapsedTimef() - fReceiver->lastFaceReceivedTime;
        if(trackingAge > ttiv){
            //cout<<"not rendering"<<endl;
            //return;
            howStrong = 0.0;
        }
        else if(trackingAge > 0.0) howStrong = ttiv / trackingAge;
    }
    //synt->volume = howStrong / 10.0;
}

//not doing this if in render mode and calibration not ready jet!
void testApp::render() {
	ofPushStyle();
	ofSetLineWidth(geti("lineWidth"));
	if(getb("useSmoothing")) {
		ofEnableSmoothing();
	} else {
		ofDisableSmoothing();
	}
	int shading = geti("shading");
	bool useLights = shading == 1;
	bool useShader = shading == 2;
	if(useLights) {
		light.enable();
		ofEnableLighting();
		glShadeModel(GL_SMOOTH);
		glEnable(GL_NORMALIZE);
	}


	ofSetColor(255);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_DEPTH_TEST);

	if(useShader) {
		ofFile fragFile("shader.frag"), vertFile("shader.vert");
		Poco::Timestamp fragTimestamp = fragFile.getPocoFile().getLastModified();
		Poco::Timestamp vertTimestamp = vertFile.getPocoFile().getLastModified();
		if(fragTimestamp != lastFragTimestamp || vertTimestamp != lastVertTimestamp) {
			bool validShader = shader.load("shader");
			setb("validShader", validShader);
		}
		lastFragTimestamp = fragTimestamp;
		lastVertTimestamp = vertTimestamp;

		shader.begin();
		shader.setUniform1f("elapsedTime", ofGetElapsedTimef());
		shader.setUniform1f("howStrong", howStrong);//todo
		shader.end();
	}
	ofColor transparentBlack(0, 0, 0, 0);
	switch(geti("drawMode")) {
		case 0: // faces
            //texture.bind();
            myPlayer.getTextureReference().bind();
			if(useShader) shader.begin();
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			objectMesh.drawFaces();
			if(useShader) shader.end();
			myPlayer.getTextureReference().unbind();
			//texture.unbind();
			break;
		case 1: // fullWireframe
			if(useShader) shader.begin();
			objectMesh.drawWireframe();
			if(useShader) shader.end();
			break;
		case 2: // outlineWireframe
			LineArt::draw(objectMesh, true, transparentBlack, useShader ? &shader : NULL);
			break;
		case 3: // occludedWireframe
			LineArt::draw(objectMesh, false, transparentBlack, useShader ? &shader : NULL);
			break;
        case 4: // pointCloud
			if(useShader) shader.begin();
			objectMesh.drawVertices();
			if(useShader) shader.end();
			break;
        case 5: // noseOnly
            if(useShader) shader.begin();
			//objectMesh.drawVertices();

			ofSetColor(255, 0, 0);//draw a red nose
            ofSphere(objectMesh.getVertex(5), 0.005);

			ofSetColor(0, 255, 0);//draw green cheeks
            ofSphere(objectMesh.getVertex(28), 0.005);
            ofSphere(objectMesh.getVertex(61), 0.005);
            ofSphere(objectMesh.getVertex(30), 0.005);
            ofSphere(objectMesh.getVertex(63), 0.005);

            ofSetColor(0, 0, 255);//chin
            ofSphere(objectMesh.getVertex(42), 0.005);

			if(useShader) shader.end();
			break;
	}

	glPopAttrib();
	if(useLights) {
		ofDisableLighting();
	}
	ofPopStyle();

}

void testApp::saveCalibration() {
	string dirName = "calibration-" + ofGetTimestampString() + "/";
	ofDirectory dir(dirName);
	dir.create();

	FileStorage fs(ofToDataPath(dirName + "calibration-advanced.yml"), FileStorage::WRITE);

	Mat cameraMatrix = intrinsics.getCameraMatrix();
	fs << "cameraMatrix" << cameraMatrix;

	double focalLength = intrinsics.getFocalLength();
	fs << "focalLength" << focalLength;

	Point2d fov = intrinsics.getFov();
	fs << "fov" << fov;

	Point2d principalPoint = intrinsics.getPrincipalPoint();
	fs << "principalPoint" << principalPoint;

	cv::Size imageSize = intrinsics.getImageSize();
	fs << "imageSize" << imageSize;

	fs << "translationVector" << tvec;
	fs << "rotationVector" << rvec;

	Mat rotationMatrix;
	Rodrigues(rvec, rotationMatrix);
	fs << "rotationMatrix" << rotationMatrix;

	double rotationAngleRadians = norm(rvec, NORM_L2);
	double rotationAngleDegrees = ofRadToDeg(rotationAngleRadians);
	Mat rotationAxis = rvec / rotationAngleRadians;
	fs << "rotationAngleRadians" << rotationAngleRadians;
	fs << "rotationAngleDegrees" << rotationAngleDegrees;
	fs << "rotationAxis" << rotationAxis;

	ofVec3f axis(rotationAxis.at<double>(0), rotationAxis.at<double>(1), rotationAxis.at<double>(2));
	ofVec3f euler = ofQuaternion(rotationAngleDegrees, axis).getEuler();
	Mat eulerMat = (Mat_<double>(3,1) << euler.x, euler.y, euler.z);
	fs << "euler" << eulerMat;

	ofFile basic("calibration-basic.txt", ofFile::WriteOnly);
	ofVec3f position( tvec.at<double>(1), tvec.at<double>(2));
	basic << "position (in world units):" << endl;
	basic << "\tx: " << ofToString(tvec.at<double>(0), 2) << endl;
	basic << "\ty: " << ofToString(tvec.at<double>(1), 2) << endl;
	basic << "\tz: " << ofToString(tvec.at<double>(2), 2) << endl;
	basic << "axis-angle rotation (in degrees):" << endl;
	basic << "\taxis x: " << ofToString(axis.x, 2) << endl;
	basic << "\taxis y: " << ofToString(axis.y, 2) << endl;
	basic << "\taxis z: " << ofToString(axis.z, 2) << endl;
	basic << "\tangle: " << ofToString(rotationAngleDegrees, 2) << endl;
	basic << "euler rotation (in degrees):" << endl;
	basic << "\tx: " << ofToString(euler.x, 2) << endl;
	basic << "\ty: " << ofToString(euler.y, 2) << endl;
	basic << "\tz: " << ofToString(euler.z, 2) << endl;
	basic << "fov (in degrees):" << endl;
	basic << "\thorizontal: " << ofToString(fov.x, 2) << endl;
	basic << "\tvertical: " << ofToString(fov.y, 2) << endl;
	basic << "principal point (in screen units):" << endl;
	basic << "\tx: " << ofToString(principalPoint.x, 2) << endl;
	basic << "\ty: " << ofToString(principalPoint.y, 2) << endl;
	basic << "image size (in pixels):" << endl;
	basic << "\tx: " << ofToString(principalPoint.x, 2) << endl;
	basic << "\ty: " << ofToString(principalPoint.y, 2) << endl;

	saveMat(Mat(objectPoints), dirName + "objectPoints.yml");
	saveMat(Mat(imagePoints), dirName + "imagePoints.yml");
}

void testApp::setupControlPanel() {
	panel.setup(300, 700);
	panel.msg = "tab hides the panel, space or middle mouse toggles render/selection mode, 'f' toggles fullscreen.";

	panel.addPanel("Interaction");
	panel.addToggle("setupMode", true);
	panel.addSlider("scale", 10, .1, 25);
	//panel.addSlider("backgroundColor", 128, 0, 255, true);
	panel.addSlider("backgroundColorR", 110, 0, 255, true);
	panel.addSlider("backgroundColorG", 85, 0, 255, true);
	panel.addSlider("backgroundColorB", 75, 0, 255, true);
	panel.addSlider("gradientScaleX", 1, .1, 2);
	panel.addSlider("gradientScaleY", 1, .1, 2);
	panel.addToggle("multiplyGradient", false);
	panel.addMultiToggle("drawMode", 5, variadic("faces")("fullWireframe")("outlineWireframe")("occludedWireframe")("pointCloud")("noseOnly"));
	panel.addMultiToggle("shading", 0, variadic("none")("lights")("shader"));
	panel.addSlider("timeTrackingIsValid" ,0, 0, 0.2);//if last tracking is to old nothing is rendered in render mode. if set to 0 it-s allways rendered.
	panel.addToggle("saveCalibration", false);

	panel.addPanel("Calibration"); //todo: study these
	panel.addSlider("aov", 80, 50, 100);
	panel.addToggle("CV_CALIB_FIX_ASPECT_RATIO", true);
	panel.addToggle("CV_CALIB_FIX_K1", true);
	panel.addToggle("CV_CALIB_FIX_K2", true);
	panel.addToggle("CV_CALIB_FIX_K3", true);
	panel.addToggle("CV_CALIB_ZERO_TANGENT_DIST", true);
	panel.addToggle("CV_CALIB_FIX_PRINCIPAL_POINT", false);

	panel.addPanel("Rendering");
	panel.addSlider("lineWidth", 2, 1, 8, true);
	panel.addToggle("useSmoothing", false);
	panel.addSlider("screenPointSize", 2, 1, 16, true);
	panel.addSlider("selectedPointSize", 8, 1, 16, true);
	panel.addSlider("selectionRadius", 12, 1, 32);
	panel.addSlider("lightX", 200, -1000, 1000);
	panel.addSlider("lightY", 400, -1000, 1000);
	panel.addSlider("lightZ", 800, -1000, 1000);
	panel.addToggle("randomLighting", false);

	panel.addPanel("Internal");
	panel.addToggle("validShader", true);
	panel.addToggle("selectionMode", true);
	panel.addToggle("hoverSelected", false);
	panel.addSlider("hoverChoice", 0, 0, objectMesh.getNumVertices(), true);
	panel.addToggle("selected", false);
	panel.addSlider("selectionChoice", 0, 0, objectMesh.getNumVertices(), true);
	panel.addSlider("slowLerpRate", .001, 0, .01);
	panel.addSlider("fastLerpRate", 1, 0, 1);
}

void testApp::updateRenderMode() {
	// generate camera matrix given aov guess
	float aov = getf("aov");
	Size2i imageSize(ofGetWidth(), ofGetHeight());
	float f = imageSize.width * ofDegToRad(aov); // i think this is wrong, but it's optimized out anyway
	Point2f c = Point2f(imageSize) * (1. / 2);
	Mat1d cameraMatrix = (Mat1d(3, 3) <<
		f, 0, c.x,
		0, f, c.y,
		0, 0, 1);

	// generate flags
	#define getFlag(flag) (panel.getValueB((#flag)) ? flag : 0)
	int flags =
		CV_CALIB_USE_INTRINSIC_GUESS |
		getFlag(CV_CALIB_FIX_PRINCIPAL_POINT) |
		getFlag(CV_CALIB_FIX_ASPECT_RATIO) |
		getFlag(CV_CALIB_FIX_K1) |
		getFlag(CV_CALIB_FIX_K2) |
		getFlag(CV_CALIB_FIX_K3) |
		getFlag(CV_CALIB_ZERO_TANGENT_DIST);

	vector<Mat> rvecs, tvecs;
	Mat distCoeffs;
	vector<vector<Point3f> > referenceObjectPoints(1);
	vector<vector<Point2f> > referenceImagePoints(1);
	//int n = referencePoints.size();

	for(int i = 0; i < currCalibPoints; i++) {
        referenceObjectPoints[0].push_back(objectPoints[i]);
        referenceImagePoints[0].push_back(imagePoints[i]);
	}
	const static int minPoints = 6;
	if(referenceObjectPoints[0].size() >= minPoints) {
		calibrateCamera(referenceObjectPoints, referenceImagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, flags);
		rvec = rvecs[0];
		tvec = tvecs[0];
		intrinsics.setup(cameraMatrix, imageSize);
		modelMatrix = makeMatrix(rvec, tvec);
		calibrationReady = true;
	} else {
		calibrationReady = false;
	}
}

void testApp::drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg, ofColor fg) {
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_TEST);
	ofVec2f tooltipOffset(5, -25);
	ofSetColor(color);
	float w = ofGetWidth();
	float h = ofGetHeight();
	ofSetLineWidth(1.5);
	ofLine(position - ofVec2f(w,0), position + ofVec2f(w,0));
	ofLine(position - ofVec2f(0,h), position + ofVec2f(0,h));
	ofCircle(position, geti("selectedPointSize"));
	drawHighlightString(ofToString(label), position + tooltipOffset, bg, fg);
	glPopAttrib();
}

void testApp::drawSelectionMode() {

	ofSetColor(255);
	cam.begin();
	float scale = getf("scale");
	ofScale(scale, scale, scale);
	render();

	if(getb("setupMode")) {
		imageMesh = getProjectedMesh(objectMesh);
	}
	cam.end();

	if(getb("setupMode")) {
		// draw all points cyan small
		glPointSize(geti("screenPointSize"));
		glEnable(GL_POINT_SMOOTH);
		ofSetColor(cyanPrint);
		imageMesh.drawVertices();

		// draw hover point magenta
		int choice;
		float distance;
		ofVec3f selected = getClosestPointOnMesh(imageMesh, mouseX, mouseY, &choice, &distance);
		if(!ofGetMousePressed(MOUSE_SELECT_BUTTON) && distance < getf("selectionRadius")) {
			seti("hoverChoice", choice);
			setb("hoverSelected", true);
			drawLabeledPoint(choice, selected, magentaPrint);
		} else {
			setb("hoverSelected", false);
		}

		// draw selected point yellow
		if(getb("selected")) {
			int choice = geti("selectionChoice");
			ofVec2f selected = imageMesh.getVertex(choice);
			drawLabeledPoint(choice, selected, yellowPrint, ofColor::white, ofColor::black);
		}
	}

}

void testApp::multiplyLightConeFalloff(){
    ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
    //ofBackground(ofColor::black);
    ofPushMatrix();
    //float scale = getf("gradientScale");
    float halfW = ofGetWidth()/2.0;
    float halfH = ofGetHeight()/2.0;
    ofTranslate(halfW, halfH, 0);
    ofScale(getf("gradientScaleX"), getf("gradientScaleY"), 1);
    ofTranslate(-halfW, -halfH, 0);

    ofBackgroundGradient(ofColor::white,ofColor::black, OF_GRADIENT_CIRCULAR);
    ofPopMatrix();
    ofDisableBlendMode();
}

void testApp::drawRenderMode() {
	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);

	if(calibrationReady) {
		intrinsics.loadProjectionMatrix(0.1, 2000);
		applyMatrix(modelMatrix);
		render();
		if(getb("setupMode")) {
			imageMesh = getProjectedMesh(objectMesh);
		}
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);



	if(getb("setupMode")) {

		//draw the 'old' reference points. they cant be changed. dont know if it even makes sense to draw them...
		for(int i = 0; i < currCalibPoints; i++) {
            drawLabeledPoint(i, toOf(imagePoints[i]), cyanPrint);
		}

		// move the point that needs to be dragged
		// draw it yellow
		int choice = geti("selectionChoice");
		if(getb("selected")) {
			//referencePoints[choice] = true;

			//update current object point
            objectPoints[currCalibPoints-1] = toCv(objectMesh.getVertex(choice)); //is the objectMesh projected right?


			Point2f& cur = imagePoints[currCalibPoints-1];/////////////////

            cur = Point2f(mouseX, mouseY);

            drawLabeledPoint(choice, toOf(cur), yellowPrint, ofColor::white, ofColor::black);
            ofSetColor(ofColor::black);
            ofRect(toOf(cur), 1, 1);//black dot in the center
		}
	}
}

//--------------------------------------------------------------
/*
void testApp::audioOut(float * output, int bufferSize, int nChannels){

    updateTrackingAge();

	//pan = 0.5f;
	float leftScale = 1 - synt->pan;
	float rightScale = synt->pan;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (synt->phase > TWO_PI){
		synt->phase -= TWO_PI;
	}

	if ( synt->bNoise == true){
		// ---------------------- noise --------------
		for (int i = 0; i < bufferSize; i++){
			output[i*nChannels    ] = ofRandom(0, 1) * synt->volume * leftScale;
			output[i*nChannels + 1] = ofRandom(0, 1) * synt->volume * rightScale;
		}
	} else {
		synt->phaseAdder = 0.95f * synt->phaseAdder + 0.05f * synt->phaseAdderTarget;
		for (int i = 0; i < bufferSize; i++){
			synt->phase += synt->phaseAdder;
			float sample = sin(synt->phase);
			output[i*nChannels    ] = sample * synt->volume * leftScale;
			output[i*nChannels + 1] = sample * synt->volume * rightScale;
		}
	}

}
*/
