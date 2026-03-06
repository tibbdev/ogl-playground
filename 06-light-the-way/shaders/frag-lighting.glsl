#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D tex1;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;

// Phong Lighting model Implementation in a shader
void main()
{
    // Calculate Ambient Lighting
    float ambientStrength = 0.1;
    vec3 ambient    = ambientStrength * lightColour;

    // Calculate Diffuse Lighting
    vec3 norm       = normalize(Normal);
    vec3 lightDir   = normalize(lightPos - FragPos);
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColour;

    // Calculate Specular Lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColour;

    // 1. Get the full RGBA color from the texture
    vec4 texel = texture(tex1, TexCoord);

    // 2. Multiply ONLY the RGB parts of the texture by the lighting properties we've calculated
    vec3 litColor = texel.rgb * (ambient + diffuse + specular);

    // 3. Combine the lit RGB with the original Alpha
    FragColor = vec4(litColor, texel.a);
}