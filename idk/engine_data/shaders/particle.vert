#version 450 

layout (location = 0) in vec3 position; // vert pos
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 center; // particle center
layout (location = 3) in float rotation;
layout (location = 4) in float size;
layout (location = 5) in vec4 color;

U_LAYOUT(0, 0) uniform BLOCK(CameraBlock)
{
	mat4 perspective_transform;
	mat4 view_transform;
} PerCamera;

layout(location = 2) out VS_OUT
{
	vec2 uv;
	vec4 color;
} vs_out;

layout(location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	vs_out.uv       = uv;
	vs_out.color    = color;
	
	const vec3 cam_right  = vec3(PerCamera.view_transform[0][0], PerCamera.view_transform[1][0], PerCamera.view_transform[2][0]);
	const vec3 cam_up     = vec3(PerCamera.view_transform[0][1], PerCamera.view_transform[1][1], PerCamera.view_transform[2][1]);
	const float s = sin(rotation);
	const float c = cos(rotation);
	const mat3 rot = mat3(c, s, 0, -s, c, 0, 0, 0, 1);
    gl_Position     = PerCamera.perspective_transform * PerCamera.view_transform *
                          vec4(center + mat3(cam_right, cam_up, 0, 0, 1) * rot * position * size * 0.5, 1.0);
}