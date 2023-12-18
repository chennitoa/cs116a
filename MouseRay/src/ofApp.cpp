#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(ofColor::black);
	theCam = &easyCam;

	// setup cameras
	//
	easyCam.setDistance(20);
	easyCam.setNearClip(.1);

	sideCam.setPosition(glm::vec3(50, 0, 0));
	sideCam.lookAt(glm::vec3(0, 0, 0));
	sideCam.setNearClip(.1);

	topCam.setPosition(glm::vec3(0, 50, 0));
	topCam.lookAt(glm::vec3(0, 0, 0));
	topCam.setNearClip(.1);

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	theCam->begin();
	drawAxis();
	ofNoFill();
	
	// draw Grid
	//
	ofPushMatrix();
	ofSetColor(ofColor::dimGray);
	ofRotateDeg(90);
	ofDrawGridPlane();
	ofPopMatrix();

	// Test draw mouse Ray
	//
	//ofSetColor(ofColor::lightBlue);

	// example useage of r(t) = o + dt
	//
	if (bDrawMouseRay) {
		ofDrawLine(mouseRayOrigin, mouseRayOrigin + mouseRayDir * 60);
	}
	
	// draw the easy cam
	//
	easyCam.draw();

	theCam->end();
}

void ofApp::drawAxis() {
	ofPushMatrix();
	ofTranslate(glm::vec3(0, .02, 0)); // move up slightly
	ofSetColor(ofColor::red);
	ofDrawLine(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
	ofSetColor(ofColor::green);
	ofDrawLine(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	ofSetColor(ofColor::blue);
	ofDrawLine(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
	ofSetColor(ofColor::white);
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case 'c':
		if (easyCam.getMouseInputEnabled()) easyCam.disableMouseInput();
		else easyCam.enableMouseInput();
		break;
	case OF_KEY_F1:
		theCam = &easyCam;
		break;
	case OF_KEY_F2:
		theCam = &sideCam;
		break; 
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	default:
		break;
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

	// Compute Mouse Ray (test)
	//
     glm::vec3 q = easyCam.screenToWorld(glm::vec3(x, y, 0));
	 glm::vec3 p = easyCam.getPosition();
	 glm::vec3 d = glm::normalize(q - p);

	 bDrawMouseRay = true;
	 mouseRayDir = d;
	 mouseRayOrigin = p;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bDrawMouseRay = false;
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
