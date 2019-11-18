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
struct Light
{
    int   type; // 0 for point, 1 for dir, 2 for spot
	vec3  color;
	vec3  v_pos;
	vec3  v_dir;
	
	float cos_inner;
	float cos_outer;
	
	float falloff;
	
	float shadow_bias;
	
	float intensity;
	
	int cast_shadow;
	
	mat4 vp;
};

U_LAYOUT(5, 0) uniform BLOCK(LightBlock)
{
	int   light_count;
	Light lights[MAX_LIGHTS];
} LightBlk;

S_LAYOUT(7, 4) uniform sampler2D shadow_maps[MAX_LIGHTS];

// lighting functions 

const float PI = 3.1415926535;

vec3 Normal()
{
	return normalize(texture(gbuffers[eNormal] ,fs_in.uv).rgb );
}
vec3 Tangent()
{
	return normalize(texture(gbuffers[eTangent] ,fs_in.uv).rgb);
}



vec3 fresnel(float cos_theta, vec3 f0)
{
	return f0 + (vec3(1.0) - f0) * pow(1.0 - cos_theta, 5.0);
}

vec3 fresnelRoughness(float cos_theta, vec3 f0, float roughness)
{
	return f0 + (vec3(1.0 - roughness) - f0) * pow(1.0 - cos_theta, 5.0);
}

