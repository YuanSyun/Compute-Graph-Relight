#version 330 core

in vec4 ProjTexCoord;
in vec2 TexCoords;

uniform sampler2D ProjectTex;
uniform float bias;

out vec4 FragColor;

void main()
{
	vec4 projTexColor = vec4(1.0, 0.0, 0.0, 1.0);
	projTexColor = texture(ProjectTex, ProjTexCoord.xy/ProjTexCoord.w);
	FragColor = projTexColor;
}