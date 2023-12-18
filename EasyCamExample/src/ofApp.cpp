#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundColor(ofColor::black);
    cam.setDistance(150);
    cam.setNearClip(0.1);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();

    ofPushMatrix();
    ofSetColor(ofColor::dimGray);
    ofRotateDeg(45);
    ofDrawGridPlane();
    ofPopMatrix();

    // draw objects here
    ofSetColor(ofColor::white);
    ofNoFill();
    ofDrawTriangle(glm::vec3(0, 0, 0), glm::vec3(10, 10, 10), glm::vec3(0, 10, 0));
    

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
            ofsetDistance(150);
            ofRotateDeg(45);
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
