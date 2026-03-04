#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D tex1;
uniform vec3 lightColour;

void main()
{
    // 1. Get the full RGBA color from the texture
    vec4 texel = texture(tex1, TexCoord);

    // 2. Multiply ONLY the RGB parts by the lightColour
    vec3 litColor = texel.rgb * lightColour;

    // 3. Combine the lit RGB with the original Alpha
    FragColor = vec4(litColor, texel.a);
}