float DistributionGGX(vec3 normal, vec3 half_vec, float roughness)
{
	float alpha  = roughness * roughness;
	float alpha2 = alpha * alpha;
	float NdotH  = max(dot(normal, half_vec), 0.0);
	float NdotH2 = NdotH * NdotH;
	
	float numer  = alpha2;
	float denom  = (NdotH2 * (alpha2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return numer / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = r *r / 8.0;
	
	float numer = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return numer / denom;
}

float GeometrySmith(vec3 normal, vec3 view, vec3 light, float roughness)
{
	float NdotV = max(dot(normal, view), 0.0);
	float NdotL = max(dot(normal, light), 0.0);
	
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}

//vec2 sampleOffsetDirections[8] = vec2[]
//(
//   vec3( 1,  1), vec3( 1, -1), vec3(-1, -1), vec3(-1,  1), 
//   vec3( 1,  0), vec3(-1,  0), vec3( 0,  1), vec3( 0, -1)
//);   

float computePCF(int iter, vec2 tc, vec2 texelSize, float tc_z, float bias, float curDepth, int x)
{
	//float z_depth = tc_z/tc.w;
	
	vec4 sampleShadow = texture(shadow_maps[iter],  tc.xy + vec2(x,-1) * texelSize);
	vec4 sampleShadow1 = texture(shadow_maps[iter], tc.xy + vec2(x,0) * texelSize );
	vec4 sampleShadow2 = texture(shadow_maps[iter], tc.xy + vec2(x,1) * texelSize );
	
	float avgDepth = 0.f, biasedCDepth = curDepth - bias;
	if(biasedCDepth > sampleShadow.r) 
		avgDepth = 1.f;
	
	if(biasedCDepth > sampleShadow1.r) 
		avgDepth += 1.f;
	
	if(biasedCDepth > sampleShadow2.r) 
		avgDepth += 1.f;
		
	return avgDepth;
} 

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float computeStratifiedPoisson(int iter, vec2 tc, vec2 texelSize, float tc_z, float bias, int x)
{
	//int index = x;
	//float z_depth = tc_z/tc.w;
	
	int index = int(16.f*rand(tc.xy*x))%16;
	vec2 poissonTC = poissonDisk[index]/700.f;
	vec4 sampleShadow = texture(shadow_maps[iter],tc.xy + vec2(x,-1) * texelSize + poissonTC);
	vec4 sampleShadow1 = texture(shadow_maps[iter],tc.xy + vec2(x,0) * texelSize + poissonTC);
	vec4 sampleShadow2 = texture(shadow_maps[iter],tc.xy + vec2(x,1) * texelSize + poissonTC);
	
	//0.2f*(1.f - sampleShadow) - 0.2f*(1.f - sampleShadow1) - 0.2f*(1.f - sampleShadow2)
	//return 0.2f*(-1.f + (-sampleShadow.r + sampleShadow1.r + sampleShadow2.r));
	
	return 0.2f*(3.f-sampleShadow.r - sampleShadow1.r - sampleShadow2.r);
}
 
 
vec3 ShadowCoords(vec4 fPosInLS)
{
	//Transformation of proj coord to NDC[0,1]
	 // perform perspective divide
	 fPosInLS.xyz=fPosInLS.xyz/fPosInLS.w;
    vec3 projCoords = fPosInLS.xyz *0.5f + 0.5f;
	//projCoords =  ;
	
	return projCoords.xyz ;
}
float ShadowCalculation(int iter, vec3 lightDir , vec3 normal,vec4 fPosInLS)
{
	vec3 projCoords = ShadowCoords(fPosInLS);
	
	if(projCoords.x>0.f && projCoords.y > 0.f)
	{					
		{//Other
		
			//Oversampling check
			if(projCoords.z > 1.0f)
				return 0.f;
				
			float curDepth = projCoords.z;
				
			//Bias calculation
			float bias = max(0.005 * (1.0 - dot(normal,lightDir)),0.009);
			bias = LightBlk.lights[iter].shadow_bias + bias;
			
			//PCF
			float avgDepth = 0.f;
			float	tDepth=0.f;
			vec2 texelSize = textureSize(shadow_maps[iter],0);
			texelSize = 1.f/texelSize;
			float tc_z = projCoords.z - bias;
			
			for(int x = -2; x <= 2; ++x)
			{		
				avgDepth += computePCF(iter,projCoords.xy, texelSize,tc_z,bias,curDepth,x);
				avgDepth -= computeStratifiedPoisson(iter,projCoords.xy,texelSize, tc_z, bias,x);
			}

			
			//divide by 9 values
			avgDepth /= 15.0f;
			
			return avgDepth;
		}
	}else{
		return 0.0f;
	}
	//return 0;
	//return 0;
}

vec3 pbr(
	int i
,	vec3  view_pos
,	vec3  normal
,   vec3  reflected
,	vec3  albedo
,	float metallic
,	float roughness
,	float ambient_o
)
{
// temporary light code
	Light light = LightBlk.lights[i];
	
	vec3  frag_to_light = (vec4(light.v_pos,1)).xyz - view_pos;
	vec3  light_dir =	frag_to_light;

	
	
	float dist      =  length(frag_to_light); 
	
	if (light.type != 0) light_dir = -light.v_dir;
	if (light.type != 1) light_dir /= dist;

	
	vec3  view_dir  = -normalize(view_pos); // camera is at 0
	vec3  half_vec  =  normalize(view_dir + light_dir);
	
	float atten = 1;
	
	float spotlight_effect =1;
	
	if(light.type==2)
	{
	
		float cos_alpha= dot(normalize((frag_to_light)),normalize(light_dir));
	    float cos_phi  = light.cos_outer;
	    float cos_theta  = light.cos_inner;
		spotlight_effect = min(pow(((cos_alpha - cos_phi)/(cos_theta - cos_phi)),0.5),1);
        spotlight_effect = (acos(cos_alpha)>acos(cos_phi))?0:spotlight_effect;
	}
	if (light.type != 1) atten = (1/light.falloff)/(dist*dist);
	
	atten = min(max(atten, 1),10);
	
	vec3 radiance = light.color.rgb * atten * spotlight_effect;
	
	const vec3 f0 = vec3(0.04);
	vec3 F = fresnel(min(max(dot(half_vec, view_dir), 0.0),1), mix(f0, albedo, metallic));
	
	float ndf = DistributionGGX(normal, half_vec, roughness);
	float G   = GeometrySmith(normal, view_dir, light_dir, roughness);
	
	vec3  numer = ndf * G * F;
	float denom = 4.0 * max(dot(normal, view_dir), 0.0) * max(dot(normal, light_dir), 0.0);
	vec3 specular = numer / max(denom, 0.001);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	
	float NdotL = max(dot(normal, light_dir), 0.0);
	
	kD *= 1.0 - metallic;
	
	//return vec3(cos_alpha*0.5 +0.5);
	return (kD * albedo / PI + specular) * radiance * NdotL;
}


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
	
	vec3 reflected = vec3(PerCamera.inverse_view_transform * vec4(reflect(-view_dir, normal),0));
	
	vec4 world_pos = 
		PerCamera.inverse_view_transform *
		vec4(view_pos,1);
	
	for (int i = 0; i < LightBlk.light_count; ++i)
	{
		vec3 result = pbr(i, view_pos.xyz, normal, reflected, albedo, metallic, roughness, ambient_o); 
		
		if (LightBlk.lights[i].type == 1)
		{
			if(LightBlk.lights[i].cast_shadow!=0)
				result *= vec3(1.f - ShadowCalculation(i,(LightBlk.lights[i].v_dir) ,normal ,LightBlk.lights[i].vp * world_pos));
			//vvvp = LightBlk.lights[i].vp;
		}
		if (LightBlk.lights[i].type == 2)
		{
			if(LightBlk.lights[i].cast_shadow!=0)
				result *= (vec3(1-ShadowCalculation(i,LightBlk.lights[i].v_dir,normal ,LightBlk.lights[i].vp * world_pos)));
		}
		
		light_accum += result;
	}
	vec3 F = mix(vec3(0.04), albedo, metallic);
	vec3 kS = fresnelRoughness(max(dot(normal,view_dir), 0.0), F, roughness);
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
	
	
	vec3 irradiance = texture(irradiance_probe, normal).rgb;
	vec3 diffuse = irradiance * albedo;
	
	const float MAX_REFLECTION_LOD = 4.0;
	
	//vec3 prefilteredColor = textureLod(environment_probe, reflected, roughness * MAX_REFLECTION_LOD).rgb;
	//vec2 envBRDF = texture(brdfLUT, vec2(max(dot(normal, view_dir), 0.0), roughness)).rg;
	//vec3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);
	//vec3 ambient = (kD * diffuse + specular) * vec3(0.01);
	vec3 ambient = vec3(0.03) * albedo;
	
	vec3 color = light_accum + ambient;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2)); 
	out_color = vec4(color,1);	
}