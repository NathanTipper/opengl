#version 330 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};

struct DirLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;
  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct SpotLight
{
  vec3 position;
  vec3 direction;

  float innerCone;
  float outerCone;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform Material material;
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

void main()
{
  vec3 norm = normalize(normal);
  vec3 viewDir = normalize(viewPos - fragPos);
  vec3 result = CalcDirLight(dirLight, norm, viewDir);


  for(int i = 0; i < NR_POINT_LIGHTS; ++i)
  {
    result += CalcPointLight(pointLights[i], norm, viewDir);
  }

#if 0
  result += CalcSpotLight(spotLight, norm, viewDir);
#endif
  FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(-light.direction);
  float diff = max(dot(normal, lightDir), 0.0);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

  return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(light.position - fragPos);

  float diff = max(dot(normal, lightDir), 0.0);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);

  float distance = length(lightDir);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(light.direction - fragPos);

  float diff = max(dot(normal, lightDir), 0.0);
  
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);

  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.innerCone - light.outerCone;
  float intensity = max((theta - light.outerCone) / epsilon, 0.0);
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
  ambient *= intensity;
  diffuse *= intensity;
  specular *= intensity;

  return (ambient + diffuse + specular);
}

