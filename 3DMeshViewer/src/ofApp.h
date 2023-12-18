#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <string>

#define NORMALCOLOR ofColor::blue
#define HIGHLIGHTEDCOLOR ofColor::yellow


typedef glm::vec3 Vertex;

class Face
{
	public:
		// Uses indices to represent vertices
		Face(unsigned int v1, unsigned int v2, unsigned int v3)
		{
			this->v1 = v1;
			this->v2 = v2;
			this->v3 = v3;
			isHighlighted = false;
		};

		unsigned int v1;
		unsigned int v2;
		unsigned int v3;
		bool isHighlighted;
};

class Mesh
{
	public:
		void loadMesh(std::string filename);
		void pushStringToMesh(std::string objString);

		vector<Vertex*> vertices;
		vector<Face*> faces;

		void draw();
		void highlightFacesFromVertex(unsigned int vertexIndex);
		void highlightFacesFromFace(unsigned int faceIndex);
};

class ofApp : public ofBaseApp
{
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

		// Mesh
		Mesh mesh3D;
		void loadMeshFromFile(std::string filename);

		// Gui
		ofxPanel gui;
		ofxIntSlider vertexSelect;
		ofxButton vertexHighlight;
		ofxIntSlider faceSelect;
		ofxButton faceHighlight;

		bool hideGui;

		void vertexHighlightPressed();
		void faceHighlightPressed();
};
