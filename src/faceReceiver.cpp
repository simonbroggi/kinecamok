#include "faceReceiver.h"
#include <iostream>

#define CHAR2UINT64(charArr,i) (unsigned char) charArr[i] + ((unsigned char) charArr[i+1] << 8) + ((unsigned char) charArr[i+2] << 16) + ((unsigned char) charArr[i+3] << 24) + ((unsigned char) charArr[i+4] << 32 )+ ((unsigned char) charArr[i+5] << 40) + ((unsigned char) charArr[i+6] << 48) + ((unsigned char) charArr[i+7] << 56)
#define CHAR2INT32(charArr,i) (unsigned char) charArr[i] + ((unsigned char) charArr[i+1] << 8) + ((unsigned char) charArr[i+2] << 16) + ((unsigned char) charArr[i+3] << 24)
#define CHAR2UINT16(charArr,i) (unsigned char) charArr[i] + ((unsigned char) charArr[i+1] << 8)
/*
#define CHAR2UINT64(charArr,i) charArr[i] | (charArr[i+1] << 8) | (charArr[i+2] << 16) | (charArr[i+3] << 24) | (charArr[i+4] << 32 ) | (charArr[i+5] << 40) | (charArr[i+6] << 48) | (charArr[i+7] << 56)
#define CHAR2INT32(charArr,i) charArr[i] | (charArr[i+1] << 8) | (charArr[i+2] << 16) | (charArr[i+3] << 24)
#define CHAR2UINT16(charArr,i) charArr[i] | (charArr[i+1] << 8)
*/

#define CHAR2SINGLE(charArr,i) (unsigned char) charArr[i] | ((unsigned char) charArr[i+1] << 8) | ((unsigned char) charArr[i+2] << 16) | ((unsigned char) charArr[i+3] << 24)

faceReceiver::faceReceiver(){

    //create the socket and bind to port 11001
	udpConnection.Create();
	udpConnection.Bind(11001);
	udpConnection.SetNonBlocking(true);

    lastFaceReceivedTime = - 100000;
}

faceReceiver::~faceReceiver(){
    closeConnection();
}

void faceReceiver::closeConnection(){
    udpConnection.Close();
}
/*
bool faceReceiver::update(vector<ofVec3f>& verts){
    char udpMessage[1024];
    //cout<<"receive"<<endl;
    udpConnection.Receive(udpMessage, 1024);
    string tmpMessage=udpMessage;
    if(tmpMessage!=""){
        //cout<<tmpMessage<<endl;
        return true;
    }
    return false;
}
*/
bool faceReceiver::update(vector<ofVec3f>& verts){
    int inArrayLen = 1468;
    //udpConnection.SetReceiveBufferSize(inArrayLen); //what's the point?
    char udpMessage[inArrayLen]; //probably this causes an error sometimes...
    udpConnection.Receive(udpMessage,inArrayLen);

    string tmpMessage=udpMessage;
    unsigned short shouldbe = 1;

    //cout<<sizeof(shouldbe)<<endl;

    if(tmpMessage!=""){
        //cout<<endl<< sizeof(udpMessage)/sizeof(udpMessage[0]) <<"message:"<<endl<<tmpMessage<<endl;

        //sizes of the protocoll
        int header = 10;
        int faceHeader = 6;

        unsigned long tick = CHAR2UINT64(udpMessage,0);
        unsigned short numFaces = CHAR2UINT16(udpMessage,8);


        int faceHeaderIndex = header; //since only one face is supported this is easy

        unsigned short faceID = CHAR2UINT16(udpMessage, faceHeaderIndex);
        int numFacePoints = CHAR2INT32(udpMessage, faceHeaderIndex+2);
        //cout<<tick<<" "<<numFaces<<" "<<faceID<<" has "<<numFacePoints<<" points"<<endl;
        int facePointsIndex = faceHeaderIndex + faceHeader;

        //cout<<endl<<endl<<"coords=[";

        for(int i = 0; i < verts.size(); i++){
            if(i>=numFacePoints){
                //cout<<"adsfasdfaasdf"<<endl;
                //wtf!? what's with all the extra verts? not doing anything with them dost update the others either, so just set them to 0
                //no longer needed, yahuuuuu :)
                verts[i].x = 0;
                verts[i].y = 0;
                verts[i].z = 0;
            }
            else{
                memcpy(&verts[i].x, &udpMessage[facePointsIndex + i*4*3],     4);
                memcpy(&verts[i].y, &udpMessage[facePointsIndex + i*4*3 + 4], 4);
                memcpy(&verts[i].z, &udpMessage[facePointsIndex + i*4*3 + 8], 4);

                //cout<<"("<<verts[i].x<<", "<<verts[i].y<<", "<<verts[i].z<<")";

                //if(i+1<numFacePoints) cout<<", ";
                //else cout<<"]"<<endl;
            }

        }

        lastFaceReceivedTime = ofGetElapsedTimef();
        return true;
    }
    return false;
}

