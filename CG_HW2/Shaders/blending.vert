#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec4 ProjTexCoord;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 ProjectorMatrix;

void main(){

    gl_Position = projection * view * model * vec4(position, 1.0f);

    //projective texture coord
    vs_out.ProjTexCoord = ProjectorMatrix * model * vec4(position, 1.0f);
}