#include "simUtils.h"

void simUtils::multiplyVignette(float sx, float sy){
    float w = ofGetWidth(), h = ofGetHeight();

    ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
    //ofBackground(ofColor::black);
    ofPushMatrix();
    //float scale = getf("gradientScale");
    float halfW = w/2.0;
    float halfH = h/2.0;
    ofTranslate(halfW, halfH, 0);
    ofScale(sx, sy, 1);
    ofTranslate(-halfW, -halfH, 0);

    //mesh construction similar to that from ofBackgroundGradient(ofColor::white,ofColor::black, OF_GRADIENT_CIRCULAR);
    ofVec2f center(w / 2, h / 2);
    int n = 24; // circular gradient resolution
    float angleBisector = TWO_PI / (n * 2);
    float smallRadius = ofDist(0, 0, w / 2, h / 2);
    float bigRadius = smallRadius;//  / cos(angleBisector);
    // this could be optimized by building a single mesh once, then copying
    // it and just adding the colors whenever the function is called.
    /*
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    mesh.addVertex(center);
    mesh.addColor(ofColor::white);
    for(int i = 0; i <= n; i++) {
        float theta = i * TWO_PI / n;
        mesh.addVertex(center + ofVec2f(sin(theta), cos(theta)) * bigRadius);
        mesh.addColor(ofColor::black);
    }*/

    ofMesh mesh2;
    mesh2.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    for(int i = 0; i <= n; i++) {
        float theta = i * TWO_PI / n;
        ofVec2f smallPerformanceInc = ofVec2f(sin(theta), cos(theta));
        mesh2.addVertex(center + smallPerformanceInc * bigRadius);
        mesh2.addColor(ofColor::white);
        mesh2.addVertex(center + smallPerformanceInc * bigRadius*1.4);
        mesh2.addColor(ofColor::black);
    }

    ofMesh mesh3;
    mesh3.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    for(int i = 0; i <= n; i++) {
        float theta = i * TWO_PI / n;
        ofVec2f smallPerformanceInc = ofVec2f(sin(theta), cos(theta));
        mesh3.addVertex(center + smallPerformanceInc * bigRadius*1.4);
        mesh3.addColor(ofColor::black);
        mesh3.addVertex(center + smallPerformanceInc * bigRadius*4);
        mesh3.addColor(ofColor::black);
    }

    glDepthMask(false);
    //mesh.draw();
    mesh2.draw();
    mesh3.draw();
    glDepthMask(true);

    ofPopMatrix();
    ofDisableBlendMode();

}
