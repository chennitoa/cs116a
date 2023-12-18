#pragma once

#include "ofMain.h"

#include <glm/gtx/intersect.hpp>


class ofApp : public ofBaseApp {

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
		void drawAxis();
		
		ofEasyCam easyCam;
		ofCamera sideCam;
		ofCamera topCam;

		ofCamera *theCam;
		
		glm::vec3 v0;
		glm::vec3 v1;
		glm::vec3 v2;

		float sphere_radius;
		glm::vec3 sphere_position;
		bool drawSphere;
		ofSpherePrimitive sphere;

		glm::vec3 mouseRayOrigin;
		glm::vec3 mouseRayDirection;
		bool drawMouseRay;
};
