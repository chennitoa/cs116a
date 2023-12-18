
//
//  Starter file for Project 3 - Skeleton Builder
//
//  This file includes functionality that supports selection and translate/rotation
//  of scene objects using the mouse.
//
//  Modifer keys for rotatation are x, y and z keys (for each axis of rotation)
//
//  Also demonstates simple keyframing and playback (With 2 keys)
//
//  (c) Kevin M. Smith  - 3 December 2019
//

#include "ofMain.h"
#include "box.h"
#include "Primitives.h"

class KeyFrame {
public:
	int frame = -1;     //  -1 => no key is set;
	glm::vec3 position = glm::vec3(0, 0, 0);   // translate channel
	glm::vec3 rotation = glm::vec3(0, 0, 0);   // rotate channel
	glm::vec3 scale  = glm::vec3(1, 1, 1);   // rotate channel
	SceneObject *obj = NULL;                   // object that is keyframed
};

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
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
	bool objSelected() { return (selected.size() ? true : false); };

	// key framing
	//
	void nextFrame() {
		frame = (frame == frameEnd ? frameBegin: frame + 1);
	}
	void prevFrame() {
		frame = (frame == frameBegin ? frame : frame - 1);
	}
	void startPlayback() {
		bInPlayback = true;
	}

	void stopPlayback() {
		bInPlayback = false;
	}

	// check if both keys set
	//
	bool keyFramesSet() { return (key1.frame != -1 && key2.frame != -1); }

	// linear interpolation between two keyframes
	//
	glm::vec3 linearInterp(int frame, int frameStart, int frameEnd, const glm::vec3 & start, const glm::vec3 & end) {
		return mapVec(frame, frameStart, frameEnd, start, end);
	}

	// example non-linear interpolation between two keyframes (ease-in ease-out)
	//
	glm::vec3 easeInterp(int frame, int frameStart, int frameEnd, const glm::vec3 & start, const glm::vec3 & end) {

		// normalize range (0 to 1) and input to ease formula
		//
		float s = ease(ofMap(frame, frameStart, frameEnd, 0.0, 1.0));

		return mapVec(s, 0.0, 1.0, start, end);
	}


	//  ease-in and ease-out interpolation between two key frames
	//  this function produces a sigmoid curve normalized in x, y in (0 to 1);
	//
	float ease(float x) {
		return (x*x / (x*x + (1 - x)*(1 - x)));
	}

	// helper functions to use ofMap on a vector
	//
	// input a float value in a float range, output a vector 
	//
	glm::vec3 mapVec(float val, float start, float end, const glm::vec3 & outStart, const glm::vec3 & outEnd) {
		return glm::vec3(
			ofMap(val, start, end, outStart.x, outEnd.x),
			ofMap(val, start, end, outStart.y, outEnd.y),
			ofMap(val, start, end, outStart.z, outEnd.z));
	}

	// input a vec3 value in a vec3 range, output a vector 
	//
	glm::vec3 mapVec(const glm::vec3 & val, const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 & outStart, const glm::vec3 & outEnd) {
		return glm::vec3(
			ofMap(val.x, start.x, end.x, outStart.x, outEnd.x),
			ofMap(val.y, start.y, end.y, outStart.y, outEnd.y),
			ofMap(val.z, start.z, end.z, outStart.z, outEnd.z));
	}

	// set keyframe for SceneObject at current frame
	// call this function the first time and key1 is set
	// call this function again and key2 is set.
	// this "cycles" until you call resetKeyFrames();
	//
	void setKeyFrame(SceneObject * obj) {
		if (bKey2Next && key1.obj == obj) {
			key2.frame = frame;
			key2.obj = obj;
			key2.position = obj->position;
			key2.rotation = obj->rotation;
			key2.scale = obj->scale;
			bKey2Next = false;
		}
		else {
			key1.frame = frame;
			key1.obj = obj;
			key1.position = obj->position;
			key1.rotation = obj->rotation;
			key1.scale = obj->scale;
			bKey2Next = true;
		}
	}

	// reset key frames
	//
	void resetKeyFrames() {
		key1.frame = key2.frame = -1;
		key1.obj = key2.obj = NULL;
		bKey2Next = false;
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
	vector<SceneObject *> scene;
	vector<SceneObject *> selected;
	ofPlanePrimitive plane;

	Sphere rootJoint;
	Sphere childJoint;

	// key framing
	//
	KeyFrame key1, key2;    // this demo just has 2 key frames
	int frame = 1;         // current frame
	int frameBegin = 1;     // first frame of playback range;
	int frameEnd = 200;     // last frame of playback range;
	bool bInPlayback = false;  // true => we are in playback mode
	bool bKey2Next = false;


	// state
	//
	bool bDrag = false;
	bool bHide = true;
	bool bAltKeyDown = false;
	bool bRotateX = false;
	bool bRotateY = false;
	bool bRotateZ = false;
	bool bAnimate = false;
	bool bScale = false;
	glm::vec3 lastPoint;
};