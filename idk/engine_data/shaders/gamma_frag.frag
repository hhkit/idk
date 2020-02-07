
layout(input_attachment_index = 0 ,set=2, binding=0) uniform subpassInput color_input;

U_LAYOUT(set = 3,binding =0) uniform float linear_to_gamma;

out vec4 out_color;

void main()
{
	vec4 color = subpassLoad(color_input);
	out_color = vec4(pow(color.rgb,linear_to_gamma),color.a);
}