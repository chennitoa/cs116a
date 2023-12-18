//
//  state.cpp
//

#include "state.h"


// Private function declarations
// If another interpolation function is added to the enum, one must be added here
float linearInterp(int currentFrame, int totalFrames);
float easeInterp(int currentFrame, int totalFrames);
float cubicInterp(int currentFrame, int totalFrames);

typedef float (*InterpFunc)(int, int);

InterpFunc interpolationFunctions[3] = {
    linearInterp,
    easeInterp,
    cubicInterp,
};


float SceneState::interpolate(int currentFrame, int totalFrames) {
    return interpolationFunctions[interpolationFunction](currentFrame, totalFrames);
}


void SceneState::clear() {
    objectStates.clear();
}


void SceneState::setScene(vector<SceneObject*> scene) {
    objectStates.clear();

    // Duplicate scene by pushing back all the objects in the scene
    for (SceneObject* obj : scene) {
        objectStates.insert(pair<SceneObject*, ObjectState>(obj, ObjectState(obj)));
    }
}


bool SceneState::isActive() {
    return !objectStates.empty();
}


float linearInterp(int currentFrame, int totalFrames) {
    return static_cast<float>(currentFrame) / static_cast<float>(totalFrames);
}


float easeInterp(int currentFrame, int totalFrames) {
    float normalizedFrame = static_cast<float>(currentFrame) / static_cast<float>(totalFrames);
    return pow(normalizedFrame, 2) / (pow(normalizedFrame, 2) + pow(1 - normalizedFrame, 2));
}


float cubicInterp(int currentFrame, int totalFrames) {
    float normalizedFrame = static_cast<float>(currentFrame) / static_cast<float>(totalFrames);
    return (pow((2.0f * normalizedFrame) - 1.0f, 3) / 2.0f) + 0.5f;
}