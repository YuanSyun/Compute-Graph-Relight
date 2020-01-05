#version 330 core

in VS_OUT {
	vec4 ProjTexCoord;
} fs_in;

out vec4 FragColor;

uniform sampler2D reusltTex1;
uniform sampler2D reusltTex2;
uniform float time;

void main(){
	vec3 t0 = texture(reusltTex1, fs_in.ProjTexCoord.xy/fs_in.ProjTexCoord.w).xyz;
	vec3 t1 = texture(reusltTex2, fs_in.ProjTexCoord.xy/fs_in.ProjTexCoord.w).xyz;
	FragColor = vec4(mix(t0, t1, time), 1.0);
	//FragColor = vec4(t0, 1.0);
}