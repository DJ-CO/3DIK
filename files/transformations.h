#ifndef TRANSFORMATIONS_H_INCLUDED
#define TRANSFORMATIONS_H_INCLUDED

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/vector_angle.hpp>
#include "bone.h"

#define PI 3.14159265359

void CalcTrans (Armature &armature);
void Translate (glm::vec3 front, glm::vec3 up, glm::vec3 right, Armature &armature, float x, float y);
void Rotate (glm::vec3 front, glm::vec3 up, glm::vec3 right, Armature &armature, float x, float y);
void InverseKinematicsAhead (glm::vec3 front, glm::vec3 up, glm::vec3 right, Joint *target,  Joint *eoChain, glm::vec3 targetLoc);
void InverseKinematicsBehind (glm::vec3 front, glm::vec3 up, glm::vec3 right, Joint *target,  Joint *eoChain, glm::vec3 targetLoc);

void CalcTrans (Armature &armature)
{
    // One for bones
    for (int i = 0; i < armature.bones.size(); i++)
    {
        // Draw the Bone
        glm::vec3 scale = glm::vec3(0,0,0);
        glm::vec3 location = glm::vec3(0,0,0);
        glm::vec3 rotationAngle = glm::vec3(0,0,0);
        glm::vec3 rotationVector = glm::vec3(0,1,0);
        glm::vec3 displacement = glm::vec3(0,0,0);
        glm::vec3 xAxis  = glm::vec3(1,0,0);
        glm::vec3 yAxis  = glm::vec3(0,1,0);
        glm::vec3 zAxis  = glm::vec3(0,0,1);
        scale = glm::vec3 (1.0f,1.0f,1.0f);

        // Calculate the displacement (a 3D vector) from head to tail
        if (armature.pose) displacement = (armature.bones[i]->tail->poseLocation + armature.bones[i]->tail->editLocation) - (armature.bones[i]->head->poseLocation + armature.bones[i]->head->editLocation);
        else displacement = armature.bones[i]->tail->editLocation - armature.bones[i]->head->editLocation;


        scale = glm::vec3 (0.5f,0.5f,0.5f) * glm::length(displacement);

        // Calculate the location of the head
        if (armature.pose) location = armature.bones[i]->head->poseLocation + armature.bones[i]->head->editLocation;
        else location = armature.bones[i]->head->editLocation;

        // Calculate the Euler angles based on the displacement
        rotationAngle.y = glm::orientedAngle(zAxis, glm::normalize(glm::vec3(displacement.x, 0.0f, displacement.z)),yAxis);
        displacement = glm::rotate(displacement, -rotationAngle.y, yAxis);
        rotationAngle.x = glm::orientedAngle(yAxis, glm::normalize(glm::vec3(displacement.x, displacement.y, displacement.z)),xAxis);

        if (armature.pose)
        {
            //armature.bones[i]->poseLocation = location - armature.bones[i]->head->editLocation;
            //armature.bones[i]->poseScale = scale - armature.bones[i]->editScale;
            //armature.bones[i]->poseRotation = rotationAngle - armature.bones[i]->editRotation;
        }
        else
        {
            // Set the values of the bones in edit mode
            armature.bones[i]->editLocation = location;
            armature.bones[i]->editScale = scale;
            armature.bones[i]->editRotation = rotationAngle;
        }
    }
}


void Translate (glm::vec3 front, glm::vec3 up, glm::vec3 right, Armature &armature, float x, float y)
{
    float velocity = 0.01;

    // Translate all selected joints
    for (int i = 0; i < armature.joints.size(); i++)
    {
        if (armature.joints[i]->selected)
        {
            // Do IK **UNFINISHED**
            if (armature.pose)
            {
                glm::vec3 targetLoc = armature.joints[i]->poseLocation + velocity*x*right - velocity*y*up;
                // Do IK for the joint's children
                for (int i = 0; i < armature.joints[i]->children.size(); i++)
                {
                     InverseKinematicsAhead(front, up, right, armature.joints[i], armature.joints[i]->children[i], targetLoc);
                }
                // Do IK for the joint's parents
                InverseKinematicsBehind(front, up, right, armature.joints[i], armature.joints[i]->parent, targetLoc);

                // Move the target
                armature.joints[i]->poseLocation = targetLoc;
            }
            else
            {
                // Move the joints
                armature.joints[i]->editLocation += velocity*x*right;
                armature.joints[i]->editLocation += -velocity*y*up;
            }

        }
    }
    // Translate all selected Bones
    for (int i = 0; i < armature.bones.size(); i++)
    {
        if (armature.bones[i]->selected)
        {
            if (armature.pose)
            {
                //armature.bones[i]->poseLocation += x*right;
                //armature.bones[i]->poseLocation += y*up;
            }
            else
            {
                armature.bones[i]->editLocation += x*right;
                armature.bones[i]->editLocation += y*up;
            }
        }
    }

}

