#version 330 core
out vec4 result;

in vec2 uv;

uniform sampler2D texIn;

void main()
{
    result = texture(texIn, uv);
    //result = vec4(1.0f, 1.0f, 1.0f, 1.0f);
} 
