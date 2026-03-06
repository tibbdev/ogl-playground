#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    // We pass coordinates in NDC space (-1 to 1)
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
}