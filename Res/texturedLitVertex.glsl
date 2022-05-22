#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texuv;
uniform mat4 mvp;

out vec3 norm;
out vec2 uv;

void main()
{
    uv = texuv;
    norm = normal;
    gl_Position = mvp * vec4(position, 1.0);
}
