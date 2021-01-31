#version 450

struct Material{
    sampler2D texture_diffuse0;
    sampler2D texture_specular0;
    float shininess;
};

struct Light {
    vec4 lightVector;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
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

    float attenuation = 1.0f;
    vec3 lightDir;

    if(light.lightVector.w == 0.0){ //Directional
        lightDir = normalize(-light.lightVector.rgb);
    }else if(light.lightVector.w == 1.0){ //Point light
        lightDir = normalize(light.lightVector.rgb - fragPos);
        float distance = length(light.lightVector.rgb - fragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
    }

    // ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse0, texCoord).rgb;

    // diffuse 
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, texCoord).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular0, texCoord).rgb;  
    
    // If light is a point light it will attenuate
    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;   

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

}