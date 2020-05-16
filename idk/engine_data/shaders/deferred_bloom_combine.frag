#version 460

layout (input_attachment_index=1, set=2, binding=0) uniform subpassInput color_input;
layout (input_attachment_index=2, set=2, binding=1) uniform subpassInput depth_input;
layout (input_attachment_index=3, set=2, binding=2) uniform subpassInput gView_pos;
//layout (input_attachment_index=4, set=2, binding=3) uniform subpassInput bright_input;

//S_LAYOUT(3,1) uniform sampler2D brightness_input;

S_LAYOUT(7,0) uniform sampler2D ColCorrectLut[1];

S_LAYOUT(4,0) uniform BLOCK(PostProcessingBlock)
{
	vec3 threshold;
	float fogDensity;

	//Bloom
	float blurStrength;
	float blurScale;
	
	int useFog;
	int useBloom;
	
	vec4 fogColor;
}ppb;

//S_LAYOUT(5,0) uniform BLOCK(ViewportBlock)
//{
//	vec2 pos;
//	vec2 extent;
//}vb;


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

vec3 sizeSpace(vec3 coords)
{
	return coords*15;
}

vec3 directSample(ivec3 sizeSpace)
{
	ivec2 uv = ivec2(sizeSpace.x + sizeSpace.z*16,sizeSpace.y);
	vec3 result = texelFetch(ColCorrectLut[0],uv,0).rgb;
	return result;//pow(result,vec3(1/1.01));
}
vec3 interp(vec3 c0, vec3 c1, float t)
{
	//t = smoothstep(0,1,t);
	return (1-t)*c0+t*c1;
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

layout(location = 2) in VS_OUT
{
  vec2 uv;	
} vs_out;

//const vec3 fogColor = vec3(0.5, 0.5,0.5);
//const float fogDensity = 1.8; //magic number verified by YY
    
float d0 =7, dmax = 55;   //magic numbers verified by YY
float fog_cap = 0.028125; //magic number verified by YY

void main()
{
	float depth = subpassLoad(depth_input).r;
	if(depth == 1)
		discard;
	vec3 frag_color = subpassLoad(color_input).rgb;
	

	//hard set ratio on bloom because of unwanted shading effects when casted on wall
	
	if(ppb.useFog == 1)
	{
		vec4 view_pos = subpassLoad(gView_pos);
		float dist = 0;
		float fogFactor = 0;
		
		//range based
		dist = (abs(view_pos.z)-d0)/(dmax-d0); //magic number verified by YY
		//Exponential fog                      //magic number verified by YY
		float d = dist * ppb.fogDensity;           //magic number verified by YY
		d= 1 - 1/exp(d);                       //magic number verified by YY
		d = pow(d,4);                          //magic number verified by YY
		fogFactor = d;//1.0 /exp( d );
		fogFactor = clamp( fogFactor, 0.0, fog_cap );
	
		frag_color = mix(frag_color,ppb.fogColor.rgb,fogFactor);
	}
	
	//if(ppb.useBloom == 1)
	//{
	//	vec3 brightness = subpassLoad(bright_input).rgb;
	//	frag_color += brightness * 0.15f; 
	//}
	

	out_color = vec4(ReinhardOperator(frag_color),1);
	
	out_color = clamp(out_color,0,1); //Cannot afford to have it go outside of its LUT
	
	
	//LUT colour correction shifted here
	//vec3 og = pow(out_color.rgb,vec3(1/2.2));
	//vec3 p = sizeSpace(og);
	//vec3 p0 =floor(p);
	//vec3 p1 =ceil(p);
	//ivec3 ip0 = ivec3(p0);
	//ivec3 ip1 = ivec3(p1);
	//
	//vec3 t = (p - p0);// /(p1-p0);
	//out_color.rgb = trilinearSample(ip0, ivec3(1,0,0),ivec3(0,1,0),ivec3(0,0,1), t);	
	//out_color.rgb = pow(out_color.rgb,vec3(2.2));
	
	gl_FragDepth = depth; //write this for late depth test, let the gpu discard this if it's smaller
}