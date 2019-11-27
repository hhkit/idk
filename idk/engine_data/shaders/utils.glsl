
const float PI = 3.1415926535;

vec3 unpack_normal(vec3 normal_in)
{
	vec3 T = fs_in.tangent;
	vec3 N = fs_in.normal;
	vec3 B = cross(N, T);
	mat3 TBN = mat3(T,B,N);
	normal_in -= vec3(0.5);
	return normalize(TBN * normal_in);
}