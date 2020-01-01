#version 330 core

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 uv;

// Values that stay constant for the whole mesh.
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main(){

	gl_Position =  lightSpaceMatrix * model * vec4(position, 1.0f);

	uv = texCoords;
}