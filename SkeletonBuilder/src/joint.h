//
//  joint.h
//

#pragma once
#include "Primitives.h"
#include "ofMain.h"


class Joint : public SceneObject {
public:
    Joint(string id, glm::vec3 tran, glm::vec3 rot, float length = 2.0f, ofColor color = ofColor::orange) : SceneObject() {
        // Joint in neutral points straight down
        // Position should reflect the end of the joint
        position = tran;
		rotation = rot;
        name = id;
        jointLength = length;
        diffuseColor = color;

        baseSphere = new Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 0.2f, color);
        endSphere = new Sphere(glm::vec3(0.0f, -length, 0.0f), 0.2f, color);

        // Cone is in between base and endSphere
        // 1:1:6:1:1 sphere to spacing to cone to spacing to sphere
        bone = new Cone(glm::vec3(0.0f, -0.5f * length, 0.0f), glm::vec3(0, 0, 0),
                        glm::vec3(0.2f, 0.4f * length, 0.2f), color);
        
        // Set children for convenient transformations
        

        // Make sure baseSphere parent is eventually passed along
        this->addChild(baseSphere);
        this->addChild(bone);
        this->addChild(endSphere);
    }
    ~Joint() {
        delete baseSphere;
        delete bone;
        delete endSphere;
    }
    void draw();
    bool intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal);
    void addChildJoint(SceneObject* child);
    string toStringFormat();

    float jointLength;

    // At the base
    Sphere* baseSphere;

    // The bone
    Cone* bone;

    // If the joint has no children
    Sphere* endSphere;

};