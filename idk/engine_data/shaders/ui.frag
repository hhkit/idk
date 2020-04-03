#version 450

layout(location = 1) in VS_OUT
{
  vec3 position;
  vec2 uv;
  vec4 color;
} fs_in;

layout(location = 0) out vec4 out_color;

S_LAYOUT(2, 5) uniform sampler2D _uTex[1];

void main()
{
	out_color = fs_in.color * texture(_uTex[0], fs_in.uv);
	
	if(out_color.a < 0.001176)
		discard;
} 