#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texuv;

out vec2 uv;

uniform mat4 mvp;

void main()
{
    uv = texuv;
    gl_Position = mvp * vec4(pos, 1.0);
}
