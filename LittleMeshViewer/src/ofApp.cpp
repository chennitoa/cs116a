#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundColor(ofColor::black);
    cam.setDistance(30);
    cam.setNearClip(0.1);
    ofEnableDepthTest();
    
    // Load Model
    if (model.loadModel("geo/monster.obj"))
    {
        bModelLoaded = true;
        model.setScaleNormalization(false);
        model.setPosition(4.15, 0, 0);
    }
    else
    {
        std::cout << "Can't load model." << endl;
        ofExit(1);
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();

    // Grid
    ofPushMatrix();
    ofSetColor(ofColor::dimGray);
    ofRotateDeg(90);
    ofDrawGridPlane();
    ofPopMatrix();

    // Draw objects here
    ofPushMatrix();
    if (bModelLoaded) {
        cout << "Value of bDrawFaces " << bDrawFaces << endl;
        if (bDrawFaces) model.drawFaces();
        model.drawWireframe();
    }
    ofPopMatrix();
    

    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    switch (key)
    {
        case 'f':
        {
            ofToggleFullscreen();
            break;
        }
        case 'r':
        {
            cam.setDistance(30);
        }
        case 'w':
        {
            bDrawFaces = !bDrawFaces;
        }
        default:
        {
            break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
