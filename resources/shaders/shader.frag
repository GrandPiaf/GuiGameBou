#version 450

struct Material{
    sampler2D texture_diffuse0;
    sampler2D texture_specular0;
    float shininess;
};

struct DirectionalLight{
    // Light direction : must be inverted before making calculation in order to have the vector from the fragment to the light
    vec3 direction;
    // Light color
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    // Light position : from which we determine the vector from the fragment to the light 
    vec3 position;
    // Light color
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    // Attenuation : the further the light is, the less it affect the fragment
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight{
    // Light position & direction : from which we determine the vector from the fragment to the light 
    vec3 position;
    vec3 direction;
    // Angle cutoffs for smooth borders
    float cutOff; // inner
    float outerCutOff; // outer
    // Light color
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    // Attenuation : the further the light is, the less it affect the fragment
    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform Material material;

// Lights
uniform DirectionalLight dirLight;
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

// In
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Out
out vec4 FragColor;

// Function works like in C
vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos); 

    // Directional light
    vec3 result = CalculateDirectionalLight(dirLight, norm, viewDir);

    // Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalculatePointLight(pointLights[i], norm, FragPos, viewDir);
    };

    // Spot light
    result += CalculateSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0f);

}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir){

    vec3 lightDir = normalize(-light.direction);

    // ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse0, TexCoord).rgb;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, TexCoord).rgb;  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular0, TexCoord).rgb;  

    // Combine
    return (ambient + diffuse + specular);
};

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    vec3 lightDir = normalize(light.position - fragPos);
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));  

    // ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse0, TexCoord).rgb;

    // diffuse 
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, TexCoord).rgb;  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular0, TexCoord).rgb;  
    
    // If light is a point light it will attenuate
    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;   

    // Combine
    return (ambient + diffuse + specular);
};

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    // ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse0, TexCoord).rgb;
    
    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse0, TexCoord).rgb;  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular0, TexCoord).rgb;  
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    
    // attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    return (ambient + diffuse + specular);
};