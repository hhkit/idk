#version 450

#define MAX_LIGHTS 8
#define MAX_MAP  8

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

S_LAYOUT(7, 4) uniform sampler2D shadow_maps[MAX_LIGHTS];

S_LAYOUT(9, 1) uniform sampler2D shadow_map_directional[MAX_MAP];

S_LAYOUT(13, 1) uniform samplerCube shadow_map_point[MAX_LIGHTS];

S_LAYOUT(11, 1) uniform BLOCK(DirectionalBlock)
{
	DLight directional_vp[MAX_MAP];
}DirectionalBlk;

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
	int j=0;
	float view_z_abs = abs(view_pos.z);
	for (int i = 0; i < LightBlk.light_count; ++i)
	{
		Light curr_light = LightBlk.lights[i];
		vec3 result = pbr_metallic(curr_light, view_pos.xyz, normal, reflected, albedo, metallic, roughness, ambient_o); 
		vec4 cascade_c = vec4(0,0,0,0);
		if (curr_light.type == 1)
		{
			if(curr_light.cast_shadow!=0)
			{
				//result *= vec3(1.f - ShadowCalculation(curr_light,shadow_maps[i],(curr_light.v_dir) ,normal ,curr_light.vp * world_pos));			
				
				vec3 shadow_factor = vec3(1.f,1.f,1.f);
				if(view_z_abs <= DirectionalBlk.directional_vp[j].far_plane)
				{
					shadow_factor = vec3(1.f - ShadowCalculation(curr_light,shadow_map_directional[j],(curr_light.v_dir) ,normal ,DirectionalBlk.directional_vp[j].vp * world_pos));
					//shadow_factor *= shadow_factor;
					//cascade_c = vec4(0.1,0,0,0);
				}
				else if(view_z_abs <= DirectionalBlk.directional_vp[++j].far_plane)
				{
					shadow_factor = vec3(1.f - ShadowCalculation(curr_light,shadow_map_directional[j],(curr_light.v_dir) ,normal ,DirectionalBlk.directional_vp[j].vp * world_pos));
					//shadow_factor *= shadow_factor;
					//cascade_c = vec4(0,0.1,0,0);
				}
				//else if(view_z_abs <= DirectionalBlk.directional_vp[++j].far_plane)
				//{
				//	shadow_factor = vec3(1.f - ShadowCalculation(curr_light,shadow_map_directional[j],(curr_light.v_dir) ,normal ,DirectionalBlk.directional_vp[j].vp * world_pos));
				//	//shadow_factor *= shadow_factor;
				//	//cascade_c = vec4(0,0,0.1,0);
				//}
				result *= shadow_factor;
				j = 0;
				
			}
			//vvvp = curr_light.vp;
		}
		else if (curr_light.type == 2)
		{
			if(curr_light.cast_shadow!=0)
				result *= (vec3(1-ShadowCalculation(curr_light,shadow_maps[i],curr_light.v_dir,normal ,curr_light.vp * world_pos)));
		}
		
		light_accum += result;// + cascade_c.xyz;
	}
	vec3 F = mix(vec3(0.04), albedo, metallic);
	vec3 kS = fresnelRoughness(max(dot(normal,view_dir), 0.0), F, roughness);
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
import /engine_data/shaders/pbr_end.glsl
}