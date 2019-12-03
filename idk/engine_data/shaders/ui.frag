#version 450

layout(location = 1) in VS_OUT
{
  vec3 position;
  vec2 uv;
} fs_in;

layout(location = 0) out vec4 FragColor;

S_LAYOUT(3, 0) uniform sampler2D tex;
U_LAYOUT(6, 0) uniform BLOCK(UIBlock)
{
	vec4 color;
	int is_font;
} PerUI;

void main()
{
	vec4 s = texture(tex, fs_in.uv);
	vec4 m = vec4((PerUI.is_font!=0)?1.0:0.0);
	FragColor = mix(s * PerUI.color, vec4(1, 1, 1, s.r) * PerUI.color, m);
	//FragColor = s * PerUI.color;
	
	if(FragColor.a < 0.001176)
		discard;
} 