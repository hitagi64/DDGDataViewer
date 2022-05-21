#version 330 core
layout (location = 0) in vec4 position;
uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(position.xyz, 1.0);
}
