#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;

out vec3 color;

uniform mat4 mvp;

void main()
{
    color = col;
    gl_Position = mvp * vec4(pos, 1.0);
}
