#version 460

layout (input_attachment_index=1, set=2, binding=0) uniform subpassInput color_input;
layout (input_attachment_index=2, set=2, binding=1) uniform subpassInput depth_input;


layout(location=0) out vec4 out_color;
//layout(location=3) out vec4 out_hdr;
out float gl_FragDepth;

S_LAYOUT(7,0) uniform sampler2D ColCorrectLut[1];

//(Relative luminance, obtained from wiki)
float Luminance(vec3 color)
{
	return 0.2126f*color.r + 0.7152f*color.g + 0.0722f*color.b;
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
	return color/(L+1.f);
}
vec3 sizeSpace(vec3 coords)
{
	return coords*15.f;
}

vec3 directSample(ivec3 sizeSpace)
{
	ivec2 uv = ivec2(sizeSpace.x + sizeSpace.z*16.f,sizeSpace.y);
	vec3 result = texelFetch(ColCorrectLut[0],uv,0).rgb;
	return result;//pow(result,vec3(1/1.01));
}
vec3 interp(vec3 c0, vec3 c1, float t)
{
	//t = smoothstep(0,1,t);
	return (1.f-t)*c0+t*c1;
}
vec3 linearSample(ivec3 p0, ivec3 p1, float t)
{
	vec3 c0 =directSample(p0);
	vec3 c1 = directSample(p1);
	return interp(c0,c1,t);
}
vec3 bilinearSample(ivec3 p, ivec3 d0, ivec3 d1, vec2 uv)
{
	vec3 c0=linearSample(p,p+d0,uv.x);
	vec3 c1=linearSample(p+d1,p+d1+d0,uv.x);
	return interp(c0,c1,uv.y);
}
vec3 trilinearSample(ivec3 p, ivec3 d0, ivec3 d1, ivec3 d2, vec3 uvw)
{
	vec3 c0=bilinearSample(p,d0,d2,uvw.xz);
	vec3 c1=bilinearSample(p+d1,d0,d2,uvw.xz);
	return interp(c0,c1,uvw.y);
}


float ungamma = 1.f/2.2f;


void main()
{
	float depth = subpassLoad(depth_input).r;
	
	if(depth == 1.f)
		discard;
		
	vec3  light = subpassLoad(color_input).rgb;
	
	//float brightness = dot(light, vec3(0.9f,0.9f,0.9f));
	//if(brightness > 1.0)
    //    out_hdr = vec4(light, 1.0);
    //else
    //    out_hdr = vec4(0.0, 0.0, 0.0, 1.0);
	
	out_color = vec4(ReinhardOperator(light),1.f);
		
	out_color = clamp(out_color,0,1); //Cannot afford to have it go outside of its LUT
	
	vec3 og = pow(out_color.rgb,vec3(ungamma));
	vec3 p = sizeSpace(og);
	vec3 p0 =floor(p);
	vec3 p1 =ceil(p);
	ivec3 ip0 = ivec3(p0);
	ivec3 ip1 = ivec3(p1);
	
	vec3 t = (p - p0);// /(p1-p0);
	out_color.rgb = trilinearSample(ip0, ivec3(1,0,0),ivec3(0,1,0),ivec3(0,0,1), t);
	
	//out_color.rgb = abs(out_color.rgb-og);
	//float err = 0.000000059604645;
	//if(out_color.r<=err)
	//	out_color.r=1;
	//if(out_color.g<=err)
	//	out_color.g=1;
	//if(out_color.b<=err)
	//	out_color.b=1;
	out_color.rgb = pow(out_color.rgb,vec3(2.2f));
	gl_FragDepth = depth; //write this for late depth test, let the gpu discard this if it's smaller
	
}