#version 460

layout (input_attachment_index=1, set=2, binding=0) uniform subpassInput color_input;
layout (input_attachment_index=2, set=2, binding=1) uniform subpassInput depth_input;


layout(location=0) out vec4 out_color;
layout(location=3) out vec4 out_hdr;
out float gl_FragDepth;

//(Relative luminance, obtained from wiki)
float Luminance(vec3 color)
{
	return 0.2126*color.r + 0.7152*color.g + 0.0722*color.b;
}

//Tone Mapping
//Formula: Ld(x,y) = L(x,y)/(1+L(x,y)), where Ld(x,y) is the mapped luminance
//since L = 0.2126*color.r + 0.7152*color.g + 0.0722*color.b;
//and Ld(x,y) = L(x,y)/(1+L(x,y))
//Sub L into L(x,y) as the numerator
// Ld(x,y) = (0.2126*color(x,y).r + 0.7152*color(x,y).g + 0.0722*color(x,y).b)/(1 + L(x,y));
//by distributivity: Distribute denominator into the sums
// Ld(x,y) = (0.2126*color(x,y).r/(1 + L(x,y)) + 0.7152*color(x,y).g/(1 + L(x,y)) + 0.0722*color(x,y).b/(1 + L(x,y)));
//we can now collapse color_prime(x,y).component = color(x,y).component/(1+L(x,y))
//this results in:
// Ld(x,y) = (0.2126*color_prime(x,y).r + 0.7152*color_prime(x,y).g + 0.0722*color_prime(x,y).b);
//which means, that the new color we're looking for to achieve the desired luminance is color_prime.
// color_prime(x,y) = color(x,y) / (L(x,y)+1);

vec3 ReinhardOperator(vec3 color)
{
	float L = Luminance(color.rgb); 
	return color/(L+1);
}

void main()
{
	float depth = subpassLoad(depth_input).r;
	vec3  light = subpassLoad(color_input).rgb;
	
	out_color = vec4(ReinhardOperator(light),1);
	
	float brightness = dot(light, vec3(0.45,0.68,0.68));
	 if(brightness > 1.0)
        out_hdr = vec4(light, 1.0);
    else
        out_hdr = vec4(0.0, 0.0, 0.0, 1.0);
		
	gl_FragDepth = depth; //write this for late depth test, let the gpu discard this if it's smaller
	
}