#version 450

layout(location = 1) in VS_OUT
{
  vec3 position;
  vec2 uv;
} fs_in;

layout(location = 0) out vec4 FragColor;

S_LAYOUT(3, 0) uniform sampler2D tex;
U_LAYOUT(4, 1) uniform BLOCK(UIBlock)
{
	vec4 color;
	bool is_font;
} PerUI;

void main()
{
	vec4 s = texture(tex, fs_in.uv);
	vec4 huh = PerUI.is_font? vec4(1):vec4(0);
	FragColor = mix(s * PerUI.color, vec4(1, 1, 1, s.r) * PerUI.color, huh);
	
	if(FragColor.a < 0.001176)
		discard;
} 