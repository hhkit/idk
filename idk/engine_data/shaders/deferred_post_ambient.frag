#version 450

#define MAX_LIGHTS 8

import /engine_data/shaders/deferred_utils.glsl
U_LAYOUT(3,1) uniform BLOCK(PBRBlock)
{
	mat4 inverse_view_transform;
} PerCamera;

//S_LAYOUT(3, 1) uniform samplerCube test;

S_LAYOUT(6,1) uniform samplerCube irradiance_probe;
S_LAYOUT(6,2) uniform samplerCube environment_probe;
S_LAYOUT(6,3) uniform sampler2D   brdfLUT;  

layout(location=2) in VS_OUT
{
  vec2 uv;
} fs_in;

// lighting 
// temporary light
import /engine_data/shaders/light.glsl

U_LAYOUT(5, 0) uniform BLOCK(LightBlock)
{
	int   light_count;
	Light lights[MAX_LIGHTS];
} LightBlk;


// lighting functions 

vec3 Normal()
{
	return normalize(Load(gNormal).rgb );
}
vec3 Tangent()
{
	return normalize(Load(gTangent).rgb);
}
#define FRAG_NORMAL Normal()
#define FRAG_TANGENT Tangent()
import /engine_data/shaders/utils.glsl
import /engine_data/shaders/pbr_utils.glsl


// forward shading only cares about color!
layout(location = 0)out vec4 out_color;


void main()
{
	// declare initial values here
	LoadGBuffers(view_pos, normal, tangent, metallic, roughness, albedo,ambient_o, uv, emissive);
	
	vec3 view_dir = -normalize(view_pos);

	vec3 light_accum = vec3(0);

	normal = normalize(normal);
	
import /engine_data/shaders/pbr_end.glsl
	out_color = vec4(ambient+emissive,1);
}