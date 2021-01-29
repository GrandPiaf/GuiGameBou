#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec3 normal;
out vec2 texCoord;


void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    normal = aNormal;
    texCoord = aTexCoord;
}