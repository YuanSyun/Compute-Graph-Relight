#version 330 core

in vec4 ProjTexCoord;
in vec2 TexCoords;

uniform sampler2D ProjectTex;
uniform float bias;

out vec4 FragColor;

void main()
{
	vec4 projTexColor = vec4(0.0);
	if ( texture( ProjectTex, (ProjTexCoord.xy/ProjTexCoord.w) ).z  <  (ProjTexCoord.z-bias)/ProjTexCoord.w )
	{
		projTexColor = texture(ProjectTex, ProjTexCoord.xy/ProjTexCoord.w);
	}
	FragColor = projTexColor;
	//FragColor = texture(ProjectTex, TexCoords);
}