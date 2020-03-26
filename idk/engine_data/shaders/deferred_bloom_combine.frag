#version 460

layout (input_attachment_index=1, set=2, binding=0) uniform subpassInput color_input;
layout (input_attachment_index=2, set=2, binding=1) uniform subpassInput depth_input;
layout (input_attachment_index=3, set=2, binding=2) uniform subpassInput gView_pos;

S_LAYOUT(3,1) uniform sampler2D brightness_input;

S_LAYOUT(7,0) uniform sampler2D ColCorrectLut[1];

S_LAYOUT(4,0) uniform BLOCK(PostProcessingBlock)
{
	vec3 fogColor;
	float FogDensity;

	//Bloom
	float blurStrength;
	float blurScale;
	
	int useFog;
	int useBloom;
}ppb;


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

void main()
{
	float depth = subpassLoad(depth_input).r;
	vec3 frag_color = subpassLoad(color_input).rgb;
	
	//if(depth == 1)
		//discard;
	
	vec4 view_pos = subpassLoad(gView_pos);
	
	vec2 uv =vs_out.uv;
	uv.x = 1-uv.x;
	vec3 brightness = texture(brightness_input,uv).rgb;

	//hard set ratio on bloom because of unwanted shading effects when casted on wall
	
	if(ppb.useBloom == 1)
		frag_color += brightness * 0.15f; 
	
	
	if(ppb.useFog == 1)
	{
		float dist = 0;
		float fogFactor = 0;
		
		//range based
		dist = length(view_pos);
		 
		//Exponential fog
		float d = dist * ppb.FogDensity;
		fogFactor = 1.0 /exp( d );
		fogFactor = clamp( fogFactor, 0.0, 1.0 );
		
		frag_color = mix(ppb.fogColor,frag_color,fogFactor);
	}

	out_color = vec4(ReinhardOperator(frag_color),1);
	
	out_color = clamp(out_color,0,1); //Cannot afford to have it go outside of its LUT
	
	vec3 og = pow(out_color.rgb,vec3(1/2.2));
	vec3 p = sizeSpace(og);
	vec3 p0 =floor(p);
	vec3 p1 =ceil(p);
	ivec3 ip0 = ivec3(p0);
	ivec3 ip1 = ivec3(p1);
	
	vec3 t = (p - p0);// /(p1-p0);
	out_color.rgb = trilinearSample(ip0, ivec3(1,0,0),ivec3(0,1,0),ivec3(0,0,1), t);	
	out_color.rgb = pow(out_color.rgb,vec3(2.2));
	
	//gl_FragDepth = depth; //write this for late depth test, let the gpu discard this if it's smaller
}