#version 450

layout(location = 1) in VS_OUT
{
  vec3 position;
  vec2 uv;
  vec4 color;
} fs_in;

layout(location = 0) out vec4 FragColor;

S_LAYOUT(3, 0) uniform sampler2D tex;

void main()
{
	FragColor = texture(tex, fs_in.uv) * fs_in.color;
	
	if(FragColor.a < 0.001176)
		discard;
} 