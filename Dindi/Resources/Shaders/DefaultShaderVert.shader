#version 420 core

#define DND_MAX_LIGHTS 1000
#define DND_CSM_LEVELS 3

layout(location = 0) in vec3 a_Coord;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;

uniform mat4 u_Transform;


uniform mat4 u_SingleLightTransform[DND_CSM_LEVELS];


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


out vec2 v_TexCoord;

//#TODO: Multiply this by the model matrix, please.
out vec3 v_FragPos;
out vec3 v_Normal;
out vec4 v_FragPosLightSpace[DND_CSM_LEVELS];
out vec4 v_FragPosViewSpace; 

mat3 TBN;

out vec3 v_TangentLightDirPos;
out vec3 v_TangentViewPos;
out vec3 v_TangentFragPos;
out mat3 v_TBN;

void main()
{
	gl_Position = c_ViewProjection * (u_Transform * vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f));
	v_TexCoord = a_TexCoord;
	v_FragPos = vec3(u_Transform * vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f)).xyz;
	v_Normal = mat3(u_Transform) * a_Normal;

	v_FragPosViewSpace = c_View * (u_Transform *  vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f));

	vec3 T = normalize(vec3(u_Transform * vec4(a_Tangent, 0.0f)));
	vec3 N = normalize(vec3(u_Transform * vec4(a_Normal, 0.0f)));

	T = normalize(T - dot(T, N) * N);

	vec3 B = cross(N, T);


	TBN = transpose(mat3(T, B, N));
	v_TangentLightDirPos = TBN * vec3(c_DirLightPos.xyz);
	v_TangentViewPos = TBN * vec3(c_CameraPos.xyz);
	v_TangentFragPos = TBN * vec3(u_Transform * vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f));

	v_TBN = transpose(TBN);	
	
	for (int i = 0; i < DND_CSM_LEVELS; ++i)
	{
		v_FragPosLightSpace[i] = (u_SingleLightTransform[i] * vec4(v_FragPos.x, v_FragPos.y, v_FragPos.z, 1.0f));
	}
}