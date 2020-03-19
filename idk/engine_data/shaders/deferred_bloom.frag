#version 460

layout (input_attachment_index=1, set=2, binding=0) uniform subpassInput color_input;
layout (input_attachment_index=2, set=2, binding=1) uniform subpassInput brightness_input;


layout(location=0) out vec4 out_color;

float Luminance(vec3 color)
{
	return 0.2126*color.r + 0.7152*color.g + 0.0722*color.b;
}

vec3 ReinhardOperator(vec3 color)
{
	float L = Luminance(color.rgb); 
	return color/(L+1);
}

void main()
{
	vec3 frag_color = subpassLoad(color_input).rgb;
	vec3 brightness = subpassLoad(brightness_input).rgb;
	
	
	
	
	vec3 color = ReinhardOperator(frag_color); 
	out_color = vec4(color,1);
}