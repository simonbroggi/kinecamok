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
    bufferSize = 1468;
    //char buffer[bufferSize]{};
    //cout<<"just init"<<endl;
    buffer = new char[bufferSize];
    //std::string buffer(bufferSize, 0);
    //http://stackoverflow.com/questions/3134458/how-to-initialize-a-char-array
    memset(reinterpret_cast<void*>(buffer), 0, bufferSize);
    //cout<<"oh comon"<<endl;
    //std::fill(buffer, buffer + bufferSize, 0);
    //cout<<"whatsup"<<endl;
    /*
    for(int i=0; i<bufferSize; i++){
        buffer[i] = '0';
        cout<<i<<",";
    }
    cout<<endl;
    */

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

    //udpConnection.SetReceiveBufferSize(bufferSize); //what's the point?
    //char udpMessage[bufferSize]; //probably this causes an error sometimes...


    udpConnection.Receive(buffer,bufferSize);
    string tmpMessage=buffer;
    unsigned short shouldbe = 1;

    //cout<<sizeof(shouldbe)<<endl;

    if(tmpMessage!=""){
        //cout<<endl<< sizeof(udpMessage)/sizeof(udpMessage[0]) <<"message:"<<endl<<tmpMessage<<endl;
        //sizes of the protocoll
        int header = 10;
        int faceHeader = 6;
        unsigned long tick = CHAR2UINT64(buffer,0);
        unsigned short numFaces = CHAR2UINT16(buffer,8);


        int faceHeaderIndex = header; //since only one face is supported this is easy

        unsigned short faceID = CHAR2UINT16(buffer, faceHeaderIndex);
        int numFacePoints = CHAR2INT32(buffer, faceHeaderIndex+2);
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
                memcpy(&verts[i].x, &buffer[facePointsIndex + i*4*3],     4);
                memcpy(&verts[i].y, &buffer[facePointsIndex + i*4*3 + 4], 4);
                memcpy(&verts[i].z, &buffer[facePointsIndex + i*4*3 + 8], 4);

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

