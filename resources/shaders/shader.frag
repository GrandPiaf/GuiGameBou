#version 450

struct Material{
    sampler2D texture_diffuse0;
    sampler2D texture_specular0;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 FragColor;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse0, texCoord).rgb;

    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, texCoord).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular0, texCoord).rgb;  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

}