void Rotate (glm::vec3 front, glm::vec3 up, glm::vec3 right, Armature &armature, float x, float y)
{
    // Use camera forward vector
    // rotate the bone's DISPLACEMENT around the front vector
    // Go through the armature and find the selected bone
    for (int i = 0; i < armature.bones.size(); i++)
    {
        if (armature.bones[i]->selected)
        {
            if (armature.pose)
            {
                glm::vec3 displacement = glm::vec3(0,0,0);
                glm::vec3 rotationAngle = glm::vec3(0,1,0);
                glm::vec3 xAxis  = glm::vec3(1,0,0);
                glm::vec3 yAxis  = glm::vec3(0,1,0);
                glm::vec3 zAxis  = glm::vec3(0,0,1);
                displacement = armature.bones[i]->tail->resultLocation - armature.bones[i]->head->resultLocation;
                displacement = glm::rotate(displacement,float(y/(50*2*PI)), front);
                // Assign a pose location to the joint
                armature.bones[i]->tail->poseLocation += displacement -(armature.bones[i]->tail->resultLocation - armature.bones[i]->head->resultLocation);
                // Use the pose location to calculate the pose rotation of the bone
                displacement = (armature.bones[i]->tail->poseLocation + armature.bones[i]->tail->editLocation) - (armature.bones[i]->head->poseLocation + armature.bones[i]->head->editLocation);
                rotationAngle.y = glm::orientedAngle(zAxis, glm::normalize(glm::vec3(displacement.x, 0.0f, displacement.z)),yAxis);
                displacement = glm::rotate(displacement, -rotationAngle.y, yAxis);
                rotationAngle.x = glm::orientedAngle(yAxis, glm::normalize(glm::vec3(displacement.x, displacement.y, displacement.z)),xAxis);
                armature.bones[i]->poseRotation = rotationAngle - armature.bones[i]->editRotation;
            }
            else
            {
                glm::vec3 displacement = glm::vec3(0,0,0);
                displacement = armature.bones[i]->tail->editLocation - armature.bones[i]->head->editLocation;
                displacement = glm::rotate(displacement,float(y/(50*2*PI)), front);
                armature.bones[i]->tail->editLocation = armature.bones[i]->head->editLocation + displacement;
            }
        }
    }
}

// **UNFINISHED**
void InverseKinematicsAhead (glm::vec3 front, glm::vec3 up, glm::vec3 right, Joint *target,  Joint *eoChain, glm::vec3 targetLoc)
{
    // If length(joint newlocation - parent location) == length(joint oldLocation)+ length(parent location)
        // Move joint to new location
    // else length(joint newLocation - parent parent location) < length(joint oldLocation) + length(parent location) + length(parent parent location)
        // Move joint to new location
        // calculate length(joint newLocation - parent parent location)
        // Know all three sides of the triangle

    // Take in front joint, back joint
        // Calculate the sum of the distances


}

void InverseKinematicsBehind (glm::vec3 front, glm::vec3 up, glm::vec3 right, Joint *target,  Joint *eoChain, glm::vec3 targetLoc)
{
    // Calculate the chain distance between the target and the eoChain
    float chainDistance = 0;
    float requiredDistance = 0;
    int chainLength = 0;
    Joint * currentJoint = target;
    // If the joint's parent is null, return
    if (target->parent == NULL)return;
    // If target has a parent, calculate how many and what distance they make
    for (int i = 0; chainDistance <= requiredDistance && currentJoint->parent != NULL;i++)
    {
        // Calculate the min required distance
        requiredDistance = glm::distance(target->resultLocation + targetLoc - target->poseLocation , currentJoint->parent->resultLocation);

        // Add the distance between the two joints to the total
        chainDistance += glm::distance(currentJoint->resultLocation, currentJoint->parent->resultLocation);

        currentJoint = currentJoint->parent;
    }


}
#endif // TRANSFORMATIONS_H_INCLUDED
