#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

uniform Material material;


void main()
{    
    // FragColor = texture(material.texture_diffuse1, TexCoords);

    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0);  // Non-linear depth visualization

    // Linear depth visualization
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    FragColor = vec4(vec3(depth), 1.0);
}
