#version 450

uniform sampler2D texture_diffuse0;

uniform sampler2D texture_specular0;

in vec3 normal;
in vec2 texCoord;

out vec4 FragColor;

void main()
{
    FragColor = mix(texture(texture_diffuse0, texCoord), texture(texture_specular0, texCoord), 0.2);
}