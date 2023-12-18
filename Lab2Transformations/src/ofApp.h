#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include <glm/gtx/transform.hpp>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		// Camera
		ofEasyCam cam;

		// Pyramid
		vector<glm::vec3> points;
		vector<glm::vec3> faces;

		// Gui base
		bool hideGui = false;
		ofxPanel gui;

		// Scale gui
		ofxPanel scaleGui;
		ofParameter<glm::vec3> scaleSlider;

		// Rotate gui
		ofxPanel rotateGui;
		ofParameter<float> rotateSlider;
		ofParameter<glm::vec3> rotateAxis;

		// Translate gui
		ofxPanel translateGui;
		ofParameter<glm::vec3> translateSlider;
		
		
};
