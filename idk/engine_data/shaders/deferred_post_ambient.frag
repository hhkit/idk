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


vec4 Load(subpassInput input_att)
{
	return subpassLoad(input_att);
}


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
	if(Load(gDepth).r==1)
		discard;
	vec3  view_pos  = Load(gViewPos).rgb;
	vec3  normal    = Normal();
	vec3  tangent   = Tangent();
	vec4 uv_met_rou = Load(gUvMetRough);
	vec4 alb_amb_occ= Load(gAlbAmbOcc );
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