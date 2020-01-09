#ifndef BONE_H_INCLUDED
#define BONE_H_INCLUDED

class Joint;
class Bone;
class Armature;

class Bone
{
public:
    // The tip of the bone
    Joint * tail;
    // The root of the bone
    Joint * head;
    // object meant to represent the bone
    Object boneObject;
    //  All derived from “head” edit properties
    glm::vec3 editLocation {1,0,0};
    glm::vec3 editRotation {0,0,0};
    glm::vec3 editScale {1,1,1};
    // All derived from “head” pose properties
    glm::vec3  poseLocation {0,0,0};
    glm::vec3  poseRotation {0,0,0};
    glm::vec3  poseScale {0,0,0};
    // The resultant rotation after all parent rotations are considered
    glm::vec3  resultRotation {0,0,0};
    // Whether the bone is selected or not
    bool selected = false;
};

class Joint
{
public:
    // object meant to represent the joint
    Object jointObject;
    glm::vec3 editLocation {0,0,0};
    glm::vec3 poseLocation {0,0,0};
    glm::vec3 resultLocation {0,0,0};
    Joint * parent;
    vector <Joint*>  children;
    // Is connected to parent by bone, and therefore has bone
    bool isConnected = false;
    // Whether the joint is selected or not
    bool selected = false;
    // the bone below the joint (this joint is the tail of this bone)
    Bone * bone;
};

class Armature
{
public:
    vector <Joint*> roots;
    vector <Joint*> joints;
    vector <Bone*> bones;
    Joint * current;
    bool pose = false;
    Object boneObject;
    Object jointObject;

    // Create joint
    void CreateJoint (Joint *parent)
    {
        Joint * joint;
        joint = new Joint;

        // If a parent is not supplied
        if (parent == NULL)
        {
            //  cout << "root created" << endl;
            joint->parent = NULL;
            joint->bone = NULL;
            // Note that this joint must be a root
            roots.push_back(joint);// DOESN'T GET PAST
            // Since this joint is a root, it must have at least 1 child
            CreateJoint (joint);

        }
        else
        {
            // Assign the joint's parent
            joint->parent = parent;
            // Offset the joint from its parent
            joint->editLocation = joint->parent->editLocation + glm::vec3{1,1,1};
            // Add this joint to its parent's list of children
            joint->isConnected = true;
            joint->parent->children.push_back(joint);
            // Set up the joint's bone
            Bone * bone;
            bone = new Bone;
            // This joint is the tail of this bone
            bone->tail = joint;
            // This joint's parent is the head of this bone
            bone->head = joint->parent;
            // This joint's bone is this bone
            joint->bone = bone;
            // Push this bone back onto the vector of bones
            bones.push_back (joint->bone);
        }
        joints.push_back (joint);

        current = joint;
    }

    // Initialize objects for bones and joints
    void InitObjects (void)
    {
        boneObject.location = glm::vec3 (0.0f,0.0f,0.0f);
        boneObject.rotation = glm::vec3 (0.0f,0.0f,0.0f);
        boneObject.scale = glm::vec3 (1.0f,1.0f,1.0f);
        boneObject.meshDir = "resources/models/Bone/Bone.obj";
        boneObject.hidden = false;
        boneObject.selected = false;
        boneObject.model.LoadModel(boneObject.meshDir);

        jointObject.location = glm::vec3 (0.0f,0.0f,0.0f);
        jointObject.rotation = glm::vec3 (0.0f,0.0f,0.0f);
        jointObject.scale = glm::vec3 (1.0f,1.0f,1.0f);
        jointObject.meshDir = "resources/models/Joint/Joint.obj";
        jointObject.hidden = false;
        jointObject.selected = false;
        jointObject.model.LoadModel(jointObject.meshDir);
    }

    // Calculate the result locations of all the roots
    void CalcResultLocations (void)
    {
        for (int i = 0; i < roots.size(); i++)
        {
            //if (roots[i]->parent == NULL) cout << "NULL" << endl;
            CalcResultLoc(roots[i]);
        }
    }

    // Recursively determine the result locations of a root and all its children
    void CalcResultLoc(Joint *joint)
    {
        if (joint->parent == NULL)
        {
            joint->resultLocation = joint->editLocation + joint->poseLocation;
        }
        else
        {
            // Account for inherited rotation from the joint's bone's parents
            glm::vec3 displacement = joint->editLocation - joint->parent->editLocation;
            displacement = glm::length(displacement)*glm::vec3(0.0f,1.0f,0.0f);
            displacement = glm::rotate(displacement, joint->bone->resultRotation.x, glm::vec3(1,0,0));
            displacement = glm::rotate(displacement, joint->bone->resultRotation.y, glm::vec3(0,1,0));

            joint->resultLocation = joint->parent->resultLocation + displacement;

        }

        for (int i = 0; i < joint->children.size(); i++)
        {
            CalcResultLoc(joint->children[i]);
        }
    }

    // Calculate the result rotations of all the bones using traditional methods
    void CalcResultRotations (void)
    {
        for (int i = 0; i < roots.size(); i++)
        {
            //if (roots[i]->parent == NULL) cout << "NULL" << endl;
            CalcResultRot(roots[i]);
        }
    }

    // Calculate the result rotations of bones
    void CalcResultRot (Joint *joint)
    {
        if (joint->bone == NULL)
        {
            for (int i = 0; i < joint->children.size(); i++)
            {
                joint->children[i]->bone->resultRotation = joint->children[i]->bone->editRotation + joint->children[i]->bone->poseRotation;
            }
        }
        else
        {
            // cout << "Bone was not null" <<endl;
            for (int i = 0; i < joint->children.size(); i++)
            {
                joint->children[i]->bone->resultRotation = joint->bone->resultRotation - joint->bone->editRotation + joint->children[i]->bone->editRotation + joint->children[i]->bone->poseRotation;
            }
        }
        // Do this for all of the bone's children
        for (int i = 0; i < joint->children.size(); i++)
        {
            CalcResultRot(joint->children[i]);
        }
    }

};


#endif // BONE_H_INCLUDED
