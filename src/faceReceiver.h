#include "ofMain.h"
#include "ofxNetwork.h"

class faceReceiver{
    public:
        faceReceiver();
        ~faceReceiver();
        bool update( vector<ofVec3f>& );
        void closeConnection();
        float lastFaceReceivedTime;

    private:
        int bufferSize;
        char* buffer;
        ofxUDPManager udpConnection;
        int nVerts;
        int nIndices;
        ofVbo faceMesh;
        //ofVec3f *v;
        string message;

};

