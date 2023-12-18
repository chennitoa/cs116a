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

	v0 = glm::vec3(3.0, 3.0, 3.0);
	v1 = glm::vec3(5.0, 6.0, 7.0);
	v2 = glm::vec3(11.0, -1.0, -1.0);

	// Radius is 1/20th the size of the triangle
	sphere_radius = (v0.length() + v1.length() + v2.length()) / 60.0;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw() {
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
	
	// Draw Triangle
	ofPushMatrix();
	ofSetColor(ofColor::yellow);

	ofDrawTriangle(v0, v1, v2);	

	// Draw mouse ray
	if (drawMouseRay)
	{
		ofSetColor(ofColor::mediumPurple);
		ofDrawLine(mouseRayOrigin, mouseRayOrigin + mouseRayDirection * 60);
	}

	if (drawSphere)
	{
		ofSetColor(ofColor::orange);
		ofFill();
		sphere.setRadius(sphere_radius);
		sphere.setPosition(sphere_position);
		sphere.draw();
	}

	// draw the easy cam
	//
	ofNoFill();
	ofSetColor(ofColor::yellow);
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
		{
			if (easyCam.getMouseInputEnabled())
			{
				easyCam.disableMouseInput();
			}
			else
			{
				easyCam.enableMouseInput();
			}
			break;
		}
		case OF_KEY_F1:
		{
			theCam = &easyCam;
			break;
		}
		case OF_KEY_F2:
		{
			theCam = &sideCam;
			break;
		}
		case OF_KEY_F3:
		{
			theCam = &topCam;
			break;
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

	// Calculate mouse on screen
	glm::vec3 mousePosition = easyCam.screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 cameraPosition = easyCam.getPosition();
	glm::vec3 mouseDirection = glm::normalize(mousePosition - cameraPosition);

	// Draw mouse ray
	drawMouseRay = true;
	mouseRayOrigin = mousePosition;
	mouseRayDirection = mouseDirection;

	if (!easyCam.getMouseInputEnabled())
	{
		glm::vec2 result;
		float distance;

		// Draw sphere
		drawSphere = false;
		if (glm::intersectRayTriangle(mousePosition, mouseDirection, v0, v1, v2, result, distance))
		{
			// result.x is v1, result.y is v2, 1 - result.x - result.y is v0
			cout << "Barycentric Coordinates: (" << 1 - result.x - result.y << ", " << result.x << ", " << result.y << ")" << endl;

			sphere_position = glm::vec3(v0 * (1 - result.x - result.y) + v1 * result.x + v2 * result.y);
			drawSphere = true;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	drawMouseRay = false;
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
