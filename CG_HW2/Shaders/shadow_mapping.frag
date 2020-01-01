#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
	vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;	//光坐標系下的位置
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform sampler2D noiseTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float dissolvingThreshold;
uniform int dissolvingEffects;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	//正規化
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	//[-1, 1]移動到[0,1]
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

	//用表面向量的夾角調整bias的大小(0.05-0.005)
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	//PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0/textureSize(shadowMap, 0);
	for(int x = -1; x<=1;++x)
	{
		for(int y=-1; y<=1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	//超出燈光範圍
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}

    return shadow;
}

void main()
{         
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;

    // shadwing
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);    
	
	vec3 lighting = (1.0 - shadow) * color;
	//vec3 lighting = shadow * color;
	//vec3 lighting = shadow; 

	if(dissolvingEffects==1)
	{
		float value = texture(noiseTexture, fs_in.TexCoords).r;
		if(value < dissolvingThreshold)
		{
			discard;
		}else
		{
			FragColor = vec4(lighting, 1.0f);
		}
	}else if(dissolvingEffects == 2)
	{
		float value = texture(noiseTexture, fs_in.TexCoords).r;
		if(value < dissolvingThreshold)
		{
			FragColor = vec4(lighting, 0.0f);
		}else
		{
			FragColor = vec4(lighting, 1.0f);
		}
	}else
	{
		FragColor = vec4(lighting, 1.0f);
	}
	
}