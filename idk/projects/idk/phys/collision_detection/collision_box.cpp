#include "stdafx.h"
#include "collision_box.h"
#include <ds/span.inl>
#include <phys/collision_contact.h>
#include <phys/collision_rotational.h>
namespace idk::phys
{
// #pragma optimize("", off)
	col_result generate_contacts(const box& lhs, const box& rhs)
	{
		// Manifold m;
		collision_transform atx;
		collision_transform btx;
		vec3 eA = lhs.extents * 0.5f;
		vec3 eB = rhs.extents * 0.5f;
		atx.position = lhs.center;
		atx.rotation = quat_cast<mat3>(lhs.rotation);
		btx.position = rhs.center;
		btx.rotation = quat_cast<mat3>(rhs.rotation);

		// B's frame in A's space
		const mat3 C = atx.rotation.transpose() * btx.rotation;

		mat3 absC;
		bool parallel = false;
		const float kCosTol = float(1.0e-6);
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				float val = abs(C[i][j]);
				absC[i][j] = val;

				if (val + kCosTol >= float(1.0))
					parallel = true;
			}
		}

		// Vector from center A to center B in A's space
		vec3 t = atx.rotation.transpose() * (btx.position - atx.position);

		// Query states
		float s;
		float aMax = -FLT_MAX;
		float bMax = -FLT_MAX;
		float eMax = -FLT_MAX;
		int aAxis = ~0;
		int bAxis = ~0;
		int eAxis = ~0;
		vec3 nA;
		vec3 nB;
		vec3 nE;

		// Face axis checks
		const auto column = [](const mat3& m, size_t num) -> vec3
		{
			return vec3{ m[0][num], m[1][num] ,m[2][num] };
		};

		// a's x axis
		s = abs(t.x) - (eA.x + eB.dot(column(absC, 0)));
		if (track_face_axis(&aAxis, 0, s, &aMax, atx.rotation[0], &nA))
			return col_failure{};

		// a's y axis
		s = abs(t.y) - (eA.y + eB.dot(column(absC, 1)));
		if (track_face_axis(&aAxis, 1, s, &aMax, atx.rotation[1], &nA))
			return col_failure{};

		// a's z axis
		s = abs(t.z) - (eA.z + eB.dot(column(absC, 2)));
		if (track_face_axis(&aAxis, 2, s, &aMax, atx.rotation[2], &nA))
			return col_failure{};

		// b's x axis
		s = abs(t.dot(C[0])) - (eB.x + absC[0].dot(eA));
		if (track_face_axis(&bAxis, 3, s, &bMax, btx.rotation[0], &nB))
			return col_failure{};

		// b's y axis
		s = abs(t.dot(C[1])) - (eB.y + absC[1].dot(eA));
		if (track_face_axis(&bAxis, 4, s, &bMax, btx.rotation[1], &nB))
			return col_failure{};

		// b's z axis
		s = abs(t.dot(C[2])) - (eB.z + absC[2].dot(eA));
		if (track_face_axis(&bAxis, 5, s, &bMax, btx.rotation[2], &nB))
			return col_failure{};

		if (!parallel)
		{
			// Edge axis checks
			float rA;
			float rB;

			// Cross( a.x, b.x )
			rA = eA.y * absC[0][2] + eA.z * absC[0][1];
			rB = eB.y * absC[2][0] + eB.z * absC[1][0];
			s = abs(t.z * C[0][1] - t.y * C[0][2]) - (rA + rB);
			if (track_edge_axis(&eAxis, 6, s, &eMax, vec3(float(0.0), -C[0][2], C[0][1]), &nE))
				return col_failure{};

			// Cross( a.x, b.y )
			rA = eA.y * absC[1][2] + eA.z * absC[1][1];
			rB = eB.x * absC[2][0] + eB.z * absC[0][0];
			s = abs(t.z * C[1][1] - t.y * C[1][2]) - (rA + rB);
			if (track_edge_axis(&eAxis, 7, s, &eMax, vec3(float(0.0), -C[1][2], C[1][1]), &nE))
				return col_failure{};

			// Cross( a.x, b.z )
			rA = eA.y * absC[2][2] + eA.z * absC[2][1];
			rB = eB.x * absC[1][0] + eB.y * absC[0][0];
			s = abs(t.z * C[2][1] - t.y * C[2][2]) - (rA + rB);
			if (track_edge_axis(&eAxis, 8, s, &eMax, vec3(float(0.0), -C[2][2], C[2][1]), &nE))
				return col_failure{};

			// Cross( a.y, b.x )
			rA = eA.x * absC[0][2] + eA.z * absC[0][0];
			rB = eB.y * absC[2][1] + eB.z * absC[1][1];
			s = abs(t.x * C[0][2] - t.z * C[0][0]) - (rA + rB);
			if (track_edge_axis(&eAxis, 9, s, &eMax, vec3(C[0][2], float(0.0), -C[0][0]), &nE))
				return col_failure{};

			// Cross( a.y, b.y )
			rA = eA.x * absC[1][2] + eA.z * absC[1][0];
			rB = eB.x * absC[2][1] + eB.z * absC[0][1];
			s = abs(t.x * C[1][2] - t.z * C[1][0]) - (rA + rB);
			if (track_edge_axis(&eAxis, 10, s, &eMax, vec3(C[1][2], float(0.0), -C[1][0]), &nE))
				return col_failure{};

			// Cross( a.y, b.z )
			rA = eA.x * absC[2][2] + eA.z * absC[2][0];
			rB = eB.x * absC[1][1] + eB.y * absC[0][1];
			s = abs(t.x * C[2][2] - t.z * C[2][0]) - (rA + rB);
			if (track_edge_axis(&eAxis, 11, s, &eMax, vec3(C[2][2], float(0.0), -C[2][0]), &nE))
				return col_failure{};

			// Cross( a.z, b.x )
			rA = eA.x * absC[0][1] + eA.y * absC[0][0];
			rB = eB.y * absC[2][2] + eB.z * absC[1][2];
			s = abs(t.y * C[0][0] - t.x * C[0][1]) - (rA + rB);
			if (track_edge_axis(&eAxis, 12, s, &eMax, vec3(-C[0][1], C[0][0], float(0.0)), &nE))
				return col_failure{};

			// Cross( a.z, b.y )
			rA = eA.x * absC[1][1] + eA.y * absC[1][0];
			rB = eB.x * absC[2][2] + eB.z * absC[0][2];
			s = abs(t.y * C[1][0] - t.x * C[1][1]) - (rA + rB);
			if (track_edge_axis(&eAxis, 13, s, &eMax, vec3(-C[1][1], C[1][0], float(0.0)), &nE))
				return col_failure{};

			// Cross( a.z, b.z )
			rA = eA.x * absC[2][1] + eA.y * absC[2][0];
			rB = eB.x * absC[1][2] + eB.y * absC[0][2];
			s = abs(t.y * C[2][0] - t.x * C[2][1]) - (rA + rB);
			if (track_edge_axis(&eAxis, 14, s, &eMax, vec3(-C[2][1], C[2][0], float(0.0)), &nE))
				return col_failure{};
		}

		// Artificial axis bias to improve frame coherence
		const float kRelTol = float(0.95);
		const float kAbsTol = float(0.01);
		int axis;
		float sMax;
		vec3 n;
		float faceMax = max(aMax, bMax);
		if (kRelTol * eMax > faceMax + kAbsTol)
		{
			axis = eAxis;
			sMax = eMax;
			n = nE;
		}

		else
		{
			if (kRelTol * bMax > aMax + kAbsTol)
			{
				axis = bAxis;
				sMax = bMax;
				n = nB;
			}

			else
			{
				axis = aAxis;
				sMax = aMax;
				n = nA;
			}
		}

		if (n.dot(btx.position - atx.position) < float(0.0))
			n = -n;

		assert(axis != ~0);

		col_success res;
		if (axis < 6)
		{
			collision_transform rtx;
			collision_transform itx;
			vec3 eR;
			vec3 eI;
			bool flip;

			if (axis < 3)
			{
				rtx = atx;
				itx = btx;
				eR = eA;
				eI = eB;
				flip = false;
			}

			else
			{
				rtx = btx;
				itx = atx;
				eR = eB;
				eI = eA;
				flip = true;
				n = -n;
			}

			// Compute reference and incident edge information necessary for clipping
			vec3 incident[4];
			compute_incident_face(itx, eI, n, incident);
			unsigned clipEdges[4];
			mat3 basis;
			vec3 e;
			compute_edges_and_basis(eR, rtx, n, axis, clipEdges, &basis, &e);

			// Clip the incident face against the reference face side planes
			vec3 out[8];
			float depths[8];
			int outNum;
			outNum = clip(rtx.position, e, clipEdges, basis, incident, out, depths);

			
			res.centerA = lhs.center;
			res.centerB = rhs.center;
			if (outNum)
			{
				res.contactCount = outNum;
				res.normal = flip ? -n : n;

				for (int i = 0; i < outNum; ++i)
				{
					ContactPoint* c = res.contacts + i;

					// q3FeaturePair pair = out[i].f;

					// if (flip)
					// {
					// 	std::swap(pair.inI, pair.inR);
					// 	std::swap(pair.outI, pair.outR);
					// }

					// c->fp = out[i].f;
					c->position = out[i];
					res.contact_centroid += c->position;

					c->penetration = depths[i];
					res.max_penetration = max(res.max_penetration, depths[i]);
					
				}
				res.contact_centroid /= s_cast<float>( outNum );
			}
		}

		else
		{
			n = atx.rotation * n;

			if (n.dot(btx.position - atx.position) < 0.0f)
				n = -n;

			vec3 PA, QA;
			vec3 PB, QB;
			support_edge(atx, eA, n, &PA, &QA);
			support_edge(btx, eB, -n, &PB, &QB);

			vec3 CA, CB;
			edges_contact(&CA, &CB, PA, QA, PB, QB);

			res.normal = n;
			res.contactCount = 1;

			ContactPoint* c = res.contacts;
			// q3FeaturePair pair;
			// pair.key = axis;
			// c->fp = pair;
			c->penetration = sMax;
			res.max_penetration = sMax;
			c->position = (CA + CB) * float(0.5);
			res.contact_centroid = c->position;
		}

		return res;
	}

	
	col_result collide_box_box_discrete(const box& lhs, const box& rhs)
	{
		// auto disp = rhs.center - lhs.center;
		// 
		// // test on lhs
		// struct res { real min, max; };
		// constexpr auto find_minmax = [](const vec3& n, const auto& pts) -> res
		// {
		// 	real lmin = std::numeric_limits<float>::max();
		// 	real lmax = std::numeric_limits<float>::lowest();
		// 	for (auto& pt : pts)
		// 	{
		// 		auto dot = pt.dot(n);
		// 		lmin = std::min(lmin, dot);
		// 		lmax = std::max(lmax, dot);
		// 	}
		// 	return { lmin, lmax };
		// };
		// 
		// vec3 normal_collision;
		// auto max_pen = std::numeric_limits<float>::max();
		// 
		// const auto lpoints = lhs.points();
		// const auto rpoints = rhs.points();
		// 
		// const auto collide = [&](span<const vec3> axes) -> opt<col_failure>
		// {
		// 	for (auto& n : axes)
		// 	{
		// 		if (n.length_sq() < epsilon)
		// 			continue;
		// 
		// 		auto [lmin, lmax] = find_minmax(n, lpoints);
		// 		auto [rmin, rmax] = find_minmax(n, rpoints);
		// 
		// 		if (lmin > rmax || rmin > lmax)
		// 			return col_failure{ {}, n };
		// 		
		// 		auto new_penetration = std::min(rmax - lmin, lmax - rmin);
		// 		if (new_penetration < max_pen)
		// 		{
		// 			max_pen = new_penetration;
		// 			normal_collision = n;
		// 		}
		// 	}
		// 	return std::nullopt;
		// };
		// 
		// const auto l_axes = lhs.axes();
		// const auto r_axes = rhs.axes();
		// if (auto res = collide(span<const vec3>{l_axes.begin(), l_axes.end()})) // if fail
		// 	return *res;
		// 
		// if (auto res = collide(span<const vec3>{r_axes.begin(), r_axes.end()})) // if fail
		// 	return *res;
		// 
		// array<vec3, 9> cross_norms
		// {
		// 	l_axes[0].cross(r_axes[0]).normalize(),
		// 	l_axes[0].cross(r_axes[1]).normalize(),
		// 	l_axes[0].cross(r_axes[2]).normalize(),
		// 	l_axes[1].cross(r_axes[0]).normalize(),
		// 	l_axes[1].cross(r_axes[1]).normalize(),
		// 	l_axes[1].cross(r_axes[2]).normalize(),
		// 	l_axes[2].cross(r_axes[0]).normalize(),
		// 	l_axes[2].cross(r_axes[1]).normalize(),
		// 	l_axes[2].cross(r_axes[2]).normalize(),
		// };
		// 
		// if (auto res = collide(span<const vec3>{cross_norms}))
		// 	return *res;
		// else
		// {
		// 	col_success result;
		// 	result.centerA = lhs.center;
		// 	result.centerB = rhs.center;
		// 	result.normal_of_collision = (rhs.center - lhs.center).dot(normal_collision) > 0 ? -normal_collision : normal_collision;
		// 	result.penetration_depth = max_pen;
		// 	result.point_of_collision; // todo
		// 	result.manifold = generate_contacts(lhs, rhs);
		// 	compute_basis(result.manifold.normal, result.manifold.tangentVectors, result.manifold.tangentVectors + 1);
		// 	return result;
		// }

		auto res = generate_contacts(lhs, rhs);
		if (res)
		{
			auto& v = res.value();
			
			compute_basis(v.normal, v.tangentVectors, v.tangentVectors + 1);
		}

		return res;
	}
}