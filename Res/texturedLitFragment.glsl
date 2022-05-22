#version 330 core
out vec4 FragColor;

in vec3 norm;
in vec2 uv;

uniform sampler2D texIn;

void main()
{
    //float light = (dot(norm, vec3(0.0f, 1.0f, 0.3f))/2.0f)+0.5f;
    //FragColor = vec4(light, light, light, 1.0f);
    //FragColor = vec4(uv, 1.0f, 1.0f);
    FragColor = texture(texIn, uv);
    FragColor.a = FragColor.a*2;
    if (FragColor.a < 0.01){
        discard;
    }
} 
