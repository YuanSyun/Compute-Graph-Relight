#version 330 core

in vec2 uv;

out vec4 FragColor;

uniform float dissolvingThreshold;
uniform sampler2D noiseTexture;
uniform int dissolvingEffects;

void main(){
	
	if(dissolvingEffects == 1)
	{
		float value = texture(noiseTexture, uv).r;

		if(value < dissolvingThreshold)
		{
			discard;
		}
	}else if(dissolvingEffects == 2)
	{
		float value = texture(noiseTexture, uv).r;

		if(value < dissolvingThreshold)
		{
			FragColor = vec4(1.0, 1.0, 1.0, 0.0);
		}
	}
	
}