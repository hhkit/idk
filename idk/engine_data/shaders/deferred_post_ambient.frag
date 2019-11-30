#version 450

#define MAX_LIGHTS 8

const int eAlbedoAmbOcc        = 0       ;
const int eUvMetallicRoughness = 1       ;
const int eViewPos             = 2       ;
const int eNormal              = 3       ;
const int eTangent             = 4       ;
const int eDepth               = 5       ;
const int eGBufferSize         = eDepth+1;

S_LAYOUT(2, 0) uniform sampler2D gbuffers[eGBufferSize];

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
	return normalize(texture(gbuffers[eNormal] ,fs_in.uv).rgb );
}
vec3 Tangent()
{
	return normalize(texture(gbuffers[eTangent] ,fs_in.uv).rgb);
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
	if(texture(gbuffers[eDepth],fs_in.uv).r==1)
		discard;
	vec3  view_pos  = texture(gbuffers[eViewPos],fs_in.uv).rgb;
	vec3  normal    = Normal();
	vec3  tangent   = Tangent();
	vec4 uv_met_rou = texture(gbuffers[eUvMetallicRoughness] ,fs_in.uv);
	vec4 alb_amb_occ= texture(gbuffers[eAlbedoAmbOcc] ,fs_in.uv);
	vec2  uv        = uv_met_rou.xy;
	
	vec3  albedo    = alb_amb_occ.rgb;
	float metallic  = uv_met_rou.z;
	float roughness = uv_met_rou.w;
	float ambient_o = alb_amb_occ.a;
	
	vec3 view_dir = -normalize(view_pos);

	vec3 light_accum = vec3(0);

	normal = normalize(normal);
	
import /engine_data/shaders/pbr_end.glsl
	out_color = vec4(ambient,1);
}