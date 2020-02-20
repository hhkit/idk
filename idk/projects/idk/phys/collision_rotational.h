#pragma once
#include <idk.h>
#include <phys/collision_rotation_utils.h>
namespace idk::phys
{
	bool track_face_axis(int* axis, int n, float s, float* sMax, const vec3& normal, vec3* axisNormal);
	bool track_edge_axis(int* axis, int n, float s, float * sMax, const vec3& normal, vec3* axisNormal);
	
	void compute_incident_face(const collision_transform& itx, const vec3& e, vec3 n, vec3* out);
	void compute_edges_and_basis(const vec3& eR, const collision_transform& rtx, vec3 n, int axis, unsigned* out, mat3* basis, vec3* e);
	void support_edge(const collision_transform& tx, const vec3& e, vec3 n, vec3* aOut, vec3* bOut);
	void edges_contact(vec3* CA, vec3* CB, const vec3& PA, const vec3& QA, const vec3& PB, const vec3& QB);
	int orthographic(float sign, float e, int axis, int clipEdge, vec3* in, int inCount, vec3* out);
	int clip(const vec3& rPos, const vec3& e, unsigned* clipEdges, const mat3& basis, vec3* incident, vec3* outVerts, float* outDepths);
	
}