#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    //gl_Position = projection * view * vec4(aPos, 1.0);

    // Optimization: setting z = w, so when the perspective division is applied its z component translates to w / w = 1.0.
    // It's give us possibility to render skybox last.
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
