#version 460

//layout (input_attachment_index=1, set=2, binding=0) uniform subpassInput color_input;
//layout (input_attachment_index=2, set=2, binding=1) uniform subpassInput brightness_input;

S_LAYOUT(3,1) uniform sampler2D brightness_input;

S_LAYOUT(2,1) uniform BLOCK(blurBlock)
{
	int blurdirection;
} bBlock;

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

S_LAYOUT(5,0) uniform BLOCK(ViewportBlock)
{
	vec2 pos;
	vec2 extent;
}vb;


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

float weight[5] = float[](
	0.227027f,
	0.1945946f,
	0.1216216f,
	0.054054f,
	0.016216f
);

void main()
{
	
	//vec3 frag_color = subpassLoad(color_input).rgb;
	
	
	float blurStrength = ppb.blurStrength;
	float blurScale = ppb.blurScale;
	
	vec2 tex_offset = 1.0 / textureSize(brightness_input, 0) * blurScale; // gets size of single texel
	vec2 uv =vb.pos + vs_out.uv*vb.extent;
	
	vec2 hac = tex_offset * 4;
	vec2 end = vb.pos + vb.extent - hac;
	uv = clamp(uv, vb.pos + hac, end);	
	
	vec3 brightness = texture(brightness_input,uv).rgb * weight[0];
	
	
	if (bBlock.blurdirection == 1)
	{
		for(int i = 1; i < 5; ++i)
		{
			// H
			vec2 ha = vec2(tex_offset.x * i, 0.0);
			float wb = weight[i] * blurStrength;
			brightness += texture(brightness_input, uv + ha).rgb * wb;
			brightness += texture(brightness_input, uv - ha).rgb * wb;
		}
	}
	else
	{
		for(int i = 1; i < 5; ++i)
		{
			// V
			vec2 ha = vec2(0.0, tex_offset.y * i);
			float wb = weight[i] * blurStrength;
			brightness += texture(brightness_input, uv + ha).rgb * wb;
			brightness += texture(brightness_input, uv - ha).rgb * wb;
		}
	}

	out_color = vec4(brightness,1);
	
	//if(out_color.rgb == vec3(0))
		//discard;
}