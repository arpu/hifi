//
//  SkeletonModel.cpp
//  interface
//
//  Created by Andrzej Kapolka on 10/17/13.
//  Copyright (c) 2013 High Fidelity, Inc. All rights reserved.
//

#include <glm/gtx/transform.hpp>

#include "Avatar.h"
#include "SkeletonModel.h"

SkeletonModel::SkeletonModel(Avatar* owningAvatar) :
    _owningAvatar(owningAvatar)
{
}

void SkeletonModel::simulate(float deltaTime) {
    if (!isActive()) {
        return;
    }
    
    setTranslation(_owningAvatar->getPosition());
    setRotation(_owningAvatar->getOrientation() * glm::angleAxis(180.0f, 0.0f, 1.0f, 0.0f));
    const float MODEL_SCALE = 0.0006f;
    setScale(glm::vec3(1.0f, 1.0f, 1.0f) * _owningAvatar->getScale() * MODEL_SCALE);
    
    Model::simulate(deltaTime);
}

bool SkeletonModel::render(float alpha) {
    if (_jointStates.isEmpty()) {
        return false;
    }
    
    const FBXGeometry& geometry = _geometry->getFBXGeometry();
    
    for (int i = 0; i < _jointStates.size(); i++) {
        const JointState& joint = _jointStates.at(i);
        
        glPushMatrix();
        glMultMatrixf((const GLfloat*)&joint.transform);
        
        if (i == geometry.rootJointIndex) {
            glColor3f(1.0f, 0.0f, 0.0f);
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        glutSolidSphere(0.05f, 10, 10);
        
        glPopMatrix();    
    }
    
    Model::render(alpha);
    
    return true;
}

void SkeletonModel::updateJointState(int index) {
    JointState& state = _jointStates[index];
    const FBXGeometry& geometry = _geometry->getFBXGeometry();
    const FBXJoint& joint = geometry.joints.at(index);
    
    glm::quat combinedRotation = joint.preRotation * state.rotation * joint.postRotation;
    if (joint.parentIndex == -1) {
        glm::mat4 baseTransform = glm::translate(_translation) * glm::mat4_cast(_rotation) *
            glm::scale(_scale) * glm::translate(_offset);
        
        state.transform = baseTransform * geometry.offset * joint.preTransform *
            glm::mat4_cast(combinedRotation) * joint.postTransform;
        state.combinedRotation = _rotation * combinedRotation;
    
    } else {
        const JointState& parentState = _jointStates.at(joint.parentIndex);
        if (index == geometry.leanJointIndex) {
            // get the rotation axes in joint space and use them to adjust the rotation
            state.combinedRotation = _rotation * glm::quat(glm::radians(glm::vec3(_owningAvatar->getHead().getLeanForward(),
                0.0f, _owningAvatar->getHead().getLeanSideways()))) * glm::inverse(_rotation) * parentState.combinedRotation *
                    joint.preRotation * joint.rotation;
            state.rotation = glm::inverse(joint.postRotation * glm::inverse(state.combinedRotation) *
                parentState.combinedRotation * joint.preRotation);
            combinedRotation = joint.preRotation * state.rotation * joint.postRotation;
        }
        state.transform = parentState.transform * joint.preTransform *
            glm::mat4_cast(combinedRotation) * joint.postTransform;
        state.combinedRotation = parentState.combinedRotation * combinedRotation;
    }
    
    if (index == geometry.rootJointIndex) {
        state.transform[3][0] = _translation.x;
        state.transform[3][1] = _translation.y;
        state.transform[3][2] = _translation.z;
    }
}
