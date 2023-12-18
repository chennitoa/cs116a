//
//  joint.cpp
//


#include "joint.h"


void Joint::draw() {
    
    baseSphere->draw();
    bone->draw();

    // If the joint has no children, one "child" is the base sphere
    if (endSphere->childList.empty()) {
        endSphere->draw();
    }
}


bool Joint::intersect(const Ray &ray, glm::vec3 &point, glm::vec3 &normal) {
    // Calculate the intersection from baseSphere end going forward
    if (baseSphere->intersect(ray, point, normal)) {
        return true;
    }
    else if (bone->intersect(ray, point, normal)) {
        return true;
    }
    else if (childList.size() > 1 && endSphere->intersect(ray, point, normal)) {
        return true;
    }
    else {
        return false;
    }
}


void Joint::addChildJoint(SceneObject* child) {
    endSphere->addChild(child);
}


string Joint::toStringFormat() {
    // joint -> parent (endSphere) -> joint
    // Assumes parents are joints

    glm::vec3 posVec = static_cast<glm::vec3>(position);
    glm::vec3 rotVec = static_cast<glm::vec3>(rotation);

    if (parent && parent->parent) {
        size_t buf_size = 512;
        const char* format = "create -joint %s -rotate <%f, %f, %f> -translate <%f, %f, %f> -parent %s";
        auto buf = make_unique<char[]>(buf_size);
        int str_len = snprintf(buf.get(), buf_size, format, name.c_str(),
                               rotVec.x, rotVec.y, rotVec.z,
                               posVec.x, posVec.y, posVec.z,
                               parent->parent->parent->name.c_str());
        return string(buf.get(), buf.get() + str_len);
    }
    else {
        size_t buf_size = 512;
        const char* format = "create -joint %s -rotate <%f, %f, %f> -translate <%f, %f, %f>";
        auto buf = make_unique<char[]>(buf_size);
        int str_len = snprintf(buf.get(), buf_size, format, name.c_str(),
                              rotVec.x, rotVec.y, rotVec.z,
                              posVec.x, posVec.y, posVec.z);
        return string(buf.get(), buf.get() + str_len);
    }
}