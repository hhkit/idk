#version 460

layout (input_attachment_index=1, set=2, binding=0) uniform subpassInput light_accum_input;
layout (input_attachment_index=2, set=2, binding=1) uniform subpassInput depth_input;


layout(location=0) out vec4 out_color;

void main()
{
	float depth = subpassLoad(depth_input).r;
	vec4 light = subpassLoad(light_accum_input).rgba;
	if(depth==1)
		discard;
	out_color = light/(light+1);
}