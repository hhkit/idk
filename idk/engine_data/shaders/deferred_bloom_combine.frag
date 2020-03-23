#version 460

layout (input_attachment_index=1, set=2, binding=0) uniform subpassInput color_input;
//layout (input_attachment_index=2, set=2, binding=1) uniform subpassInput brightness_input;

S_LAYOUT(3,1) uniform sampler2D brightness_input;


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

layout(location = 2) in VS_OUT
{
  vec2 uv;	
} vs_out;

void main()
{

	vec3 frag_color = subpassLoad(color_input).rgb;
	
	vec2 uv =vs_out.uv;
	uv.x = 1-uv.x;
	vec3 brightness = texture(brightness_input,uv).rgb;
	
	//float b = dot(brightness, vec3(0.45,0.70,0.70));
	 //if(b > 1.0)
    frag_color += brightness;
		
	//vec3 color = ReinhardOperator(brightness); 
	//out_color = vec4(frag_color,1);//vec4(brightness,1);
	
	out_color = vec4(ReinhardOperator(frag_color),1);
}