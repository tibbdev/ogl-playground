#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float factor;

void main()
{
    // This blends your vertex colors with your texture!
    // FragColor = texture(tex1, TexCoord) * vec4(ourColor, 1.0);
    // FragColor = mix(texture(tex1, TexCoord) * vec4(ourColor, 1.0), texture(tex2, TexCoord), 0.2);
    FragColor = mix(texture(tex1, TexCoord), texture(tex2, TexCoord), factor);
}