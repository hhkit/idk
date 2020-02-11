#include "stdafx.h"
#include "collision_box.h"
#include <ds/span.inl>
#include <phys/collision_contact.h>
#include <phys/collision_rotational.h>
namespace idk::phys
{
	Manifold generate_contacts(const box& lhs, const box& rhs)
	{
		Manifold m;
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
		vec3 t = atx.rotation * (btx.position - atx.position);

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

		// a's x axis
		s = abs(t.x) - (eA.x + absC[0].dot(eB));
		if (track_face_axis(&aAxis, 0, s, &aMax, atx.rotation[0], &nA))
			return;

		// a's y axis
		s = abs(t.y) - (eA.y + q3Dot(absC.Column1(), eB));
		if (track_face_axis(&aAxis, 1, s, &aMax, atx.rotation[1], &nA))
			return;

		// a's z axis
		s = abs(t.z) - (eA.z + q3Dot(absC.Column2(), eB));
		if (track_face_axis(&aAxis, 2, s, &aMax, atx.rotation[2], &nA))
			return;

		// b's x axis
		s = abs(q3Dot(t, C[0])) - (eB.x + q3Dot(absC[0], eA));
		if (track_face_axis(&bAxis, 3, s, &bMax, btx.rotation[0], &nB))
			return;

		// b's y axis
		s = abs(q3Dot(t, C[1])) - (eB.y + q3Dot(absC[1], eA));
		if (track_face_axis(&bAxis, 4, s, &bMax, btx.rotation[1], &nB))
			return;

		// b's z axis
		s = abs(q3Dot(t, C[2])) - (eB.z + q3Dot(absC[2], eA));
		if (track_face_axis(&bAxis, 5, s, &bMax, btx.rotation[2], &nB))
			return;

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
				return;

			// Cross( a.x, b.y )
			rA = eA.y * absC[1][2] + eA.z * absC[1][1];
			rB = eB.x * absC[2][0] + eB.z * absC[0][0];
			s = abs(t.z * C[1][1] - t.y * C[1][2]) - (rA + rB);
			if (track_edge_axis(&eAxis, 7, s, &eMax, vec3(float(0.0), -C[1][2], C[1][1]), &nE))
				return;

			// Cross( a.x, b.z )
			rA = eA.y * absC[2][2] + eA.z * absC[2][1];
			rB = eB.x * absC[1][0] + eB.y * absC[0][0];
			s = abs(t.z * C[2][1] - t.y * C[2][2]) - (rA + rB);
			if (track_edge_axis(&eAxis, 8, s, &eMax, vec3(float(0.0), -C[2][2], C[2][1]), &nE))
				return;

			// Cross( a.y, b.x )
			rA = eA.x * absC[0][2] + eA.z * absC[0][0];
			rB = eB.y * absC[2][1] + eB.z * absC[1][1];
			s = abs(t.x * C[0][2] - t.z * C[0][0]) - (rA + rB);
			if (track_edge_axis(&eAxis, 9, s, &eMax, vec3(C[0][2], float(0.0), -C[0][0]), &nE))
				return;

			// Cross( a.y, b.y )
			rA = eA.x * absC[1][2] + eA.z * absC[1][0];
			rB = eB.x * absC[2][1] + eB.z * absC[0][1];
			s = abs(t.x * C[1][2] - t.z * C[1][0]) - (rA + rB);
			if (track_edge_axis(&eAxis, 10, s, &eMax, vec3(C[1][2], float(0.0), -C[1][0]), &nE))
				return;

			// Cross( a.y, b.z )
			rA = eA.x * absC[2][2] + eA.z * absC[2][0];
			rB = eB.x * absC[1][1] + eB.y * absC[0][1];
			s = abs(t.x * C[2][2] - t.z * C[2][0]) - (rA + rB);
			if (track_edge_axis(&eAxis, 11, s, &eMax, vec3(C[2][2], float(0.0), -C[2][0]), &nE))
				return;

			// Cross( a.z, b.x )
			rA = eA.x * absC[0][1] + eA.y * absC[0][0];
			rB = eB.y * absC[2][2] + eB.z * absC[1][2];
			s = abs(t.y * C[0][0] - t.x * C[0][1]) - (rA + rB);
			if (track_edge_axis(&eAxis, 12, s, &eMax, vec3(-C[0][1], C[0][0], float(0.0)), &nE))
				return;

			// Cross( a.z, b.y )
			rA = eA.x * absC[1][1] + eA.y * absC[1][0];
			rB = eB.x * absC[2][2] + eB.z * absC[0][2];
			s = abs(t.y * C[1][0] - t.x * C[1][1]) - (rA + rB);
			if (track_edge_axis(&eAxis, 13, s, &eMax, vec3(-C[1][1], C[1][0], float(0.0)), &nE))
				return;

			// Cross( a.z, b.z )
			rA = eA.x * absC[2][1] + eA.y * absC[2][0];
			rB = eB.x * absC[1][2] + eB.y * absC[0][2];
			s = abs(t.y * C[2][0] - t.x * C[2][1]) - (rA + rB);
			if (track_edge_axis(&eAxis, 14, s, &eMax, vec3(-C[2][1], C[2][0], float(0.0)), &nE))
				return;
		}

