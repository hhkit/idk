
const int eAlbedoAmbOcc        = 1       ;
const int eUvMetallicRoughness = 2       ;
const int eViewPos             = 3       ;
const int eNormal              = 4       ;
const int eTangent             = 5       ;
const int eEmissive            = 5       ;
const int eDepth               = 6       ;
const int eGBufferSize         = eDepth+1;

layout(input_attachment_index = eAlbedoAmbOcc       ,set=2, binding=0) uniform subpassInput gAlbAmbOcc;
layout(input_attachment_index = eUvMetallicRoughness,set=2, binding=1) uniform subpassInput gUvMetRough;
layout(input_attachment_index = eViewPos            ,set=2, binding=2) uniform subpassInput gViewPos;
layout(input_attachment_index = eNormal             ,set=2, binding=3) uniform subpassInput gNormal;
layout(input_attachment_index = eTangent            ,set=2, binding=4) uniform subpassInput gTangent;
layout(input_attachment_index = eEmissive           ,set=2, binding=5) uniform subpassInput gEmissive;
layout(input_attachment_index = eDepth              ,set=2, binding=6) uniform subpassInput gDepth;


vec4 Load(subpassInput input_att)
{
	return subpassLoad(input_att);
}



#define LoadGBuffers(view_pos, normal, tangent, metallic, roughness, albedo,ambient_occ, uv, emissive) \
if(Load(gDepth).r==1)                                                                           \
	discard;                                                                                    \
vec4  view_pos_  = Load(gViewPos);                                                           \
vec4  normal_    = Load(gNormal);                                                            \
/*vec4  tangent_   = Load(gTangent);*/                                                           \
vec4 uv_met_rou_ = Load(gUvMetRough);                                                            \
vec4 alb_amb_occ_= Load(gAlbAmbOcc );                                                           \
vec4 e_emissive    = Load(gEmissive );                                                            \
vec4 deep_depth  = Load(gDepth );                                                            \
																								\
vec3 view_pos   = view_pos_.rgb;                                                                \
vec3 normal   = normal_.rgb   *2 - 1;                                                                    \
/*vec3 tangent   = tangent_.rgb *2 - 1;*/                                                                  \
float metallic   = uv_met_rou_.z;                                                                \
float roughness  = uv_met_rou_.w;                                                                \
vec3  albedo     = alb_amb_occ_.rgb;                                                             \
float ambient_occ= alb_amb_occ_.a;                                                               \
vec2 uv        = uv_met_rou_.xy;                                                               \
vec3  emissive   = e_emissive.rgb;                                       \
float depth_r = deep_depth.r;                                                               \
																				   