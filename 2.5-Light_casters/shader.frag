#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 FragColor;

struct Material {
    sampler2D diffuse;  // Diffuse map
    sampler2D specular;  // Specular map
    float     shininess;
};

struct Light {
    vec4 vector;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    vec3 spotlightPosition;
    vec3 spotlightDirection;
    float cutOff;
    float outerCutOff;
};
  
uniform Material material;
uniform Light light;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
    vec3 lightDir;
    float distance;
    float attenuation = 1.0;

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));  // Ambient color is equal to the diffuse color

    vec3 norm = normalize(Normal);
    if (light.vector.w == 0.0)  // Direction vector. Note: be careful for floating point errors
        lightDir = normalize(-light.vector.xyz);
    else if(light.vector.w == 1.0) {  // Position vector
        lightDir = normalize(light.vector.xyz - FragPos);
        distance = length(light.vector.xyz - FragPos);  // Distance to the light source
        attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    }
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));  // Sample from the texture to retrieve the fragment's diffuse color value

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));  // Sample the specular map to retrieve the fragment's corresponding specular intensity\

    // Spotlight
    vec3 spotDir = normalize(light.spotlightPosition - FragPos);
    float theta = dot(spotDir, normalize(-light.spotlightDirection));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 spotlight = vec3(1.0) * intensity;

    ambient  *= attenuation + spotlight; 
    diffuse  *= attenuation + spotlight;
    specular *= attenuation + spotlight;

    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}
