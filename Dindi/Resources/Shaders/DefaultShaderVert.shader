#version 420 core

layout(location = 0) in vec3 a_Coord;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_Transform;

//#TEMPORARY #TODO: Turn this into an array of matrices since we will be using this 
//as a component of the cascaded shadow map
uniform mat4 u_SingleLightTransform;

#define DND_MAX_LIGHTS 1000

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
	mat4 c_ShadowTransform;
	vec4 c_CameraPos;
	vec4 c_DirLightPos;
	PointLight c_Lights[DND_MAX_LIGHTS];
};


out vec2 v_TexCoord;

//#TODO: Multiply this by the model matrix, please.
out vec3 v_FragPos;
out vec3 v_Normal;
out vec4 v_FragPosLightSpace;

void main()
{
	gl_Position = c_ViewProjection * (u_Transform * vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f));
	v_TexCoord = a_TexCoord;
	v_FragPos = vec3(u_Transform * vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f)).xyz;
	v_Normal = mat3(u_Transform) * a_Normal;

	v_FragPosLightSpace = (u_SingleLightTransform * vec4(v_FragPos.x, v_FragPos.y, v_FragPos.z, 1.0f));
}