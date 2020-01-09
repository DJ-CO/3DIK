// Armature System Generator
// (c) D.J. Denton 2018 ICS4UI

// Basic Includes
#include <iostream>
#include <stdio.h>
#include <glew.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_opengl.h>

// Texture library
#include <SOIL2.h>

// Transformation libraries
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/vector_angle.hpp>

// Custom Shaders
#include "files/shader.h"

// Custom Headers
#include "files/camera.h"
#include "mesh.h"
#include "files/model.h"
#include "files/object.h"
#include "files/skybox.h"
#include "files/selection.h"
#include "files/viewControls.h"
#include "files/bone.h"
#include "files/render.h"
#include "files/transformations.h"

// Miscellaneous defines
// A PI constant because I think glm works in radians
#define PI 3.14159265359
// The number of lights
#define NUMBER_OF_LIGHTS 2
// Defines for the types of lights
#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2

using namespace std;

// Screen constants
const int WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

// Function to control camera movement
void DoMovement(SDL_Event event, Armature &armature);

// GEt keys
void GetKeys (SDL_Event event);

//create camera
Camera camera(glm::vec3 (0.0f, 0.0f, 3.0f));

// For reading keyboard
const Uint8 *keys = SDL_GetKeyboardState(NULL);

// Main function
int main(int argc, char *argv[])
{
   // Initialize SDL
   // Initializes the specific part of SDL that can use the opengl window
    SDL_Init(SDL_INIT_VIDEO);
    // Makes code forward compatable???
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    // Create a window variable and stencil buffer
    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    // Create context. Must be deleted at the end.
    SDL_GLContext context = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    glewInit();
    // Initialize everything, and print an error if it doesn't initialize correctly
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        //
        cout << "SDL could not initialize! SDL error: " << SDL_GetError() << endl;
    }
    // Print error if window hasn't been created correctly
    if (NULL == window)
    {
        //
        cout << "SDL could not create window! SDL error: " << SDL_GetError() << endl;
        return -1;
    }
     // Create SDL window event
    SDL_Event windowEvent;

    // Initialize shaders and OpenGL related things
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    // Enable Alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Create variable for main shader
    Shader shader ("resources/shaders/reflection.vs", "resources/shaders/reflection.frag");
     // Create variable for skybox shader
    Shader skyboxShader ("resources/shaders/skybox.vs", "resources/shaders/skybox.frag");
    Shader selectionShader ("resources/shaders/selection.vs", "resources/shaders/selection.frag");
    // Create vector of the cube map face textures
    vector<string> faces;
    faces.push_back("resources/images/skybox/right.jpg");
    faces.push_back("resources/images/skybox/left.jpg");
    faces.push_back("resources/images/skybox/top.jpg");
    faces.push_back("resources/images/skybox/bottom.jpg");
    faces.push_back("resources/images/skybox/front.jpg");
    faces.push_back("resources/images/skybox/back.jpg");
    // Create a variable that holds the cubemap texture
    unsigned int cubemapTexture = LoadCubeMap(faces);

    // Make a framebuffer to keep the image made by the selection shader
    unsigned int selectionFBO;
    glGenFramebuffers (1, &selectionFBO);
    glBindFramebuffer (GL_FRAMEBUFFER, selectionFBO);
    // Make a texture for the main rendered image
    unsigned int selectionTexture;
    glGenTextures(1, &selectionTexture);
    glBindTexture(GL_TEXTURE_2D, selectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Attach
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, selectionTexture, 0);
    // Make a render buffeer object
    unsigned int selectionRBO;
    glGenRenderbuffers(1, &selectionRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, selectionRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // Attach
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, selectionRBO);
    // Check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)cout << "Framebuffer incomplete.";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Get all objects (the models for bones and joints
    vector <Object> objects;
    LoadAllObjects(objects);

    // Make an Armature;
    Armature armature;
    armature.CreateJoint(NULL);
    armature.InitObjects();

    // Load and initialize lights
    Light lights[NUMBER_OF_LIGHTS];
    lights[0].location = glm::vec3 (10.0f,1.0f,2.0f);
    lights[0].type = POINT;
    lights[0].diffuse = glm::vec3 (10.0f,10.0f,10.0f);
    lights[0].ambient = glm::vec3 (0.5f,0.5f,0.5f);
    lights[0].specular = glm::vec3 (0.0f,0.0f,0.0f);
    lights[0].direction = glm::vec3 (0.0f,0.0f,0.0f);
    lights[0].constant = 0.0;
    lights[0].linear = 0.0;
    lights[0].quadratic = 1.0;
    lights[0].cutOff = 1.0;
    lights[0].outerCutOff = 1.0;
    lights[0].index = "0";

    lights[1].location = glm::vec3 (-10.0f,1.0f,2.0f);
    lights[1].type = POINT;
    lights[1].diffuse = glm::vec3 (100.0f,100.0f,100.0f);
    lights[1].ambient = glm::vec3 (0.5f,0.5f,0.5f);
    lights[1].specular = glm::vec3 (0.0f,0.0f,0.0f);
    lights[1].direction = glm::vec3 (0.0f,0.0f,0.0f);
    lights[1].constant = 0.0;
    lights[1].linear = 0.0;
    lights[1].quadratic = 1.0;
    lights[1].cutOff = 12.5;
    lights[1].outerCutOff = 17.5;
    lights[1].index = "1";

    // Create projection matrix
    glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
    windowEvent.motion.x = 0;
    windowEvent.motion.y = 0;
    windowEvent.motion.xrel = 0;
    windowEvent.motion.yrel = 0;

    // MAIN LOOP
    while (true)
    {
        // Check if something is happening with the window
        if (SDL_PollEvent(&windowEvent))
        {
            // If the quit event has been triggered, break the loop, thus exiting the program
            if (SDL_QUIT == windowEvent.type)
            {
                break;
            }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
            {
                break;
            }
        }

        // Handle the movement of the camera
        DoMovement(windowEvent, armature);
        //PanView (mouseData, camera, windowEvent);
        GLint viewPosLoc;
        GLint selectionColourLoc;
        GLint modelLoc;
        GLint viewLoc;
        GLint projLoc;
        glm::mat4 view;
        view = camera.GetViewMatrix();

        // Calculate bone transforms
        CalcTrans (armature);
        armature.CalcResultLocations();
        armature.CalcResultRotations();
        // Render selection shader
        RenderArmature (selectionShader, camera, projection, NUMBER_OF_LIGHTS, selectionFBO, armature, true, lights);
        // Render shader
        RenderArmature (shader, camera, projection, NUMBER_OF_LIGHTS, 0, armature, false, lights);
        Armature_Selected(windowEvent, armature, selectionFBO, HEIGHT);
        // Swap screen buffers
        SDL_GL_SwapWindow(window);
    }

    // CLEAN UP
    glDeleteFramebuffers (1, &selectionFBO);
    // Destroy the window before exiting
    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(context);
    // Quit SDL
    SDL_Quit();
    return 0;
}

