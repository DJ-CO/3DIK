#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <iostream>
#include <fstream>
#include <vector>
#include "model.h"
#include <string>
#include <SDL_opengl.h>
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#define OBJECT_LIST_DIR "resources/objects/object_list.txt"

// Include everything needed for models?
// Include glm for vector math and stuff

using namespace std;

// I think the object class is left over from my physics simulation
// I use an Armature class for this program

// Object Class
class Object
{
public:
    glm::vec3 location;// Location
    glm::vec3 rotation;// Rotation
    glm::vec3 scale;// Scale
    glm::vec3 velocity;// Velocity

    float mass;// Mass
    float elasticity;// Elasticity

    bool hidden;// Whether the object should be drawn

    bool selected;// Whether the object is selected or not

    GLchar * meshDir;// Mesh directory for the model

    GLchar * objectFileDir;

    string name;// The object's name

    Model model;// The object's model
};

// Light
class Light
{
public:
    glm::vec3 location;// Location
    glm::vec3 diffuse;// RGB of diffuse
    glm::vec3 ambient;// RGB of ambient
    glm::vec3 specular;// RGE of specular
    glm::vec3 direction;// Components of direction
    float constant;// Amount of constant light
    float linear;// Amount of linear falloff
    float quadratic;// Amount of quadratic falloff
    float cutOff;
    float outerCutOff;
    int type;// The type of light: 0 for point light, 1 for directional light, 2 for spot light
    string index;//The "birth number" of the light


    void Draw( Shader shader )// A function meant to be used in a loop to automate the process of passing all uniform information to the fragment shader
    {
        // OpenGL is weird. I need to specify the exact name of the uniform I want to find the location of, but in a GLchar
        // This means that (the way it is intended to be done) you have to explicity declare every single uniform affected
        // by every single light, individually. This is a problem if you have many lights because, even if they were identical
        // and you stored them neatly in an array, you would have to specifically type out the index of each array when activating the lights,
        // defeating the purpose of having them in an array.

        // To remedy this situation, I thought I could have a function (this function) that does the necessary uniform setting
        // for any given light in the light array, and cycle through this function with each different index of light. I thought
        // I could concatinate the string representing the name of the uniform with the index of the light (which is passed in as
        // string) automating the process, but alas, I needed a GLchar *.

        // GLchar *'s don't concatinate, and (while the entire array of characters can be changed) individual indices cannot

        // For instance:
        // GLchar * word = "light[i].position" ;//works
        // word = "light[0].position";// Still works
        // cout << word [6]; //Outputs "0" as expected
        // word [6] = "1";// Gives an error
        // word [6] = '1';// Gives no error *******BUT CRASHES AT RUNTIME*********
        // So simply changing the [index] part of the GLchar * to match the index of the light is impossible

        // However, through some witchcraft, I managed to concatinate some strings, then somehow convert them to a GLchar const * (it only works for const)

        // This is a lot of code, but it automates the process

        string startPath = "light[" + index;// A string for the beginning part of the uniform name (will be constant, except for the index)
        string endPath = "].position";// A string for the end part of the uniform name (will be changed)
        string wholePath = startPath + endPath;// Concatinating the strings into one
        GLchar const * whichUniform = wholePath.c_str();// Setting th GLchar const * to the name of the uniform (concatination cannot be done in this step)
        glUniform3f (glGetUniformLocation(shader.Program, whichUniform), location.x, location.y, location.z); // Setting the uniform based on the light's parameters

        endPath = "].ambient"; // Changing the end of the uniform name to affect the ambient light
        wholePath = startPath + endPath; // Re-concatinating
        whichUniform = wholePath.c_str(); // Changing the GLchar const * to the new re-concatinated variable
        glUniform3f (glGetUniformLocation(shader.Program, whichUniform), ambient.r, ambient.g, ambient.b);

        endPath = "].diffuse"; // The rest is all the same
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform3f (glGetUniformLocation(shader.Program, whichUniform), diffuse.r, diffuse.g, diffuse.b);

        endPath = "].specular";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform3f (glGetUniformLocation(shader.Program, whichUniform), specular.r, specular.g, specular.b );

        endPath = "].direction";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform3f (glGetUniformLocation(shader.Program, whichUniform), direction.x, direction.y, direction.z);


        endPath = "].constant";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation(shader.Program, whichUniform), constant);

