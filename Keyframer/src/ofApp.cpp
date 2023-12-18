
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


#include "ofApp.h"

#include <algorithm>
#include <format>
#include <set>
#include <stack>


// Set draw axes
bool ofApp::bDrawAxes = false;


//--------------------------------------------------------------
//
void ofApp::setup()
{
	ofSetBackgroundColor(ofColor::black);
	ofEnableDepthTest();
	mainCam.setDistance(30);
	mainCam.setNearClip(.1);
	
	sideCam.setPosition(40, 0, 0);
	sideCam.lookAt(glm::vec3(0, 0, 0));
	topCam.setNearClip(.1);
	topCam.setPosition(0, 16, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));
	ofSetSmoothLighting(true);


	// setup one point light
	//
	light1.enable();
	light1.setPosition(5, 5, 0);
	light1.setDiffuseColor(ofColor(255.f, 255.f, 255.f));
	light1.setSpecularColor(ofColor(255.f, 255.f, 255.f));

	theCam = &mainCam;

	numJoints = 0;
	loadFromFile("skeleton.txt");

	groundPlane = new Plane(glm::vec3(0, -2, 0), glm::vec3(0, 1, 0));
	scene.push_back(groundPlane);

	// setup keyframer
	totalFrames = 120;
	keyframer = new Keyframer(totalFrames);
	isAnimating = false;

	keyframingGui.setup("", ofxPanelDefaultFilename, 10.0f, 300.0f);
	keyframingGui.add(frameGoto.setup("Go to frame", 0));

	frameGoto.setMin(0);
	frameGoto.setMax(totalFrames);
	frameGoto.addListener(this, &frameGotoListener);

	frameGui.setup("", ofxPanelDefaultFilename, 10.0f, 350.0f);
	frameGui.add(deleteScene.setup("Delete Keyframe"));
	frameGui.add(createScene.setup("Create Keyframe"));

	deleteScene.addListener(this, &deleteSceneListener);
	createScene.addListener(this, &createSceneListener);
}

	

//--------------------------------------------------------------
void ofApp::exit()
{

}

//--------------------------------------------------------------
void ofApp::update()
{
	if (isAnimating) {
		keyframer->nextFrame(scene);
	}

}

//--------------------------------------------------------------
void ofApp::draw()
{
	theCam->begin();
	ofEnableDepthTest();
	ofNoFill();
	drawAxis();
	ofEnableLighting();

	//  draw the objects in scene
	//
	// material.begin();
	ofFill();
	for (SceneObject* obj : scene)
	{
		if (objSelected() && obj == selected[0])
		{
			ofSetColor(ofColor::white);
		}
		else
		{
			ofSetColor(obj->diffuseColor);
		}
		obj->draw();
	}

	material.end();
	ofDisableLighting();
	ofDisableDepthTest();
	theCam->end();

	if (!bHide)
	{
		// Draw selection properties
		if (objSelected())
		{
			selected[0]->gui.draw();
		}

		// Draw the state to get keyframing function
		keyframer->getCurrentFrame()->gui.draw();

		frameGui.draw();
		keyframingGui.draw();
	}

	string frameString = "Frame: " + to_string(keyframer->currentFrame) + " of " + to_string(totalFrames);

	ofSetColor(ofColor::white);
	ofDrawBitmapString(frameString, 5, 750);
}

// 
// Draw an XYZ axis in RGB at transform
//
void ofApp::drawAxis(glm::mat4 m, float len)
{
	if (!ofApp::bDrawAxes)
	{
		// Exit, do not draw axes
		return;
	}

	// Get current ofColor
	ofColor currColor = ofGetStyle().color;

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(glm::vec3(m*glm::vec4(0, 0, 0, 1)), glm::vec3(m*glm::vec4(len, 0, 0, 1)));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(glm::vec3(m*glm::vec4(0, 0, 0, 1)), glm::vec3(m*glm::vec4(0, len, 0, 1)));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(glm::vec3(m*glm::vec4(0, 0, 0, 1)), glm::vec3(m*glm::vec4(0, 0, len, 1)));

	// Set back to current color to not interfere with drawing
	ofSetColor(currColor);
}

