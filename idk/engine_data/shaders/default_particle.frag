#version 450

S_LAYOUT(2, 5) uniform sampler2D _uTex[1];

layout(location = 2) in VS_OUT
{
	vec2 uv;
    vec4 color;	
} fs_in;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = texture(_uTex[0], fs_in.uv) * fs_in.color;
}