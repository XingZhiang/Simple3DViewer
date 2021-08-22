#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 Tangent;
out vec3 Bitangent;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 scale;

void main()
{
   TexCoords = aTexCoords;
   Tangent = aTangent;
   Bitangent = aBitangent;
   FragPos = vec3(model * vec4(aPos,1.0f));
   Normal =  mat3(transpose(inverse(model))) * aNormal;
   gl_Position = scale * projection * view * model * vec4(aPos, 1.0);
}