// print C++ code for obj tranformation channels. (for debugging);
//
void ofApp::printChannels(SceneObject *obj)
{
	glm::vec3 posVec = static_cast<glm::vec3>(obj->position);
	glm::vec3 rotVec = static_cast<glm::vec3>(obj->rotation);
	glm::vec3 scaleVec = static_cast<glm::vec3>(obj->scale);
	cout << "position = glm::vec3(" << posVec.x << "," << posVec.y << "," << posVec.z << ");" << endl;
	cout << "rotation = glm::vec3(" << rotVec.x << "," << rotVec.y << "," << rotVec.z << ");" << endl;
	cout << "scale = glm::vec3(" << scaleVec.x << "," << scaleVec.y << "," << scaleVec.z << ");" << endl;
}

// Create new joint as a child of the selected joint
//
void ofApp::createJoint()
{
	// Create Joint
	numJoints++;
	string jointName = "joint" + to_string(numJoints);

	while (jointNames.find(jointName) != jointNames.end())
	{
		numJoints++;
		jointName = "joint" + to_string(numJoints);
	}

	Joint* newJoint = new Joint(jointName, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	jointNames.insert(pair<string, Joint*>(jointName, newJoint));
	scene.push_back(newJoint);
	jointsList.push_back(newJoint);

	// Find parent for joint
	if (objSelected())
	{
		Joint* joint = getJointPointer(selected[0]);
		if (joint)
		{
			joint->addChildJoint(newJoint);
		}
	}
	
}

// Delete joint and reconnect children
void ofApp::deleteJoint(Joint* joint)
{
	// Get parent and children
	vector<SceneObject*> children = joint->endSphere->childList;
	SceneObject* parent = joint->parent;

	// Check if parent is a joint
	bool foundJoint = false;
	for (Joint* pJoint : jointsList)
	{
		if (parent == pJoint)
		{
			for (SceneObject* child : children)
			{
				pJoint->addChildJoint(child);
				foundJoint = true;
			}
			// Remove the child from the parent joint's child list
			vector<SceneObject*> parentChildList = pJoint->endSphere->childList;
			for (size_t index = 0; index < parentChildList.size(); index++)
			{
				if (parentChildList.at(index) == joint)
				{
					parentChildList.erase(parentChildList.begin() + index);

					// Account for smaller list
					index--;
				}
			}
		}
	}

	// Parent is not a joint, could be nullptr
	if (parent == nullptr)
	{
		vector<SceneObject*> children = joint->endSphere->childList;
		for (SceneObject* child : children)
		{
			child->parent = nullptr;
		}
	}
	else if (!foundJoint)
	{
		for (SceneObject* child : children)
		{
			parent->addChild(child);
		}
		// Remove the child from the parent node
		for (size_t index = 0; index < parent->childList.size(); index++)
		{
			if (parent->childList.at(index) == joint)
			{
				parent->childList.erase(parent->childList.begin() + index);

				// Account for smaller list
				index--;
			}
		}
	}

	// Remove all references
	for (size_t index = 0; index < jointsList.size(); index++)
	{
		if (jointsList.at(index) == joint)
		{
			jointsList.erase(jointsList.begin() + index);

			// Account for smaller list
			index--;
		}
	}
	for (size_t index = 0; index < scene.size(); index++)
	{
		if (scene.at(index) == joint)
		{
			scene.erase(scene.begin() + index);

			// Account for smaller list
			index--;
		}
	}
	delete joint;

	// Don't decrement numJoints, otherwise may run into similarly named joints
}

// Save to file
//
void ofApp::saveToFile(string filename)
{
	// Topsort on the joints
	//

	vector<Joint*> saveOrder;
	set<Joint*> visited;

	for (size_t index = 0; index < jointsList.size(); index++)
	{
		topsort(saveOrder, visited, jointsList.at(index));
	}

	// End topsort by reversing the list
	reverse(saveOrder.begin(), saveOrder.end());

	// Save to file
	//

	ofFile jointsFile;
	jointsFile.open(ofToDataPath(filename), ofFile::WriteOnly, false);
	
	ofBuffer jointsBuffer;

	if (jointsFile.is_open())
	{
		for (Joint* joint : saveOrder)
		{
			jointsBuffer.append(joint->toStringFormat() + "\n");
		}
		jointsFile.writeFromBuffer(jointsBuffer);
	}
	jointsFile.close();
}

// Load from file
//
void ofApp::loadFromFile(string filename)
{
	// Checks if file exists
	// Clears all current joints on screen

	ofFile jointsFile;
    jointsFile.open(ofToDataPath(filename), ofFile::ReadOnly, false);

	if (!jointsFile.exists())
	{
		// Return if file not found
		jointsFile.close();
		cout << "File " << filename << " not found for loading." << endl;
		return;
	}

	while (!jointsList.empty())
	{
		deleteJoint(jointsList.front());
		selected.clear();
	}
	
    if (jointsFile.is_open())
    {
        std::string jointString;		
        while (getline(jointsFile, jointString))
		{			
			char name[16];
			float r1 = 0, r2 = 0, r3 = 0;
			float p1 = 0, p2 = 0, p3 = 0;
			char parent[16];

			const char* parentFormat = "create -joint %16s -rotate <%f, %f, %f> -translate <%f, %f, %f> -parent %16s";

			int matches = sscanf(jointString.c_str(), parentFormat, &name,
								 &r1, &r2, &r3,
								 &p1, &p2, &p3,
								 &parent);
			
			
			// 8 arguments in parentFormat, less if not all matched
			if (matches == 8)
			{
				Joint* newJoint = new Joint(string(name), glm::vec3(p1, p2, p3), glm::vec3(r1, r2, r3));
				jointNames.at(string(parent))->addChildJoint(newJoint);
				jointNames.insert(pair<string, Joint*>(string(name), newJoint));

				scene.push_back(newJoint);
				jointsList.push_back(newJoint);
				numJoints++;
			}
			else
			{
				const char* noParentFormat = "create -joint %16s -rotate <%f, %f, %f> -translate <%f, %f, %f>";
				matches = sscanf(jointString.c_str(), noParentFormat, &name,
							     &r1, &r2, &r3,
							     &p1, &p2, &p3);
				
				if (matches == 7)
				{
					Joint* newJoint = new Joint(string(name), glm::vec3(p1, p2, p3), glm::vec3(r1, r2, r3));
					jointNames.insert(pair<string, Joint*>(string(name), newJoint));

					scene.push_back(newJoint);
					jointsList.push_back(newJoint);
					numJoints++;
				}
				else
				{
					// Do nothing, invalid line
					cout << "Invalid line \"" << jointString << "\"" << endl;
				}
			}
		}
    }
    jointsFile.close();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_ALT:
		bAltKeyDown = false;
		mainCam.disableMouseInput();
		break;
	case 'a':
		bDrawAxes = !bDrawAxes;
		break;
	case 'x':
		bRotateX = false;
		break;
	case 'y':
		bRotateY = false;
		break;
	case 'z':
		bRotateZ = false;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 'C':
	case 'c':
		if (mainCam.getMouseInputEnabled()) mainCam.disableMouseInput();
		else mainCam.enableMouseInput();
		break;
	case 'D':
	case 'd':
		if (objSelected())
		{
			Joint* joint = getJointPointer(selected[0]);
			if (joint)
			{
				deleteJoint(joint);
			}
		}
		// Deselect the joint
		selected.clear();
		keyframer->clear();
		break;
	case 'F':
	case 'b':
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	case 'h':
		bHide = !bHide;
		break;
	case 'i':
		break;
	case 'j':
		createJoint();
		keyframer->clear();
		break;
	case 'k':
		keyframer->saveFrame(scene);
		break;
	case 'l':
		keyframer->deleteFrame();
		break;
	case 'n':
		break;
	case 'p':
		if (objSelected()) printChannels(selected[0]);
		break;
	case 'r':
		break;
	case 's':
		saveToFile("skeleton.txt");
		break;
	case 'x':
		bRotateX = true;
		break;
	case 'y':
		bRotateY = true;
		break;
	case 'z':
		bRotateZ = true;
		break;
	case OF_KEY_F1: 
		theCam = &mainCam;
		break;
	case OF_KEY_F2:
		theCam = &sideCam;
		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_ALT:
		bAltKeyDown = true;
		if (!mainCam.getMouseInputEnabled()) mainCam.enableMouseInput();
		break;
	case ' ':
		isAnimating = !isAnimating;
		if (isAnimating)
		{
			cout << "Animation started" << endl;
		}
		else
		{
			cout << "Animation stopped" << endl;
		}
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (objSelected() && bDrag) {
		glm::vec3 point; 
		mouseToDragPlane(x, y, point);
		if (bRotateX) {
			selected[0]->rotation += glm::vec3((point.x - lastPoint.x) * 20.0, 0, 0);
		}
		else if (bRotateY) {
			selected[0]->rotation += glm::vec3(0, (point.x - lastPoint.x) * 20.0, 0);
		}
		else if (bRotateZ) {
			selected[0]->rotation += glm::vec3(0, 0, (point.x - lastPoint.x) * 20.0);
		}
		else {
			selected[0]->position += (point - lastPoint);
		}
		lastPoint = point;
	}

}

//  This projects the mouse point in screen space (x, y) to a 3D point on a plane
//  normal to the view axis of the camera passing through the point of the selected object.
//  If no object selected, the plane passing through the world origin is used.
//
bool ofApp::mouseToDragPlane(int x, int y, glm::vec3 &point) {
	glm::vec3 p = theCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - theCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	float dist;
	glm::vec3 pos;
	if (objSelected()) {
		pos = selected[0]->position;
	}
	else pos = glm::vec3(0, 0, 0);
	if (glm::intersectRayPlane(p, dn, pos, glm::normalize(theCam->getZAxis()), dist)) {
		point = p + dn * dist;
		return true;
	}
	return false;
}

//--------------------------------------------------------------
//
// Provides functionality of single selection and if something is already selected,
// sets up state for translation/rotation of object using mouse.
//
void ofApp::mousePressed(int x, int y, int button){

	// if we are moving the camera around, don't allow selection
	//
	if (mainCam.getMouseInputEnabled()) return;

	// clear selection list
	//
	selected.clear();

	//
	// test if something selected
	//
	vector<SceneObject *> hits;

	glm::vec3 p = theCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - theCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	// check for selection of scene objects
	//
	for (SceneObject* obj : scene) {
		
		glm::vec3 point, norm;
		
		//  We hit an object
		//
		if (obj->isSelectable && obj->intersect(Ray(p, dn), point, norm)) {
			hits.push_back(obj);
		}
	}


	// if we selected more than one, pick nearest
	//
	SceneObject *selectedObj = NULL;
	if (!hits.empty()) {
		selectedObj = hits[0];
		float nearestDist = std::numeric_limits<float>::infinity();
		for (SceneObject* hit : hits) {
			float dist = glm::length(hit->position - theCam->getPosition());
			if (dist < nearestDist) {
				nearestDist = dist;
				selectedObj = hit;
			}	
		}
	}
	if (selectedObj) {
		selected.push_back(selectedObj);
		bDrag = true;
		mouseToDragPlane(x, y, lastPoint);
	}
	else {
		selected.clear();
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bDrag = false;

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

