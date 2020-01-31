#version 460


// declare output buffers here
layout(location = 0) out vec4 out;

layout(input_attachment_index = 0       ,set=2, binding=0) uniform subpassInput gScene;
layout(input_attachment_index = 1       ,set=2, binding=1) uniform subpassInput gClear;
layout(input_attachment_index = 1       ,set=2, binding=1) uniform subpassInput gDepth;

void main()
{
	float depth = subpassLoad(gDepth).r;
	vec4 scene = subpassLoad(gScene);
	vec4 clear = subpassLoad(gClear);
	out = (depth<1)?scene:clear;
}
