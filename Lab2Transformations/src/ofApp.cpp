#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    // Setup camera
    ofSetBackgroundColor(ofColor::black);
    cam.setDistance(10);
    cam.setNearClip(0.1);

    // Pyramid points
    points.push_back(glm::vec3(1, -0.5, 1));
    points.push_back(glm::vec3(-1, -0.5, 1));
    points.push_back(glm::vec3(-1, -0.5, -1));
    points.push_back(glm::vec3(1, -0.5, -1));
    points.push_back(glm::vec3(0, 0.5, 0));

    // Pyramid faces
    faces.push_back(glm::vec3(0, 4, 1));
    faces.push_back(glm::vec3(1, 4, 2));
    faces.push_back(glm::vec3(2, 4, 3));
    faces.push_back(glm::vec3(3, 4, 0));

    // Setup gui
    gui.setup();
    gui.add(scaleGui.setup("Scale"));
    gui.add(rotateGui.setup("Rotate"));
    gui.add(translateGui.setup("Translate"));

    scaleGui.add(scaleSlider.set("Scaling Axes", glm::vec3(1, 1, 1),
                                 glm::vec3(-10, -10, -10), glm::vec3(10, 10, 10)));
    rotateGui.add(rotateSlider.set("Degrees", 0.0f, 0.0f, 360.0f));
    rotateGui.add(rotateAxis.set("Rotate Axes", glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
    translateGui.add(translateSlider.set("Translate Position", glm::vec3(0, 0, 0),
                                         glm::vec3(-20, -20, -20), glm::vec3(20, 20, 20)));
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
    // Computing the transformation
    glm::mat4 trans = glm::translate(glm::mat4(1.0), static_cast<glm::vec3>(translateSlider));
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians<float>(rotateSlider), static_cast<glm::vec3>(rotateAxis));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), static_cast<glm::vec3>(scaleSlider));

    // Since the matrices are evaluated from right to left, scale is first on the right
    glm::mat4 transformMat = trans * (rot * scale);

    cam.begin();
    ofEnableDepthTest();

    // Draw grid
    ofPushMatrix();
    ofSetColor(ofColor::darkGray);
    ofRotateDeg(90);
    ofDrawGridPlane();
    ofPopMatrix();

    // Draw faces
    ofNoFill();
    ofSetColor(ofColor::goldenRod);
    for (glm::vec3 face : faces)
    {
        glm::vec3 p1 = points.at((int) face.x);
        glm::vec3 p2 = points.at((int) face.y);
        glm::vec3 p3 = points.at((int) face.z);

        glm::vec3 p1_t = transformMat * glm::vec4(p1, 1.0f);
        glm::vec3 p2_t = transformMat * glm::vec4(p2, 1.0f);
        glm::vec3 p3_t = transformMat * glm::vec4(p3, 1.0f);

        ofDrawTriangle(p1_t, p2_t, p3_t);
    }

    cam.end();
    ofDisableDepthTest();

    if (!hideGui)
    {
        gui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    switch (key)
    {
        case 'h':
        {
            hideGui = !hideGui;
            break;
        }
        case 'r':
        {
            translateSlider = glm::vec3(0, 0, 0);
            rotateSlider = 0.0f;
            rotateAxis = glm::vec3(0, 0, 1);
            scaleSlider = glm::vec3(1, 1, 1);
            break;
        }
        default:
        {
            break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{ 

}