        endPath = "].linear";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation(shader.Program, whichUniform), linear);

        endPath = "].quadratic";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation(shader.Program, whichUniform), quadratic);

        endPath = "].cutOff";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation (shader.Program, whichUniform), glm::cos(glm::radians(cutOff)));

        endPath = "].outerCutOff";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation (shader.Program, whichUniform), glm::cos(glm::radians(outerCutOff)));

        endPath = "].type";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation (shader.Program, whichUniform), type);

        // For reference; the non-automated version of the code would look like this
        // glUniform3f (glGetUniformLocation(shader.Program, "light[0].position"), location.x, location.y, location.z);
    }

};

// Function to load all objects from a folder, returns a list of all objects
void LoadAllObjects (vector <Object> &allObjects);

// Function to get the object list
vector <string> GetObjectList (void);

// Function to load a single object from a file
void LoadObjectFromFile (Object &objects);

// Determines whether a file can be loaded
bool CanLoadFile (GLchar * Dir);



// Function to load all objects from a folder, has a list of all objects
void LoadAllObjects (vector <Object> &allObjects)
{
    // Get object list
    vector <string> objectList = GetObjectList();
    for (int i = 0; i < objectList.size(); i++)
    {
        Object tempObject;
        char * tempChar;
        tempChar = new char [objectList[i].length()];
        strcpy(tempChar, objectList[i].c_str());
        tempObject.objectFileDir = tempChar;
        if (CanLoadFile ( tempObject.objectFileDir ))
        {

            LoadObjectFromFile(tempObject);
            tempObject.model.LoadModel(tempObject.meshDir);
            allObjects.push_back(tempObject);
        }
    }
}
//Model model;// The object's model

vector <string> GetObjectList (void)
{
    // Go through the object list, find out how many objects there are, what their directories are
    vector <string> objectList;

    if (CanLoadFile (OBJECT_LIST_DIR))
    {
        ifstream fin;
        fin.open (OBJECT_LIST_DIR);
        // Read through the file to get all of the objects
        do
        {
            string tempString;
            getline(fin, tempString);
            objectList.push_back(tempString);
        }
        while(!fin.eof());
        fin.close();
    }
    return objectList;
}

// NOT DONE
// Function to load a single object from a file
void LoadObjectFromFile (Object &object)
{
    // Create input file
    ifstream fin;
    fin.open (object.objectFileDir);

    // Read the object file and assign properties to the object
    do
    {
        string tempString;
        // Get object's name
        getline(fin, tempString);
        object.name = tempString;
        float temp = 0;
        // Get location
        object.location = glm::vec3  (0,0,0);
        // Get x location
        fin >> temp;
        object.location += glm::vec3 (temp,0,0);
        // Get y location
        fin >> temp;
        object.location += glm::vec3  (0,temp,0);
        // Get z location
        fin >> temp;
        object.location += glm::vec3  (0,0,temp);

        // Get rotation
        object.rotation = glm::vec3  (0,0,0);
        // Get x location
        fin >> temp;
        object.rotation += glm::vec3  (temp,0,0);
        // Get y location
        fin >> temp;
        object.rotation += glm::vec3  (0,temp,0);
        // Get z location
        fin >> temp;
        object.rotation += glm::vec3  (0,0,temp);

         // Get scale
        object.scale = glm::vec3  (0,0,0);
        // Get x scale
        fin >> temp;
        object.scale += glm::vec3  (temp,0,0);
        // Get y scale
        fin >> temp;
        object.scale += glm::vec3  (0,temp,0);
        // Get z scale
        fin >> temp;
        object.scale += glm::vec3  (0,0,temp);

        // Get hidden value
        fin >> temp;
        if (temp) object.hidden = true;
        else object.hidden = false;

        // Get mesh directory
        fin >> tempString;
        char * tempChar;
        tempChar = new char [tempString.length()];
        strcpy(tempChar, tempString.c_str());
        object.meshDir = tempChar;

    }
    while( false && !fin.eof());
    // Close the file
    fin.close();
    object.selected = false;
}

// DONE
bool CanLoadFile (GLchar * Dir)
{
    // Create input file
    ifstream fin;
    fin.open (Dir);
    // Check that file is open
    if (!fin.is_open()) return false;
    // Close the file
    fin.close();
    // File is valid
    return true;
}


#endif // OBJECT_H_INCLUDED
