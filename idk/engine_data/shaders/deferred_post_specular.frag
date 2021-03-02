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

// lighting functions 

vec3 Normal()
{
	return normalize(Load(gNormal).rgb );
}
vec3 Tangent()
{
return _gTangent;
	//return normalize(Load(gTangent).rgb);
}
#define FRAG_NORMAL Normal()
#define FRAG_TANGENT Tangent()
import /engine_data/shaders/utils.glsl
import /engine_data/shaders/pbr_utils.glsl


// forward shading only cares about color!
layout(location = 0)out vec4 out_color;

//const vec3 fogColor = vec3(0.5, 0.5,0.5);
//const float FogDensity = 0.005;


void main()
{

	// declare initial values here
	LoadGBuffers(view_pos, normal, tangent, specular, roughness, albedo,ambient_o, uv, emissive);
	
	vec3 view_dir = -normalize(view_pos);

	vec3 light_accum = vec3(0);
	normal = normalize(normal);
	//vec3 dir_light_accum = vec3(0);
	
	vec3 reflected = vec3(PerCamera.inverse_view_transform * vec4(reflect(-view_dir, normal),0));
	
	vec4 world_pos = 
		PerCamera.inverse_view_transform *
		vec4(view_pos,1);
	int j=0;
	float view_z_abs = abs(view_pos.z);
	for (int i = 0; i < LightBlk.light_count; ++i)
	{
		Light curr_light = LightBlk.lights[i];
		vec3 result = pbr_specular(curr_light, view_pos.xyz, normal, reflected, albedo, specular, roughness, ambient_o); 
		
		vec4 cascade_c = vec4(0,0,0,0);
		
		if(curr_light.cast_shadow!=0)
		{
			if (curr_light.type == 1)
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
	
				result *= shadow_factor;
				//shadow_accum -= shadow_factor;				
				++j;
				
				//dir_light_accum = result * shadow_factor;
			}
			else if (curr_light.type == 2)
			{
				
				result *= (vec3(1.f-ShadowCalculation(curr_light,shadow_maps[i],curr_light.v_dir,normal ,curr_light.vp * world_pos)));
				//shadow_accum -= (vec3(1-ShadowCalculation(curr_light,shadow_maps[i],curr_light.v_dir,normal ,curr_light.vp * world_pos)));	
				
			} 
		}
		
		light_accum += result;
	}
	
	
	vec3 F = mix(vec3(0.04f), albedo, specular);
	vec3 kS = fresnelRoughness(max(dot(normal,view_dir), 0.0), F, roughness);
	vec3 kD = 1.0f - kS;
	kD *= 1.0f - specular;
import /engine_data/shaders/pbr_end.glsl
}