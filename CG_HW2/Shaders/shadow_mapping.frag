#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
	vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace1;	//光坐標系下的位置
	vec4 FragPosLightSpace2;	//光坐標系下的位置
	vec4 ProjTexCoord;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float dissolvingThreshold;
uniform int dissolvingEffects;
uniform float bias;

float ShadowCalculation(sampler2D shadow_texture, vec4 fragPosLightSpace)
{
	//正規化
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	//[-1, 1]移動到[0,1]
    projCoords = projCoords * 0.5 + 0.5;

	//sample the shadow map
    float closestDepth = texture(shadow_texture, projCoords.xy).r;
    float currentDepth = projCoords.z;

	//用表面向量的夾角調整bias的大小(0.05-0.005)
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	//PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0/textureSize(shadow_texture, 0);
	for(int x = -1; x<=1;++x)
	{
		for(int y=-1; y<=1; ++y)
		{
			float pcfDepth = texture(shadow_texture, projCoords.xy + vec2(x, y) * texelSize).r;
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
    // smapling the shadow image and the hightlight image
	vec3 t0 = texture(texture0, fs_in.ProjTexCoord.xy/fs_in.ProjTexCoord.w).xyz;
	vec3 t1 = texture(texture1, fs_in.ProjTexCoord.xy/fs_in.ProjTexCoord.w).xyz;

    // blending the reulst with shadow value
    float shadow1 = ShadowCalculation(shadowMap1, fs_in.FragPosLightSpace1);
	float shadow2 = ShadowCalculation(shadowMap2, fs_in.FragPosLightSpace2);
	float final_shadow = max(shadow1, shadow2);
	vec3 lighting = mix(t0, t1, (1.0-final_shadow * 0.5));
	FragColor = vec4(lighting, 1.0f);
}