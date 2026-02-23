#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

uniform mat4 transform;
uniform mat4 translate;

out vec3 vertexColour;

void main() 
{
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // gl_Position = vec4(gl_Position.x + translate.x, gl_Position.y + translate.y, gl_Position.z + translate.z, 1.0f);
    vertexColour = aColour;
}
