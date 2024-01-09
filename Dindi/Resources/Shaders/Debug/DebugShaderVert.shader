#version 420 core

layout(location = 0) in vec3 a_Coord;
uniform mat4 u_Transform;

//We will be not doing any lighting calculation but I will keep this here so we can use other useful constant buffer data.
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
	mat4 c_View;
	vec4 c_CameraPos;
	vec4 c_DirLightPos;
	PointLight c_Lights[DND_MAX_LIGHTS];
};

void main()
{                                
	gl_Position = c_ViewProjection * (u_Transform * vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f));
}