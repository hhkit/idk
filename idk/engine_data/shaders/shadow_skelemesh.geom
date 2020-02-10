#version 450

#define SHADOW_CASCADES 2

layout(invocations = 6, triangles) in;
layout(triangle_strip, max_vertices = 12) out;

U_LAYOUT(0,0) uniform BLOCK(CameraBlock)
{
	mat4 perspective_transform[SHADOW_CASCADES];
} PerCamera;


layout (location = 0) out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
	
	for (int i =0 ; i < SHADOW_CASCADES; ++i)
	{
		gl_Layer = i;
		for(int k=0; k < gl_in.length(); ++k)
        {
			gl_Position = PerCamera.perspective_transform[i] * gl_in[k].position;
			
			EmitVertex();
		}
		EndPrimitive();
	}
}