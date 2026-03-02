#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
    // This blends your vertex colors with your texture!
    FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
}