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

const vec3 fogColor = vec3(0.5, 0.5,0.5);
const float FogDensity = 0.008;


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
	vec3 dir_light_accum = vec3(0);
	
	vec3 reflected = vec3(PerCamera.inverse_view_transform * vec4(reflect(-view_dir, normal),0));
	
	vec4 world_pos = 
		PerCamera.inverse_view_transform *
		vec4(view_pos,1);
	int j=0, k = 0;
	float view_z_abs = abs(view_pos.z);
	
	for (int i = 0; i < LightBlk.light_count; ++i)
	{
		Light curr_light = LightBlk.lights[i];
		vec3 result = pbr_metallic(curr_light, view_pos.xyz, normal, reflected, albedo, metallic, roughness, ambient_o); 
		vec4 cascade_c = vec4(0,0,0,0);
		//if(curr_light.type == 0)
		//{
		//	
		//	if(curr_light.cast_shadow!=0)
		//	{
		//		result *= (vec3(1-ShadowCalculation(curr_light,shadow_map_point[k],curr_light.v_dir,normal , world_pos, curr_light.falloff, curr_light.v_pos)));
		//		//shadow_accum -= (vec3(1-ShadowCalculation(curr_light,shadow_map_point[k],curr_light.v_dir,normal , world_pos, curr_light.falloff, curr_light.v_pos)));
		//	}
		//	
		//	++k;
		//}
		if (curr_light.type == 1)
		{
			if(curr_light.cast_shadow!=0)
			{
				vec3 shadow_factor = vec3(1.f,1.f,1.f);
				if(view_z_abs <= DirectionalBlk.directional_vp[j].far_plane)
				{
					shadow_factor = vec3(1.f - ShadowCalculation(curr_light,shadow_map_directional[j],(curr_light.v_dir) ,normal ,DirectionalBlk.directional_vp[j].vp * world_pos));
		
				}
				else if(view_z_abs <= DirectionalBlk.directional_vp[++j].far_plane)
				{
					shadow_factor = vec3(1.f - ShadowCalculation(curr_light,shadow_map_directional[j],(curr_light.v_dir) ,normal ,DirectionalBlk.directional_vp[j].vp * world_pos));
					
				}
				//else if(view_z_abs <= DirectionalBlk.directional_vp[++j].far_plane)
				//{
				//	shadow_factor = vec3(1.f - ShadowCalculation(curr_light,shadow_map_directional[j],(curr_light.v_dir) ,normal ,DirectionalBlk.directional_vp[j].vp * world_pos));
				//}
				//result *= shadow_factor;
				//shadow_accum -= shadow_factor;				
				++j;
				
				dir_light_accum = result * shadow_factor;
			}
			else
			{
				j+=2;
			}
			//vvvp = curr_light.vp;
		}
		else if (curr_light.type == 2)
		{
			if(curr_light.cast_shadow!=0)
			{
				result *= (vec3(1-ShadowCalculation(curr_light,shadow_maps[i],curr_light.v_dir,normal ,curr_light.vp * world_pos)));
				//shadow_accum -= (vec3(1-ShadowCalculation(curr_light,shadow_maps[i],curr_light.v_dir,normal ,curr_light.vp * world_pos)));
			
			}
		} 
		
		light_accum += result;// + cascade_c.xyz;
	}
	
	//distance
	
	//int remain = MAX_LIGHTS - LightBlk.light_count;
	//vec3 accum_for_fog = light_accum;
	
	
	float dist = 0;
	float fogFactor = 0;
	
	//range based
	dist = view_z_abs;
	 
	//Exponential fog
	float d = dist * FogDensity;
	fogFactor = 1.0 /exp( d *d );
	fogFactor = clamp( fogFactor, 0.0, 1.0 );
	//float be = (10.0 - view_pos.y) * 0.004;//0.004 is just a factor; change it if you want
	//float bi = (10.0 - view_pos.y) * 0.001;//0.001 is just a factor; change it if you want
 
	//float ext = exp(-dist * be);
	//float insc = exp(-dist * bi);
	
	light_accum = mix(fogColor,dir_light_accum,fogFactor) + light_accum;
	
	//light_accum = light_accum* ext + fogColor * (1.0 - insc);
	
	
	vec3 F = mix(vec3(0.04), albedo, metallic);
	vec3 kS = fresnelRoughness(max(dot(normal,view_dir), 0.0), F, roughness);
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
import /engine_data/shaders/pbr_end.glsl
}