		// Artificial axis bias to improve frame coherence
		const float kRelTol = float(0.95);
		const float kAbsTol = float(0.01);
		int axis;
		float sMax;
		vec3 n;
		float faceMax = q3Max(aMax, bMax);
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

		if (q3Dot(n, btx.position - atx.position) < float(0.0))
			n = -n;

		assert(axis != ~0);

		if (axis < 6)
		{
			q3Transform rtx;
			q3Transform itx;
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
			q3ClipVertex incident[4];
			q3ComputeIncidentFace(itx, eI, n, incident);
			u8 clipEdges[4];
			mat3 basis;
			vec3 e;
			q3ComputeReferenceEdgesAndBasis(eR, rtx, n, axis, clipEdges, &basis, &e);

			// Clip the incident face against the reference face side planes
			q3ClipVertex out[8];
			float depths[8];
			int outNum;
			outNum = q3Clip(rtx.position, e, clipEdges, basis, incident, out, depths);

			if (outNum)
			{
				m->contactCount = outNum;
				m->normal = flip ? -n : n;

				for (int i = 0; i < outNum; ++i)
				{
					q3Contact* c = m->contacts + i;

					q3FeaturePair pair = out[i].f;

					if (flip)
					{
						std::swap(pair.inI, pair.inR);
						std::swap(pair.outI, pair.outR);
					}

					c->fp = out[i].f;
					c->position = out[i].v;
					c->penetration = depths[i];
				}
			}
		}

		else
		{
			n = atx.rotation * n;

			if (q3Dot(n, btx.position - atx.position) < float(0.0))
				n = -n;

			vec3 PA, QA;
			vec3 PB, QB;
			q3SupportEdge(atx, eA, n, &PA, &QA);
			q3SupportEdge(btx, eB, -n, &PB, &QB);

			vec3 CA, CB;
			q3EdgesContact(&CA, &CB, PA, QA, PB, QB);

			m->normal = n;
			m->contactCount = 1;

			q3Contact* c = m->contacts;
			q3FeaturePair pair;
			pair.key = axis;
			c->fp = pair;
			c->penetration = sMax;
			c->position = (CA + CB) * float(0.5);
		}
	}

	col_result collide_box_box_discrete(const box& lhs, const box& rhs)
	{
		auto disp = rhs.center - lhs.center;

		// test on lhs
		struct res { real min, max; };
		constexpr auto find_minmax = [](const vec3& n, const auto& pts) -> res
		{
			real lmin = std::numeric_limits<float>::max();
			real lmax = std::numeric_limits<float>::lowest();
			for (auto& pt : pts)
			{
				auto dot = pt.dot(n);
				lmin = std::min(lmin, dot);
				lmax = std::max(lmax, dot);
			}
			return { lmin, lmax };
		};

		vec3 normal_collision;
		auto max_pen = std::numeric_limits<float>::max();

		const auto lpoints = lhs.points();
		const auto rpoints = rhs.points();

		const auto collide = [&](span<const vec3> axes) -> opt<col_failure>
		{
			for (auto& n : axes)
			{
				if (n.length_sq() < epsilon)
					continue;

				auto [lmin, lmax] = find_minmax(n, lpoints);
				auto [rmin, rmax] = find_minmax(n, rpoints);

				if (lmin > rmax || rmin > lmax)
					return col_failure{ {}, n };
				
				auto new_penetration = std::min(rmax - lmin, lmax - rmin);
				if (new_penetration < max_pen)
				{
					max_pen = new_penetration;
					normal_collision = n;
				}
			}
			return std::nullopt;
		};

		const auto l_axes = lhs.axes();
		const auto r_axes = rhs.axes();
		if (auto res = collide(span<const vec3>{l_axes.begin(), l_axes.end()})) // if fail
			return *res;

		if (auto res = collide(span<const vec3>{r_axes.begin(), r_axes.end()})) // if fail
			return *res;

		array<vec3, 9> cross_norms
		{
			l_axes[0].cross(r_axes[0]).normalize(),
			l_axes[0].cross(r_axes[1]).normalize(),
			l_axes[0].cross(r_axes[2]).normalize(),
			l_axes[1].cross(r_axes[0]).normalize(),
			l_axes[1].cross(r_axes[1]).normalize(),
			l_axes[1].cross(r_axes[2]).normalize(),
			l_axes[2].cross(r_axes[0]).normalize(),
			l_axes[2].cross(r_axes[1]).normalize(),
			l_axes[2].cross(r_axes[2]).normalize(),
		};

		if (auto res = collide(span<const vec3>{cross_norms}))
			return *res;
		else
		{
			col_success result;
			result.normal_of_collision = (rhs.center - lhs.center).dot(normal_collision) > 0 ? -normal_collision : normal_collision;
			result.penetration_depth = max_pen;
			result.point_of_collision; // todo
			return result;
		}
	}
}