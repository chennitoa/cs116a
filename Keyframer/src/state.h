//
//  joint.h
//

#pragma once
#include "Primitives.h"
#include "ofxGui.h"


class ObjectState {
public:
    ObjectState(SceneObject* obj) {
        objPointer = obj;
        position = obj->position;
        rotation = obj->rotation;
        scale = obj->scale;
    }

    SceneObject* objPointer;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};


class SceneState {
public:
    SceneState() {
        gui.setup("", ofxPanelDefaultFilename, 10.0f, 450.0f);
        gui.add(interpolationFunction.set("Interpolation Function", interpolationFunction, 0, numFunctions - 1));
    }

    ~SceneState() {}

    float interpolate(int currentFrame, int totalFrames);
    void clear();
    void setScene(vector<SceneObject*> scene);
    bool isActive();

    inline glm::vec3 getObjPosition(SceneObject* obj) {
        return objectStates.at(obj).position;
    }

    inline glm::vec3 getObjRotation(SceneObject* obj) {
        return objectStates.at(obj).rotation;
    }

    inline glm::vec3 getObjScale(SceneObject* obj) {
        return objectStates.at(obj).scale;
    }

    map<SceneObject*, ObjectState> objectStates;
    ofParameter<int> interpolationFunction;
    int numFunctions = 3;

    ofxPanel gui;
    
};