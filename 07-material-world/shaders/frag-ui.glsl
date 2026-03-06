#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D textTexture;

void main()
{
    // Fetch the color
    vec4 sampled = texture(textTexture, TexCoords);
    
    // If you're using Blended text, the letters are in the RGB 
    // and the transparency is in the Alpha.
    FragColor = sampled;
}
