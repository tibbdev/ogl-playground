#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;
out vec4 vertexColour;

void main() 
{
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vertexColour = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable
}
