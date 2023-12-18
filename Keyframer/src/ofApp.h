
//
//  Starter file for Project 3 - Skeleton Builder
//
//  This file includes functionality that supports selection and translate/rotation
//  of scene objects using the mouse.
//
//  Modifer keys for rotatation are x, y and z keys (for each axis of rotation)
//
//  (c) Kevin M. Smith  - 24 September 2018
//

#pragma once

#include "ofMain.h"
#include "box.h"
#include "Primitives.h"
#include "joint.h"
#include "keyframe.h"
#include "ofxGui.h"

#include <map>

class ofApp : public ofBaseApp
{

	public:
		void setup();
		void update();
		void draw();
		void exit();

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
		static void drawAxis(glm::mat4 transform = glm::mat4(1.0), float len = 1.0);
		bool mouseToDragPlane(int x, int y, glm::vec3 &point);
		void printChannels(SceneObject *);
		bool objSelected() { return (selected.size() ? true : false ); };
		void createJoint();
		void deleteJoint(Joint* joint);
		void saveToFile(string filename);
		void loadFromFile(string filename);

		inline Joint* getJointPointer(SceneObject* obj)
		{
			// Returns joint pointer if obj is a joint, else nullptr
			for (Joint* joint : jointsList)
			{
				if (obj == joint)
				{
					return joint;
				}
			}
			return nullptr;
		}

		inline void topsort(vector<Joint*> &order, set<Joint*> &visited, Joint* joint)
		{
			if (visited.find(joint) != visited.end())
			{
				// Visited node, exit
				return;
			}

			visited.insert(joint);

			// Recursion topsort
			for (SceneObject* obj : joint->endSphere->childList)
			{
				Joint* childJoint = getJointPointer(obj);
				if (childJoint)
				{
					topsort(order, visited, childJoint);
				}
			}

			order.push_back(joint);
		}

		// Lights
		//
		ofLight light1;
	
		// Cameras
		//
		ofEasyCam  mainCam;
		ofCamera sideCam;
		ofCamera topCam;
		ofCamera  *theCam;    // set to current camera either mainCam or sideCam

		// Materials
		//
		ofMaterial material;


		// scene components
		//
		vector<SceneObject*> scene;
		vector<SceneObject*> selected;
		vector<Joint*> jointsList;
		map<string, Joint*> jointNames;
		ofPlanePrimitive plane;

		Plane* groundPlane = nullptr;

		// state
		static bool bDrawAxes;
		bool bDrag = false;
		bool bHide = true;
		bool bAltKeyDown = false;
		bool bRotateX = false;
		bool bRotateY = false;
		bool bRotateZ = false;
		glm::vec3 lastPoint;
		int numJoints;
		
		// keyframing
		Keyframer* keyframer;
		bool isAnimating;
		int totalFrames;

		// keyframing gui
		ofxPanel keyframingGui;
		ofxInputField<int> frameGoto;

		ofxPanel frameGui;
		ofxButton deleteScene;
		ofxButton createScene;

		void frameGotoListener(int &value)
		{
			if (frameGoto.containsValidValue())
			{
				keyframer->goToFrame(value, scene);
			}
		}

		void deleteSceneListener()
		{
			keyframer->deleteFrame();
		}

		void createSceneListener()
		{
			keyframer->saveFrame(scene);
		}
};