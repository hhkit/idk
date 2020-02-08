#version 450

layout(input_attachment_index = 0 ,set=2, binding=0) uniform subpassInput color_input;

U_LAYOUT(3,0) uniform BLOCK(linear_to_gamma)
{
	float Linear_to_gamma;
};

layout(location=0) out vec4 out_color;

void main()
{
	vec4 color = subpassLoad(color_input);
	vec3 conv_col =pow(color.rgb,vec3(Linear_to_gamma));
	//vec3(
	//	pow(color.r,Linear_to_gamma),
	//	pow(color.g,Linear_to_gamma),
	//	pow(color.b,Linear_to_gamma)
	//	);
	out_color = vec4(conv_col,color.a);
}