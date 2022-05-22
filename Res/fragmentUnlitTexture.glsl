#version 330 core
out vec4 result;

in vec2 uv;

uniform sampler2D texIn;

void main()
{
    result = texture(texIn, uv);
    result.a = result.a*2;
    if (result.a < 0.01){
        discard;
    }
} 