// FUNCTIONS
void DoMovement(SDL_Event event, Armature &armature)
{
    // Add a bone
    if ( (keys [SDL_SCANCODE_A] ) && (keys [SDL_SCANCODE_LSHIFT]) )
    {
        // Go through joints and add a bone if the joint is selected
        for (int i = 0; i < armature.joints.size(); i++)
        {
            if (armature.joints[i]->selected)
            {
                armature.CreateJoint(armature.joints[i]);
                armature.joints[i]->selected = false;
            }
        }
    }

    // Rotate view
    if (event.type == SDL_MOUSEMOTION && event.button.button == SDL_BUTTON_MIDDLE && !(keys [SDL_SCANCODE_LSHIFT]))
    {
        // Create floats to hold the x and y velocities of the mouse
        GLfloat xOffset = 0, yOffset = 0;

        // Find the x and y velocities of the mouse
        xOffset = event.motion.xrel;
        yOffset = -event.motion.yrel;
        // Process how much to rotate the screen based on mouse velocity
        camera.ProcessMouseMovement(xOffset, yOffset);
    }

    // Pan
    if (event.type == SDL_MOUSEMOTION && event.button.button == SDL_BUTTON_MIDDLE && (keys [SDL_SCANCODE_LSHIFT]))
    {
        //cout << event.motion.xrel << " , " << event.motion.yrel << endl;
        camera.Scrollpan(event.motion.xrel, event.motion.yrel, 0);
    }

    // zoom
    if (event.type == SDL_MOUSEWHEEL)
    {
        //cout << event.motion.xrel << " , " << event.motion.yrel << endl;
        camera.Scrollpan(0, 0, 5*event.wheel.y);
    }


    // Translate
    if ((keys [SDL_SCANCODE_G])&& event.type == SDL_MOUSEMOTION)
    {
        //for
        Translate (camera.GetFront(), camera.GetUp(), camera.GetRight(), armature, event.motion.xrel, event.motion.yrel);
    }

    // Rotate
    if ((keys [SDL_SCANCODE_R])&& event.type == SDL_MOUSEMOTION)
    {
        Rotate (camera.GetFront(), camera.GetUp(), camera.GetRight(), armature, event.motion.xrel, event.motion.yrel);
    }

    // Toggle Pose Mode and edit mode
    if ((keys [SDL_SCANCODE_P])||(keys [SDL_SCANCODE_E]))
    {
        // Reset the selection of all bones and joints when transitioning between modes, to prevent
        // invalid selection patterns from carrying over from one mode to another.
        for (int i = 0; i < armature.joints.size(); i++) armature.joints[i]->selected = false;
        for (int i = 0; i < armature.bones.size(); i++) armature.bones[i]->selected = false;
        // Toggle Pose mode
        if ((keys [SDL_SCANCODE_P]))armature.pose = true;
        // Toggle Edit Mode
        if ((keys [SDL_SCANCODE_E]))armature.pose = false;
    }
}

