#include "collision_rotational.h"

namespace idk::phys
{
#define InFront( a ) \
	((a) < float( 0.0 ))

#define Behind( a ) \
	((a) >= float( 0.0 ))

#define On( a ) \
	((a) < float( 0.005 ) && (a) > -float( 0.005 ))

	bool track_face_axis(int* axis, int n, float s, float* sMax, const vec3& normal, vec3* axisNormal)
	{
		if (s > float(0.0))
			return true;

		if (s > * sMax)
		{
			*sMax = s;
			*axis = n;
			*axisNormal = normal;
		}

		return false;
	}

	bool track_edge_axis(int* axis, int n, float s, float* sMax, const vec3& normal, vec3* axisNormal)
	{
		if (s > float(0.0))
			return true;

		const float l = float(1.0) / normal.length();
		s *= l;

		if (s > * sMax)
		{
			*sMax = s;
			*axis = n;
			*axisNormal = normal * l;
		}

		return false;
	}

	void compute_edges_and_basis(const vec3& eR, const collision_transform& rtx, vec3 n, int axis, unsigned* out, mat3* basis, vec3* e)
	{

		n = rtx.rotation.transpose() * n;

		if (axis >= 3)
			axis -= 3;

		switch (axis)
		{
		case 0:
			if (n.x > 0.0)
			{
				out[0] = 1;
				out[1] = 8;
				out[2] = 7;
				out[3] = 9;

				*e = vec3{ eR.y, eR.z, eR.x };
				(*basis)[0] = rtx.rotation[1], (*basis)[1] = rtx.rotation[2], (*basis)[2] = rtx.rotation[0];
			}

			else
			{
				out[0] = 11;
				out[1] = 3;
				out[2] = 10;
				out[3] = 5;

				*e = vec3{ eR.z, eR.y, eR.x };
				(*basis)[0] = rtx.rotation[2], (*basis)[1] = rtx.rotation[1], (*basis)[2] = rtx.rotation[0];
				// e->Set(eR.z, eR.y, eR.x);
				// basis->SetRows(rtx.rotation.ez, rtx.rotation.ey, -rtx.rotation.ex);
			}
			break;

		case 1:
			if (n.y > 0.0f)
			{
				out[0] = 0;
				out[1] = 1;
				out[2] = 2;
				out[3] = 3;

				*e = vec3{ eR.z, eR.x, eR.y };
				(*basis)[0] = rtx.rotation[2], (*basis)[1] = rtx.rotation[0], (*basis)[2] = rtx.rotation[1];
				// e->Set(eR.z, eR.x, eR.y);
				// basis->SetRows(rtx.rotation.ez, rtx.rotation.ex, rtx.rotation.ey);
			}

			else
			{
				out[0] = 4;
				out[1] = 5;
				out[2] = 6;
				out[3] = 7;

				*e = vec3{ eR.z, eR.x, eR.y };
				(*basis)[0] = rtx.rotation[2], (*basis)[1] = -rtx.rotation[0], (*basis)[2] = -rtx.rotation[1];
				// e->Set(eR.z, eR.x, eR.y);
				// basis->SetRows(rtx.rotation.ez, -rtx.rotation.ex, -rtx.rotation.ey);
			}
			break;

		case 2:
			if (n.z > 0.0f)
			{
				out[0] = 11;
				out[1] = 4;
				out[2] = 8;
				out[3] = 0;

				*e = vec3{ eR.y, eR.x, eR.z };
				(*basis)[0] = -rtx.rotation[1], (*basis)[1] = rtx.rotation[0], (*basis)[2] = rtx.rotation[2];
				// e->Set(eR.y, eR.x, eR.z);
				// basis->SetRows(-rtx.rotation.ey, rtx.rotation.ex, rtx.rotation.ez);
			}

			else
			{
				out[0] = 6;
				out[1] = 10;
				out[2] = 2;
				out[3] = 9;

				*e = vec3{ eR.y, eR.x, eR.z };
				(*basis)[0] = -rtx.rotation[1], (*basis)[1] = -rtx.rotation[0], (*basis)[2] = -rtx.rotation[2];
				// e->Set(eR.y, eR.x, eR.z);
				// basis->SetRows(-rtx.rotation.ey, -rtx.rotation.ex, -rtx.rotation.ez);
			}
			break;
		}
	}

	int orthographic(float sign, float e, int axis, int clipEdge, vec3* in, int inCount, vec3* out)
	{
		int outCount = 0;
		vec3 a = in[inCount - 1];

		for (int i = 0; i < inCount; ++i)
		{
			vec3 b = in[i];

			float da = sign * a[axis] - e;
			float db = sign * b[axis] - e;

			vec3 cv;

			// B
			if (((InFront(da) && InFront(db)) || On(da) || On(db)))
			{
				assert(outCount < 8);
				out[outCount++] = b;
			}

			// I
			else if (InFront(da) && Behind(db))
			{
				// cv.f = b.f;
				// cv.f.outR = clipEdge;
				// cv.f.outI = 0;
				cv = a + (b - a) * (da / (da - db));
				assert(outCount < 8);
				out[outCount++] = cv;
			}

			// I, B
			else if (Behind(da) && InFront(db))
			{
				cv = a + (b - a) * (da / (da - db));
				// cv.f.inR = clipEdge;
				// cv.f.inI = 0;
				assert(outCount < 8);
				out[outCount++] = cv;

				assert(outCount < 8);
				out[outCount++] = b;
			}

			a = b;
		}

		return outCount;
	}
	int clip(const vec3& rPos, const vec3& e, unsigned* clipEdges, const mat3& basis, vec3* incident, vec3* outVerts, float* outDepths)
	{
		int inCount = 4;
		int outCount;
		vec3 in[8];
		vec3 out[8];

		for (int i = 0; i < 4; ++i)
			in[i] = basis * (incident[i] - rPos);

		outCount = orthographic(float(1.0), e.x, 0, clipEdges[0], in, inCount, out);

		if (!outCount)
			return 0;

		inCount = orthographic(float(1.0), e.y, 1, clipEdges[1], out, outCount, in);

		if (!inCount)
			return 0;

		outCount = orthographic(float(-1.0), e.x, 0, clipEdges[2], in, inCount, out);

		if (!outCount)
			return 0;

		inCount = orthographic(float(-1.0), e.y, 1, clipEdges[3], out, outCount, in);

		// Keep incident vertices behind the reference face
		outCount = 0;
		for (int i = 0; i < inCount; ++i)
		{
			float d = in[i].z - e.z;

			if (d <= float(0.0))
			{
				outVerts[outCount] = (basis * in[i]) + rPos;
				// outVerts[outCount].f = in[i].f;
				outDepths[outCount++] = d;
			}
		}

		assert(outCount <= 8);

		return outCount;
	}
}
