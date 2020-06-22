#ifndef _shader_text_h
#define _shader_text_h
constexpr char vertexshader_text[] =
"#version 330 core\n\
layout(location = 0) in vec3 aPos;\n\
layout(location = 1) in vec3 aNormal;\n\
\n\
out vec3 FragPos;\n\
out vec3 Normal;\n\
\n\
uniform mat4 model;\n\
uniform mat4 view;\n\
uniform mat4 projection;\n\
uniform mat3 normal;\n\
\n\
void main()\n\
{\n\
	FragPos = (model * vec4(aPos, 1.0)).xyz;\n\
	Normal = normal * aNormal;\n\
	gl_Position = projection * view * vec4(FragPos, 1.0);\n\
}";

constexpr char fragmentshader_text[] =
"#version 330 core\n\
out vec4 FragColor;\n\
\n\
in vec3 Normal;\n\
in vec3 FragPos;\n\
\n\
uniform vec3 lightPos;\n\
uniform vec3 viewPos;\n\
uniform vec3 lightColor;\n\
uniform vec3 objectColor;\n\
\n\
const float ambientStrength = 0.1;\n\
const float specularStrength = 0.5;\n\
\n\
void main()\n\
{\n\
    // diffuse\n\
    vec3 norm = normalize(Normal);\n\
    vec3 lightDir = normalize(lightPos - FragPos);\n\
    float diff = max(dot(norm, lightDir), 0.0);\n\
\n\
    // specular\n\
    vec3 viewDir = normalize(viewPos - FragPos);\n\
    vec3 reflectDir = reflect(-lightDir, norm);\n\
    float spec = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32);\n\
\n\
    FragColor = vec4((ambientStrength + diff + spec) * lightColor * objectColor, 1.0);\n\
}";
#endif