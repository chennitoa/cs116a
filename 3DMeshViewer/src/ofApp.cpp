#include "ofApp.h"

//--------------------------------------------------------------
void Mesh::loadMesh(std::string filename)
{
    ofFile objFile;
    objFile.open(ofToDataPath(filename), ofFile::ReadOnly, false);
    if (objFile.is_open())
    {
        std::string objString;
        while (getline(objFile, objString))
        {
            if (objString.size() == 0)
            {
                // Read in blank line
                continue;
            }
            else
            {
                pushStringToMesh(objString);
            }
        }
    }
    objFile.close();
}

//--------------------------------------------------------------
void Mesh::pushStringToMesh(std::string objString)
{
    std::stringstream ss(objString);
    string parseString;
    ss >> parseString;
    if (parseString.compare("v") == 0)
    {
        std::string p1Str, p2Str, p3Str;
        ss >> p1Str >> p2Str >> p3Str;
        // Create Vertex
        Vertex* v = new Vertex(stof(p1Str), stof(p2Str), stof(p3Str));
        vertices.push_back(v);
        return;
    }
    else if (parseString.compare("f") == 0)
    {
        std::string v1Str, v2Str, v3Str;
        ss >> v1Str >> v2Str >> v3Str;
        // Create Face
        Face* f = new Face(stoul(v1Str), stoul(v2Str), stoul(v3Str));
        faces.push_back(f);
        return;
    }
    else
    {
        // Do nothing
        return;
    }
}

//--------------------------------------------------------------
void Mesh::draw()
{
    // Draw non-highlighted faces first, then draw highlighted faces for overlap issues
    ofPushMatrix();
    ofEnableDepthTest();
    // Non-highlighted faces
    ofSetColor(NORMALCOLOR);
    for (Face* face : faces)
    {
        // Account for 1-indexed vertices
        if (!face->isHighlighted)
        {
            ofDrawTriangle(*vertices.at(face->v1 - 1), *vertices.at(face->v2 - 1), *vertices.at(face->v3 - 1));
        }
    }
    // Highlighted faces, disable depth test to always draw over
    ofDisableDepthTest();
    ofSetColor(HIGHLIGHTEDCOLOR);
    for (Face* face : faces)
    {
        // Account for 1-indexed vertices
        if (face->isHighlighted)
        {
            ofDrawTriangle(*vertices.at(face->v1 - 1), *vertices.at(face->v2 - 1), *vertices.at(face->v3 - 1));
        }
    }
    ofPopMatrix();
    ofEnableDepthTest();
}

//--------------------------------------------------------------
void Mesh::highlightFacesFromVertex(unsigned int vertexIndex)
{
    // Assumes 1-indexed vertices
    if (vertexIndex > vertices.size())
    {
        cout << "Invalid vertex index " << vertexIndex << " for selecting adjacent faces." << endl;
        return;
    }
    for (Face* face : faces)
    {
        if (face->v1 == vertexIndex || face->v2 == vertexIndex || face->v3 == vertexIndex)
        {
            face->isHighlighted = true;
        }
        else
        {
            face->isHighlighted = false;
        }
    }
}

//--------------------------------------------------------------
void Mesh::highlightFacesFromFace(unsigned int faceIndex)
{
    Face* selectedFace = faces.at(faceIndex);
    for (Face* face : faces)
    {
        int numberFacesMatched = 0;

        // Hardcoded vertex comparisons, match 2
        if (selectedFace->v1 == face->v1 || selectedFace->v1 == face->v2 || selectedFace->v1 == face->v3)
        {
            numberFacesMatched++;
        }
        if (selectedFace->v2 == face->v1 || selectedFace->v2 == face->v2 || selectedFace->v2 == face->v3)
        {
            numberFacesMatched++;
        }
        if (selectedFace->v3 == face->v1 || selectedFace->v3 == face->v2 || selectedFace->v3 == face->v3)
        {
            numberFacesMatched++;
        }

        // Highlighed all matching faces
        if (numberFacesMatched >= 2)
        {
            face->isHighlighted = true;
        }
        else
        {
            face->isHighlighted = false;
        }
    }
}

//--------------------------------------------------------------
void ofApp::vertexHighlightPressed()
{
    mesh3D.highlightFacesFromVertex(vertexSelect);
}

//--------------------------------------------------------------
void ofApp::faceHighlightPressed()
{
    mesh3D.highlightFacesFromFace(faceSelect);
}


//--------------------------------------------------------------
void ofApp::setup()
{
    // Camera setup
    ofSetBackgroundColor(ofColor::black);
    cam.setDistance(10);
    cam.setNearClip(0.1);
    ofEnableDepthTest();

    // Load mesh
    loadMeshFromFile("monster.obj");
    // mesh3D.vertices.push_back(new Vertex(1.0, 2.0, 3.0));
    // mesh3D.vertices.push_back(new Vertex(5.0, 6.0, 8.0));
    // mesh3D.vertices.push_back(new Vertex(11.0, -1.0, -1.0));
    // mesh3D.vertices.push_back(new Vertex(-11.0, -10.0, -10.0));

    // mesh3D.faces.push_back(new Face(1, 2, 3));
    // mesh3D.faces.push_back(new Face(4, 3, 1));
    // mesh3D.faces.push_back(new Face(2, 3, 4));

    // Setup gui
    gui.setup();
    gui.add(vertexSelect.setup("vertex index", 1, 1, mesh3D.vertices.size()));
    gui.add(vertexHighlight.setup("Highlight Adjacent Faces To Vertex"));
    gui.add(faceSelect.setup("face index", 1, 1, mesh3D.faces.size()));
    gui.add(faceHighlight.setup("Highlight Adjacent Faces To Face"));

    vertexHighlight.addListener(this, &ofApp::vertexHighlightPressed);
    faceHighlight.addListener(this, &ofApp::faceHighlightPressed);

    hideGui = false;

    // Print dianostic information
    int kbSize = (mesh3D.vertices.size() * sizeof(float) * 3 + mesh3D.faces.size() * sizeof(unsigned int) * 3) / 1000;
    cout << "Number of vertices: " << mesh3D.vertices.size() << endl;
    cout << "Number of faces: " << mesh3D.faces.size() << endl;
    cout << "Size of model: " << kbSize << "KB" << endl;
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
    cam.begin();

    // Grid
    ofPushMatrix();
    ofSetColor(ofColor::dimGray);
    ofRotateDeg(90);
    ofDrawGridPlane();
    ofPopMatrix();

    // Draw Faces
    ofNoFill();
    ofSetColor(NORMALCOLOR);
    mesh3D.draw();

    cam.end();

    // Draw Gui
    if (!hideGui)
    {
        ofDisableDepthTest();
        gui.draw();
        ofEnableDepthTest();
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
void ofApp::mouseMoved(int x, int y )
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

//--------------------------------------------------------------
void ofApp::loadMeshFromFile(std::string filename)
{
    mesh3D.loadMesh(filename);
}