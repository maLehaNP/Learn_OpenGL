#version 330 core

out vec4 FragColor;
  
in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
};

uniform Material material;

const float offset = 1.0 / 300.0;

void main()
{ 
    //FragColor = texture(material.texture_diffuse1, TexCoords);

    // Post-processing

    // Inversion
    //FragColor = vec4(vec3(1.0 - texture(material.texture_diffuse1, TexCoords)), 1.0);

    // Grayscale
    //FragColor = texture(material.texture_diffuse1, TexCoords);
    //float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    //FragColor = vec4(average, average, average, 1.0);

    // Physically accurate grayscale
    //FragColor = texture(material.texture_diffuse1, TexCoords);
    //float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    //FragColor = vec4(average, average, average, 1.0);

    // Kernel effects

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    // Blur kernel
    //float kernel[9] = float[](
    //    1.0 / 16, 2.0 / 16, 1.0 / 16,
    //    2.0 / 16, 4.0 / 16, 2.0 / 16,
    //    1.0 / 16, 2.0 / 16, 1.0 / 16  
    //);
    // Emboss
    //float kernel[9] = float[](
    //    -2, -1, 0,
    //    -1,  1, 1,
    //     0,  1, 2
    //);
    // Outline
    //float kernel[9] = float[](
    //    -1, -1, -1,
    //    -1,  8, -1,
    //    -1, -1, -1
    //);
    // Sharpen
    float kernel[9] = float[](
         0, -1,  0,
        -1,  5, -1,
         0, -1,  0
    );
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(material.texture_diffuse1, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    FragColor = vec4(col, 1.0);
}
