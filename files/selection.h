#ifndef SELECTION_H_INCLUDED
#define SELECTION_H_INCLUDED

#include <glew.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include "bone.h"

using namespace std;

void Objects_Selected (SDL_Event &event, vector <Object> &objects, unsigned int &FBO);
void Armature_Selected (SDL_Event &event, Armature armature, unsigned int &FBO);


void Objects_Selected (SDL_Event &event, vector <Object> &objects, unsigned int &FBO)
{

    glBindFramebuffer (GL_FRAMEBUFFER, FBO);
    // Find out which object is currently being hovered
    GLubyte * pixelB = new GLubyte[1];
    glReadPixels(event.motion.x, event.motion.y, 1, 1, GL_BLUE, GL_UNSIGNED_BYTE, pixelB);
    int selectedIndex = pixelB[0] -1;
    delete pixelB;

    /*
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
    {
        //cout << selectedIndex <<endl;
        if (objects[selectedIndex].selected == false) objects[selectedIndex].selected = true;

        for (int i = 0; i < objects.size(); i++)
        {
            if (i == selectedIndex) continue;
            objects[i].selected = false;
        }
        event.type = SDL_FIRSTEVENT;
    }
    */

    // Check Mouse stuff
    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT)
    {
        //cout << selectedIndex <<endl;
        if (objects[selectedIndex].selected == false) objects[selectedIndex].selected = true;
        else objects[selectedIndex].selected = false;

        for (int i = 0; i < objects.size(); i++)
        {
            if (i == selectedIndex) continue;
            objects[i].selected = false;
        }
        event.type = SDL_FIRSTEVENT;
    }
}

void Armature_Selected (SDL_Event &event, Armature armature, unsigned int &FBO, unsigned int HEIGHT)
{

    glBindFramebuffer (GL_FRAMEBUFFER, FBO);
    // Find out which object is currently being hovered

    GLubyte * pixelB = new GLubyte[1];
    glReadPixels(event.motion.x, HEIGHT - event.motion.y, 1, 1, GL_BLUE, GL_UNSIGNED_BYTE, pixelB);
    int selectedIndex = pixelB[0] -1;
    delete pixelB;

    // Check Mouse stuff
    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT)
    {
        // Reset all other joints
            for (int i = 0; i < armature.joints.size(); i++)
            {
                if (i == selectedIndex) continue;
                armature.joints[i]->selected = false;
            }
            // Reset all other bones
            for (int i = 0; i < armature.bones.size(); i++)
            {
                if (i == selectedIndex - armature.joints.size()) continue;
                armature.bones[i]->selected = false;
            }

        if (selectedIndex >=0)
        {
            // Select
            if (selectedIndex < armature.joints.size() && selectedIndex >= 0)
            {
                if (armature.joints[selectedIndex]->selected == false) armature.joints[selectedIndex]->selected = true;
                else armature.joints[selectedIndex]->selected = false;
            }
            else
            {
                int boneIndex = selectedIndex - armature.joints.size();
                if (armature.bones[boneIndex]->selected == false)
                {
                    armature.bones[boneIndex]->selected = true;
                    // Make bone's head and tail selected
                    if (!armature.pose)
                    {
                    armature.bones[boneIndex]->head->selected = true;
                    armature.bones[boneIndex]->tail->selected = true;
                    }
                }
                else
                {
                    armature.bones[boneIndex]->selected = false;
                    armature.bones[boneIndex]->head->selected = false;
                    armature.bones[boneIndex]->tail->selected = false;
                }
            }
        }


        event.type = SDL_FIRSTEVENT;
    }
}



#endif // SELECTION_H_INCLUDED
