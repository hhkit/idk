#version 450

#define MAX_LIGHTS 8

const int eAlbedoAmbOcc        = 1       ;
const int eUvMetallicRoughness = 2       ;
const int eViewPos             = 3       ;
const int eNormal              = 4       ;
const int eTangent             = 5       ;
const int eDepth               = 6       ;
const int eGBufferSize         = eDepth+1;

layout(input_attachment_index = eAlbedoAmbOcc       ,set=2, binding=0) uniform subpassInput gAlbAmbOcc;
layout(input_attachment_index = eUvMetallicRoughness,set=2, binding=1) uniform subpassInput gUvMetRough;
layout(input_attachment_index = eViewPos            ,set=2, binding=2) uniform subpassInput gViewPos;
layout(input_attachment_index = eNormal             ,set=2, binding=3) uniform subpassInput gNormal;
layout(input_attachment_index = eTangent            ,set=2, binding=4) uniform subpassInput gTangent;
layout(input_attachment_index = eDepth              ,set=2, binding=5) uniform subpassInput gDepth;

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

S_LAYOUT(7, 4) uniform sampler2D shadow_maps[MAX_LIGHTS];


vec4 Load(subpassInput input_att)
{
	return subpassLoad(input_att);
}


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

import /engine_data/shaders/deferred_utils.glsl

void main()
{

	// declare initial values here
	LoadGBuffers(view_pos, normal, tangent, metallic, roughness, albedo,ambient_o, uv, emissive);
	//if(Load(gDepth).r==1)
	//	discard;
	//vec3  view_pos  = Load(gViewPos).rgb;
	//vec3  normal    = Normal();
	//vec3  tangent   = Tangent();
	//vec4 uv_met_rou = Load(gUvMetRough);
	//vec4 alb_amb_occ= Load(gAlbAmbOcc );
	//vec2  uv        = uv_met_rou.xy;
	//
	//vec3  albedo    = alb_amb_occ.rgb;
	//float metallic  = uv_met_rou.z;
	//float roughness = uv_met_rou.w;
	//float ambient_o = alb_amb_occ.a;
	
	vec3 view_dir = -normalize(view_pos);

	vec3 light_accum = vec3(0);
	normal = normalize(normal);
	
	vec3 reflected = vec3(PerCamera.inverse_view_transform * vec4(reflect(-view_dir, normal),0));
	
	vec4 world_pos = 
		PerCamera.inverse_view_transform *
		vec4(view_pos,1);
	
	for (int i = 0; i < LightBlk.light_count; ++i)
	{
		vec3 result = pbr_metallic(LightBlk.lights[i], view_pos.xyz, normal, reflected, albedo, metallic, roughness, ambient_o); 
		
		if (LightBlk.lights[i].type == 1)
		{
			if(LightBlk.lights[i].cast_shadow!=0)
				result *= vec3(1.f - ShadowCalculation(LightBlk.lights[i],shadow_maps[i],(LightBlk.lights[i].v_dir) ,normal ,LightBlk.lights[i].vp * world_pos));
			//vvvp = LightBlk.lights[i].vp;
		}
		if (LightBlk.lights[i].type == 2)
		{
			if(LightBlk.lights[i].cast_shadow!=0)
				result *= (vec3(1-ShadowCalculation(LightBlk.lights[i],shadow_maps[i],LightBlk.lights[i].v_dir,normal ,LightBlk.lights[i].vp * world_pos)));
		}
		
		light_accum += result;
	}
	vec3 F = mix(vec3(0.04), albedo, metallic);
	vec3 kS = fresnelRoughness(max(dot(normal,view_dir), 0.0), F, roughness);
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
import /engine_data/shaders/pbr_end.glsl
}