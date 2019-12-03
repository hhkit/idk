#define LoadGBuffers(view_pos, normal, tangent, metallic, roughness, albedo,ambient_occ, uv, emissive) \
if(Load(gDepth).r==1)                                                                           \
	discard;                                                                                    \
vec4  view_pos_  = Load(gViewPos);                                                           \
vec4  normal_    = Load(gNormal);                                                            \
vec4  tangent_   = Load(gTangent);                                                           \
vec4 uv_met_rou_ = Load(gUvMetRough);                                                            \
vec4 alb_amb_occ_= Load(gAlbAmbOcc );                                                           \
																								\
vec3 view_pos   = view_pos_.rgb;                                                                \
vec3 normal   = normal_.rgb;                                                                    \
vec3 tangent   = tangent_.rgb;                                                                  \
float metallic   = uv_met_rou_.z;                                                                \
float roughness  = uv_met_rou_.w;                                                                \
vec3  albedo     = alb_amb_occ_.rgb;                                                             \
float ambient_occ= alb_amb_occ_.a;                                                               \
vec2 uv        = uv_met_rou_.xy;                                                               \
vec3  emissive   = vec3(view_pos_.w,normal_.w,tangent_.w);                                       \
																				   