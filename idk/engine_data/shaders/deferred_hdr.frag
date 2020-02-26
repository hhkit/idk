#version 460

layout (input_attachment_index=1, set=2, binding=0) uniform subpassInput metallic_light_accum_input;
layout (input_attachment_index=2, set=2, binding=1) uniform subpassInput metallic_depth_input;
layout (input_attachment_index=3, set=2, binding=2) uniform subpassInput specular_light_accum_input;
layout (input_attachment_index=4, set=2, binding=3) uniform subpassInput specular_depth_input;


layout(location=0) out vec4 out_color;
out float gl_FragDepth;

void main()
{
	float metallic_depth = subpassLoad(metallic_depth_input).r;
	vec3  metallic_light = subpassLoad(metallic_light_accum_input).rgb;
	float specular_depth = subpassLoad(specular_depth_input).r;
	vec3  specular_light = subpassLoad(specular_light_accum_input).rgb;
	
	float depth =min(metallic_depth,specular_depth);
	vec3 light =(metallic_depth<specular_depth)?metallic_light:specular_light;
	if(depth==1)
		discard;
	vec3 color = light/(light+1);
	out_color = vec4(color,1);
	gl_FragDepth = depth;
}