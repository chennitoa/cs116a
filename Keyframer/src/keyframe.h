//
//  keyframe.h
//

#pragma once
#include <map>
#include "state.h"


class Keyframer {
public:
    Keyframer(int numFrames) {
        // Insert empty keyframes into the map
        for (int frame = 0; frame < numFrames; frame++) {
            keyframes.insert(pair<int, SceneState*>(frame, new SceneState()));
        }

        currentFrame = 0;
        totalFrames = numFrames;
    }

    ~Keyframer() {
        for (pair<int, SceneState*> keyframe : keyframes)
        {
            delete keyframe.second;
        }
    }

    SceneState* getCurrentFrame();
    void goToFrame(int frame, vector<SceneObject*> scene);
    void nextFrame(vector<SceneObject*> scene);
    void saveFrame(vector<SceneObject*> scene);
    void deleteFrame();
    void clear();

    map<int, SceneState*> keyframes;
    int currentFrame;
    int totalFrames;

private:

    // Used for interpolation calculations
    int currKeyframe;
    int nextKeyframe;

    // Utility
    int getNextKeyframe(int startingFrame);
    int getPrevKeyframe(int startingFrame);
    glm::vec3 interpolateVector(const glm::vec3 &begin, const glm::vec3 &end, float factor);
};
