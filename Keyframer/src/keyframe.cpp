//
//  keyframe.cpp
//

#include <cmath>
#include "keyframe.h"


SceneState* Keyframer::getCurrentFrame() {
    return keyframes.at(currentFrame);
}


void Keyframer::goToFrame(int frame, vector<SceneObject*> scene) {

    if (frame >= totalFrames) {
        // Invalid frame
        return;
    }

    currentFrame = frame;

    // Check if keyframe is saved, if so, load keyframe
    SceneState* currState = getCurrentFrame();
    if (currState->isActive()) {
        for (SceneObject* obj: scene) {
            obj->position = currState->objectStates.at(obj).position;
            obj->rotation = currState->objectStates.at(obj).rotation;
            obj->scale = currState->objectStates.at(obj).scale;
        }
        currKeyframe = currentFrame;
    }
    else {
        currKeyframe = getPrevKeyframe(currentFrame);
        nextKeyframe = getNextKeyframe(currentFrame);
    }
}


void Keyframer::nextFrame(vector<SceneObject*> scene) {

    currentFrame++;

    // If reached end of total frames
    if (currentFrame == totalFrames) {
        currentFrame = 0;
    }

    // If reached end of current keyframe
    if (currentFrame == nextKeyframe) {
        currKeyframe = currentFrame;
        nextKeyframe = getNextKeyframe(currentFrame);
    }

    // If only one or zero keyframes
    if (currKeyframe == nextKeyframe) {
        // Do nothing
        return;
    }


    SceneState* currKeyframeState = keyframes.at(currKeyframe);
    SceneState* nextKeyframeState = keyframes.at(nextKeyframe);

    float framesFromCurrent = 0;
    if (currentFrame >= currKeyframe) {
        framesFromCurrent = currentFrame - currKeyframe;
    }
    else {
        framesFromCurrent = totalFrames - currKeyframe + currentFrame;
    }

    float framesFromNext = 0;
    if (nextKeyframe >= currKeyframe) {
        framesFromNext = nextKeyframe - currKeyframe;
    }
    else {
        framesFromNext = totalFrames - currKeyframe + nextKeyframe;
    }

    float interpolationFactor = currKeyframeState->interpolate(framesFromCurrent, framesFromNext);

    // Update scene objects
    currKeyframeState = keyframes.at(currKeyframe);
    for (SceneObject* obj : scene) {
        obj->position = interpolateVector(currKeyframeState->getObjPosition(obj),
                                          nextKeyframeState->getObjPosition(obj),
                                          interpolationFactor);
        
        obj->rotation = interpolateVector(currKeyframeState->getObjRotation(obj),
                                          nextKeyframeState->getObjRotation(obj),
                                          interpolationFactor);

        obj->scale = interpolateVector(currKeyframeState->getObjScale(obj),
                                       nextKeyframeState->getObjScale(obj),
                                       interpolationFactor);
    }

}


void Keyframer::saveFrame(vector<SceneObject*> scene) {
    keyframes.at(currentFrame)->setScene(scene);

    // Update current and next keyframe
    currKeyframe = currentFrame;
    nextKeyframe = getNextKeyframe(currKeyframe);
}


void Keyframer::deleteFrame() {
    keyframes.at(currentFrame)->clear();

    // Update current and next keyframe
    currKeyframe = getPrevKeyframe(currKeyframe);
    nextKeyframe = getNextKeyframe(currKeyframe);
}


void Keyframer::clear() {
    for (pair<int, SceneState*> keyframe : keyframes)
    {
        keyframe.second->clear();
    }
}


int Keyframer::getNextKeyframe(int startingFrame) {
    // Initialize search index
    int frameCounter = startingFrame + 1;
    if (frameCounter == totalFrames) {
        frameCounter = 0;
    }

    // Search keyframes
    while (frameCounter != startingFrame) {
        if (keyframes.at(frameCounter)->isActive()) {
            return frameCounter;
        }

        // Wrap around the frame counter to 0
        frameCounter++;
        if (frameCounter == totalFrames) {
            frameCounter = 0;
        }
    }

    // Only one keyframe
    return startingFrame;
}


int Keyframer::getPrevKeyframe(int startingFrame) {
    // Initialize search index
    int frameCounter = startingFrame - 1;
    if (frameCounter == -1) {
        frameCounter = totalFrames - 1;
    }

    // Search keyframes
    while (frameCounter != startingFrame) {
        if (keyframes.at(frameCounter)->isActive()) {
            return frameCounter;
        }

        // Wrap around the frame counter to the last frame
        frameCounter--;
        if (frameCounter == -1) {
            frameCounter = totalFrames - 1;
        }
    }

    // Only one keyframe
    return startingFrame;
}


glm::vec3 Keyframer::interpolateVector(const glm::vec3 &begin, const glm::vec3 &end, float factor) {
    return glm::vec3(
        ofMap(factor, 0.0f, 1.0f, begin.x, end.x),
        ofMap(factor, 0.0f, 1.0f, begin.y, end.y),
        ofMap(factor, 0.0f, 1.0f, begin.z, end.z)
    );
}