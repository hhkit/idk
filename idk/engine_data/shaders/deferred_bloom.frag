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
	vec3 fogColor;
	float FogDensity;

	//Bloom
	float blurStrength;
	float blurScale;
	
	int useFog;
	int useBloom;
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

void main()
{
	float blurStrength = ppb.blurStrength;
	float blurScale = ppb.blurScale;
	
	//vec3 frag_color = subpassLoad(color_input).rgb;
	
	float weight[5];
	weight[0] = 0.227027;
	weight[1] = 0.1945946;
	weight[2] = 0.1216216;
	weight[3] = 0.054054;
	weight[4] = 0.016216;

	vec2 tex_offset = 1.0 / textureSize(brightness_input, 0) * blurScale; // gets size of single texel
	vec2 uv =vb.pos + vs_out.uv*vb.extent;
	//uv.x = 1-uv.x;
	vec3 brightness = texture(brightness_input,uv).rgb * weight[0];
	
	
	if (bBlock.blurdirection == 1)
	{
		for(int i = 1; i < 5; ++i)
		{
			// H
			brightness += texture(brightness_input, uv + vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * blurStrength;
			brightness += texture(brightness_input, uv - vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * blurStrength;
		}
	}
	else
	{
		for(int i = 1; i < 5; ++i)
		{
			// V
			brightness += texture(brightness_input, uv + vec2(0.0, tex_offset.y * i)).rgb * weight[i] * blurStrength;
			brightness += texture(brightness_input, uv - vec2(0.0, tex_offset.y * i)).rgb * weight[i] * blurStrength;
		}
	}

	out_color = vec4(brightness,1);//vec4(brightness,1);
}