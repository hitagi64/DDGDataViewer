#version 330 core
out vec4 FragColor;

in vec3 norm;

void main()
{
    float light = (dot(norm, vec3(0.0f, 1.0f, 0.3f))/2.0f)+0.5f;
    FragColor = vec4(light, light, light, 1.0f);
} 
