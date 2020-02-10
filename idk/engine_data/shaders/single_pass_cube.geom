#version 450

layout(invocations = 6, triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout (location = 1)in VS_OUT
{
  vec3 position;
} gs_in[];

layout (location = 3) out VS_OUT
{
	vec3 position;
} gs_out;

U_LAYOUT(0,0) uniform BLOCK(view_projection)
{
	mat4 perspective_mtx;
	mat4 pv_matrices[6];
} Mat4Blk;

layout (location = 0) in gl_PerVertex
{
  vec4 gl_Position;
} gl_in[];

layout (location = 0) out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
	mat4 V_Matrix = Mat4Blk.pv_matrices[gl_InvocationID];
	gl_Layer = gl_InvocationID;
	for (int i =0 ; i < gl_in.length(); ++i)
	{
		gs_out.position = vec3(V_Matrix * vec4(gs_in[i].position, 1));
		gl_Position = Mat4Blk.perspective_mtx * vec4(gs_out.position, 1);
		
		EmitVertex();
	}
	EndPrimitive();
}