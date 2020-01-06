#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
	vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace1;
    vec4 FragPosLightSpace2;
    vec4 ProjTexCoord;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix1;
uniform mat4 lightSpaceMatrix2;
uniform mat4 ProjectorMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

	//轉到模型空間
    vs_out.FragPos = vec3(model * vec4(position, 1.0));

	vs_out.Normal = transpose(inverse(mat3(model))) * normal;

    vs_out.TexCoords = texCoords;

	//轉到光空間下的位置
    vs_out.FragPosLightSpace1 = lightSpaceMatrix1 * vec4(vs_out.FragPos, 1.0);
    vs_out.FragPosLightSpace2 = lightSpaceMatrix2 * vec4(vs_out.FragPos, 1.0);

    //projective texture coord
    vs_out.ProjTexCoord = ProjectorMatrix * model * vec4(position, 1.0f);
}