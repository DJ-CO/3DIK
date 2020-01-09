#ifndef VIEWCONTROLS_H_INCLUDED
#define VIEWCONTROLS_H_INCLUDED

#include "camera.h"
#include <glew.h>

// I think this is an outdated header
// I no longer use the MouseData class in the final program because I'm no longer (that) stupid
// Also, view panning is now done in the Camera class, camera.h

const Uint8 *keyCodes = SDL_GetKeyboardState(NULL);

class MouseData
{
public:
    int oldX = 0;
    int oldY = 0;
    int newX = 0;
    int newY = 0;
    bool leftDown = 0;
    bool middleDown = 0;
    bool rightDown = 0;

    void Update (SDL_Event event)
    {
        oldX = newX;
        oldY = newY;
        newX = event.motion.x;
        newY = event.motion.y;
        if (event.type == SDL_PRESSED && event.button.button == SDL_BUTTON_MIDDLE) middleDown = true;
        if (event.type == SDL_RELEASED && event.button.button == SDL_BUTTON_MIDDLE) middleDown = false;
    }
};

void PanView (MouseData mouseData, Camera &camera, SDL_Event event)
{

 // IF key was right or d
    if (event.motion.xrel < 300 && event.motion.xrel > -300)
    {
       camera.Pan(RIGHT, -event.motion.xrel/5.0);
    }

     if (event.motion.yrel < 300 && event.motion.yrel > -300)
    {
       camera.Pan(UP, event.motion.yrel/5.0);
    }
}

#endif // VIEWCONTROLS_H_INCLUDED
