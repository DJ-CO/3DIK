#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

void RenderArmature (Shader &shader, Camera &camera,  glm::mat4 projection, unsigned int NUMBER_OF_LIGHTS, unsigned int FBO, Armature &armature, bool selection, Light lights[]);


void RenderArmature (Shader &shader, Camera &camera,  glm::mat4 projection, unsigned int NUMBER_OF_LIGHTS, unsigned int FBO, Armature &armature, bool selection, Light lights[])
{
    // RENDER
    GLint viewPosLoc;
    GLint selectionColourLoc;
    GLint modelLoc;
    GLint viewLoc;
    GLint projLoc;
    glm::mat4 view;
    view = camera.GetViewMatrix();

    // Render the selection pass
    if (selection) glBindFramebuffer (GL_FRAMEBUFFER, FBO);
    else glBindFramebuffer (GL_FRAMEBUFFER, 0);

    // Set the background colour
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the specified shader
    shader.Use();
    // Set the uniforms for the shader
    viewPosLoc = glGetUniformLocation(shader.Program, "viewPos");
    selectionColourLoc = glGetUniformLocation(shader.Program, "selectionColour");
    glUniform3f (viewPosLoc, camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition().z );
    modelLoc = glGetUniformLocation ( shader.Program, "model");
    viewLoc = glGetUniformLocation ( shader.Program, "view");
    projLoc = glGetUniformLocation ( shader.Program, "projection");
    glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 32.0f);
    glUniform1i(glGetUniformLocation(shader.Program, "NUMBER_OF_LIGHTS"), NUMBER_OF_LIGHTS);

    glUniformMatrix4fv ( viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv ( projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Make all lights work
    for (int i = 0; i < NUMBER_OF_LIGHTS; i++)
    {
        lights[i].Draw(shader);
    }

    // For loops for armature
    // One for joints
    for (int i = 0; i < armature.joints.size(); i++)
    {
        // Show if object is selected
        if (armature.joints[i]->selected == true)
        {
            // If it's selected, change its colour a bit
            if (armature.pose) glUniform3f (glGetUniformLocation (shader.Program, "overrideColour"), 0.7, 0.0, 0.0);
            else  glUniform3f (glGetUniformLocation (shader.Program, "overrideColour"), 0.4, 0.2, 0.1);
        }
        else
        {
            glUniform3f (glGetUniformLocation (shader.Program, "overrideColour"), 0.0, 0.0, 0.0);
        }
        // Prepare to apply all transformations to all models
        glm::mat4 model;
        // If we're in pose mode, render pose locations, otherwise use the edit locations
        if (armature.pose)
        {
            model = glm::translate(model, armature.joints[i]->resultLocation);
        }
        else
        {
            // Apply translations
            model = glm::translate(model, armature.joints[i]->editLocation);
        }

        // Break for selection shader vs regular
        if (selection)
        {
            // Calculate which colour to put on the object in the selection shader using the joint's index in the joint array
            int index = i + 1;
            int r = (index / int (256*256));
            index -= 256*256*r;
            int g = index / int (256);
            index -= 256*g;
            int b = index;
            index -= b;
            glUniform3f (selectionColourLoc, r, g, b);
        }
        // Apply all transformations
        glUniformMatrix4fv (glGetUniformLocation (shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // still need to draw the model
        armature.jointObject.model.Draw(shader);
    }

    // One for bones
    for (int i = 0; i < armature.bones.size(); i++)
    {
        // Show if object is selected
        if (armature.bones[i]->selected == true)
        {
             // If it's selected, change its colour a bit
            if (armature.pose) glUniform3f (glGetUniformLocation (shader.Program, "overrideColour"), 0.0, 0.2, 0.5);
            else  glUniform3f (glGetUniformLocation (shader.Program, "overrideColour"), 0.4, 0.2, 0.1);
        }
        else
        {
            glUniform3f (glGetUniformLocation (shader.Program, "overrideColour"), 0.0, 0.0, 0.0);
        }

        // Prepare to apply all transformations to all models
        glm::mat4 model;
        // Draw the Bone
        glm::vec3 scale;
        glm::vec3 location;
        glm::vec3 rotation = glm::vec3(0,0,0);
        glm::vec3 displacement = glm::vec3(0,0,0);

        // Calculate and apply transformations of each bone for pose mode
        if (armature.pose)
        {
            model = glm::translate(model, armature.bones[i]->head->resultLocation);
            model = glm::scale(model, armature.bones[i]->poseScale + armature.bones[i]->editScale);
            model = glm::rotate(model, armature.bones[i]->resultRotation.z, glm::vec3(0.0f,0.0f,1.0f));
            model = glm::rotate(model, armature.bones[i]->resultRotation.y, glm::vec3(0.0f,1.0f,0.0f));
            model = glm::rotate(model, armature.bones[i]->resultRotation.x, glm::vec3(1.0f,0.0f,0.0f));
        }
        else
        {
            // Calculate and apply transformations of each bone for edit mode
            model = glm::translate(model, armature.bones[i]->editLocation);
            model = glm::scale(model, armature.bones[i]->editScale);
            model = glm::rotate(model, armature.bones[i]->editRotation.z, glm::vec3(0.0f,0.0f,1.0f));
            model = glm::rotate(model, armature.bones[i]->editRotation.y, glm::vec3(0.0f,1.0f,0.0f));
            model = glm::rotate(model, armature.bones[i]->editRotation.x, glm::vec3(1.0f,0.0f,0.0f));
        }

        // Possibly do selection stuff
        if (selection)
        {
            // Calculate which colour to put on the object in the selection shader using the bone's index in the bone array
            int index = i + armature.joints.size() + 1;
            int r = (index / int (256*256));
            index -= 256*256*r;
            int g = index / int (256);
            index -= 256*g;
            int b = index;
            index -= b;
            glUniform3f (selectionColourLoc, r, g, b);
        }

        // Apply all transformations
        glUniformMatrix4fv (glGetUniformLocation (shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        armature.boneObject.model.Draw(shader);
    }
}
#endif // RENDER_H_INCLUDED
