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

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 2) in mat4 model;

layout(location = 2) out vec4 fragColor;

//layout(location = 1) out vec4 color;
void main() {
    gl_Position = ubo.proj * ubo.view * model * vec4(inPosition, 0.0, 1.0);
	fragColor = vec4(inColor,1.0);
}