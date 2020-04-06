#version 450

layout(location = 1) in VS_OUT
{
  vec3 position;
  vec2 uv;
  vec3 normal;
  vec3 tangent;
  vec4 color;
} fs_in;

layout(location = 0) out vec4 out_color;

U_LAYOUT(1, 0) uniform BLOCK(_UB0) { float time; } _ub0;
S_LAYOUT(2, 5) uniform sampler2D _uTex[1];

void main()
{
	out_color = fs_in.color * texture(_uTex[0], fs_in.uv);
	
	if(out_color.a < 0.001176)
		discard;
} 