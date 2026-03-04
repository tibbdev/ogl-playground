#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D tex1;

uniform vec3 modColor;

void main()
{
    FragColor = vec4(modColor, 1.0);
}