#version 450 

layout (location = 0) in vec3 position; // vert pos
layout (location = 1) in vec3 center; // particle center
layout (location = 2) in float rotation;
layout (location = 3) in float size;
layout (location = 4) in vec4 color;

U_LAYOUT(0, 0) uniform BLOCK(CameraBlock)
{
	mat4 perspective_transform;
	mat4 view_transform;
} PerCamera;

layout(location = 2) out VS_OUT
{
	vec4 color;
} vs_out;

layout(location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	vs_out.color    = color;
	
	vec3 cam_right  = vec3(PerCamera.view_transform[0][0], PerCamera.view_transform[1][0], PerCamera.view_transform[2][0]);
	vec3 cam_up     = vec3(PerCamera.view_transform[0][1], PerCamera.view_transform[1][1], PerCamera.view_transform[2][1]);
    gl_Position     = PerCamera.perspective_transform * PerCamera.view_transform * vec4(position, 1);
}