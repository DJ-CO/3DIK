#version 330 core
#define MAX_NUMBER_OF_LIGHTS 20
#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2
#define MAXBYTE 256

in vec3 FragPos;

out vec4 colour;

uniform vec3 viewPos;
uniform vec3 selectionColour;

void main ( )
{
    vec3 viewDir = normalize( viewPos - FragPos );
    vec3 scaledColour = vec3 (selectionColour.r/MAXBYTE, selectionColour.g/MAXBYTE, selectionColour.b/MAXBYTE);
    colour = vec4 (scaledColour, 1.0);
}



