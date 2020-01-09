#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

class Mouse
{
public:

    bool leftIsDown;
    bool rightIsDown;
    bool middleIsDown;

    // Updates the state of the mouse using SDL because SDL apparently is terrible and can't do that itself
    void UpdateState (SDL_Event event)
    {
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            cout << "DOWN" <<endl;
            if (event.button.button == SDL_BUTTON_LEFT) leftIsDown = true;
            if (event.button.button == SDL_BUTTON_MIDDLE) middleIsDown = true;
            if (event.button.button == SDL_BUTTON_RIGHT) rightIsDown = true;
        }
        if (event.type == SDL_MOUSEBUTTONUP)
        {
            cout << "UP" <<endl;
            if (event.button.button == SDL_BUTTON_LEFT) leftIsDown = false;
            if (event.button.button == SDL_BUTTON_MIDDLE) middleIsDown = false;
            if (event.button.button == SDL_BUTTON_RIGHT) rightIsDown = false;
        }
    }

    void Init (void)
    {
    leftIsDown = 0;
    rightIsDown = 0;
    middleIsDown = 0;
    }
};

#endif // INPUT_H_INCLUDED
