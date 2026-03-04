#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D tex1;
uniform vec3 lightColour;
uniform vec3 lightPos;

void main()
{
    float ambientStrength = 0.1;

    vec3 ambient    = ambientStrength * lightColour;
    vec3 norm       = normalize(Normal);
    vec3 lightDir   = normalize(lightPos - FragPos);

    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColour;

    // 1. Get the full RGBA color from the texture
    vec4 texel = texture(tex1, TexCoord);

    // 2. Multiply ONLY the RGB parts by the lightColour
    vec3 litColor = texel.rgb * (ambient + diffuse);

    // 3. Combine the lit RGB with the original Alpha
    FragColor = vec4(litColor, texel.a);
}