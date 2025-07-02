#version 420 core

#define DND_MAX_LIGHTS 5
#define DND_CSM_LEVELS 3

layout(location = 0) in vec3 a_Coord;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;

uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec3 v_Normal;
out float v_ViewSpaceFragmentDistance;
out mat3 v_TBN;

struct PointLight
{
	vec4 m_Position;
	vec4 m_Color;
};

layout(std140, binding = 1) uniform ConstantData
{
	float c_Time;
	float _p1;
	float _p2;
	unsigned int numLights;
	mat4 c_ViewProjection;
	mat4 c_View;
	vec4 c_CameraPos;
	vec4 c_DirLightPos;
	PointLight c_Lights[DND_MAX_LIGHTS];
};

void main()
{
	gl_Position = c_ViewProjection * (u_Transform * vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f));
	
	v_ViewSpaceFragmentDistance = (c_View * (u_Transform *  vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f))).z;

	v_TexCoord = a_TexCoord;
	v_FragPos = vec3(u_Transform * vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f)).xyz;
	v_Normal = mat3(u_Transform) * a_Normal;


	//------------------------------------------------------------------- Normal TBN calculations
	vec3 T = normalize(vec3(mat3(u_Transform) * a_Tangent));
	vec3 N = normalize(vec3(mat3(u_Transform) * a_Normal));
	T = normalize(T - dot(T, N) * N);
	
	vec3 B = cross(N, T);
	
	if (dot(cross(N, T), B) < 0.0)
	{
		T = T * -1.0;
	}

	mat3 TBN = mat3(T, B, N);
	v_TBN = TBN;
	//------------------------------------------------------------------- Normal TBN calculations
}