#version 330 core
out vec4 result;

in vec2 uv;

uniform sampler2D texIn;

void main()
{
    result = texture(texin, uv);
} 
