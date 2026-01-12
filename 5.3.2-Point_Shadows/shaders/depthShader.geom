#version 330 core

layout (triangles) in;  // 3 triangle vertices
layout (triangle_strip, max_vertices=18) out;  // 6 triangles (6 * 3 equals 18 vertices)

uniform mat4 shadowMatrices[6];

out vec4 FragPos;  // FragPos from GS (output per emitvertex)


void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face;  // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i)  // for each triangle vertex
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}
