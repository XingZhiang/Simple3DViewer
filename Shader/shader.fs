#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 Tangent;
in vec3 Bitangent;

struct Material{
   sampler2D specular;
   float shininess;
};

struct Light{
   vec3 position;
   vec3 direction;
   float cutOff;
   float outerCutOff;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float constant;
   float linear;
   float quadratic;
};

uniform vec3 viewPos;
uniform Light light;
uniform Material material;
uniform sampler2D texture_diffuse1;

void main()
{
   vec3 lightDir = normalize( light.position - FragPos);
   float theta = dot(lightDir,normalize(-light.direction));
   // 衰减
   float distance    = length(light.position - FragPos);
   float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
   // 环境光
   vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;

   // 漫反射
   vec3 norm = normalize(Normal);
   float diff = max(dot(norm, lightDir), 0.0f);
   vec3 diffuse= light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

   //镜面光
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   vec3 specular = light.specular * spec * texture(texture_diffuse1, TexCoords).rgb;

   // 虚化
   float epsilon = (light.cutOff - light.outerCutOff);
   float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
   diffuse  *= intensity;
   specular *= intensity;

   // 衰减
   ambient  *= attenuation;
   diffuse  *= attenuation;
   specular *= attenuation;

   vec3 res = ambient + diffuse + specular;
   FragColor = vec4(res, 1.0);
}