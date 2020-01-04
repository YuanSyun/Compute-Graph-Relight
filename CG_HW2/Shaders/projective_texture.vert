#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec4 ProjTexCoord;
out vec2 TexCoords;

uniform mat4 ProjectorMatrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vec4 pos4 = vec4(position, 1.0);
	gl_Position = projection * view * model * pos4;
	ProjTexCoord = ProjectorMatrix * model * pos4;
	TexCoords = texCoords;
}