#version 330 core

in vec3 FragPos;
in vec3 Normal;

struct PointLight {
    vec3 position;
    vec3 color;

    float ambientStrength;
    float diffuseStrength;
    float specularStrength;

    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 4

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;
uniform vec3 viewPos;
uniform vec3 uColor;

out vec4 FragColor;

vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * distance * distance);

    vec3 ambient = light.ambientStrength * light.color;
    vec3 diffuse = light.diffuseStrength * diff * light.color;
    vec3 specular = light.specularStrength * spec * light.color;

    return (ambient + diffuse + specular) * attenuation;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lighting = vec3(0.0);
    for (int i = 0; i < numPointLights; ++i) {
        lighting += calcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    vec3 result = lighting * uColor;
    FragColor = vec4(result, 1.0);
}
