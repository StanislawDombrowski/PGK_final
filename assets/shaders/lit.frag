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

uniform PointLight pointLight;
uniform vec3 viewPos;
uniform vec3 uColor;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);

    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance +
                               pointLight.quadratic * distance * distance);

    vec3 ambient = pointLight.ambientStrength * pointLight.color;
    vec3 diffuse = pointLight.diffuseStrength * diff * pointLight.color;
    vec3 specular = pointLight.specularStrength * spec * pointLight.color;

    vec3 result = (ambient + diffuse + specular) * attenuation * uColor;
    FragColor = vec4(result, 1.0);
}
