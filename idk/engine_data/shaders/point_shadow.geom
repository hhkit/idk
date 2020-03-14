#version 450

#define SHADOW_TRANSFORMS 6

layout(invocations = 6, triangles) in;
layout(triangle_strip, max_vertices = 18) out;

U_LAYOUT(10,0) uniform BLOCK(CameraBlock1)
{
	float far_plane;
	vec3 light_pos;
	mat4 perspective_transform[SHADOW_TRANSFORMS];
} PerCamera;

layout (location = 0) in gl_PerVertex
{
  vec4 gl_Position;
} gl_in[];

layout (location = 0) out gl_PerVertex
{
  vec4 gl_Position;
};

layout (location = 5) out MA_IN
{
	vec4 fragPos;
} fp_out;

void main()
{
	
	for (int i =0 ; i < SHADOW_TRANSFORMS; ++i)
	{
		gl_Layer = i;
		for(int k=0; k < gl_in.length(); ++k)
        {
			fp_out.fragPos = gl_in[k].gl_Position;
			gl_Position = PerCamera.perspective_transform[i] * fp_out.fragPos;
			//gl_Position = gl_in[k].gl_Position;
			
			EmitVertex();
		}
		EndPrimitive();
	}
}