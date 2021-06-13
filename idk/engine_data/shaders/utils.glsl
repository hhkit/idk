
const float PI = 3.1415926535;

#ifndef FRAG_TANGENT
#define FRAG_TANGENT fs_in.tangent
#endif
#ifndef FRAG_NORMAL
#define FRAG_NORMAL fs_in.normal
#endif
#ifndef POST
vec3 unpack_normal(vec3 normal_in)
{

	vec3 T = FRAG_TANGENT;
	vec3 N = FRAG_NORMAL;
	N=normalize(N);
	T=normalize(T);
	T = -normalize(T-dot(N,T)*N);
	vec3 B = cross(N, T);
	mat3 TBN = mat3(T,B,N);
	//TBN = transpose(TBN);
	normal_in = normal_in*2.0-1.0;
	normal_in = normalize(normal_in);
	return normalize(TBN * normal_in);
	
	//vec3 T = FRAG_TANGENT;
	//vec3 N = FRAG_NORMAL;
	//vec3 B = cross(N, T);
	//mat3 TBN = mat3(T,B,N);
	//normal_in -= vec3(0.5);
	//return normalize(TBN * normal_in);
}
#endif