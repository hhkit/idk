#version 450

//vec2 positions[3] = vec2[](
//    vec2(0.0, -0.5),
//    vec2(0.5, 0.5),
//    vec2(-0.5, 0.5)
//);
//
//vec4 colors[3] = vec4[](
//    vec4(1.0f, 0.0f, 0.0f, 1.0f),
//    vec4(0.0f, 1.0f, 0.0f, 1.0f),
//    vec4(0.0f, 0.0f, 1.0f, 1.0f)
//);

//layout(binding = 0) uniform UniformBufferObject {
//    mat4 model;
//    mat4 view;
//    mat4 proj;
//} ubo;

layout(binding = 0, set = 0) uniform View{mat4 in_view;}view;
layout(binding = 1, set = 0) uniform Proj{mat4 in_proj;}proj;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 2) in mat4 model;

layout(location = 2) out vec4 fragColor;
layout(location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 3) out gl_PerVertex
{
    vec4 gl_Position;
};


//layout(location = 1) out vec4 color;
void main() {
//+ 1.0z to put the object back within vulkan's clipping range: 0-1 (as opposed to -1 - 1)
    gl_Position =  proj.in_proj * view.in_view *model* vec4(inPosition, 1.0);
	//gl_Position /= gl_Position.w;
	fragColor = vec4(inColor,1.0);
}