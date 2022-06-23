#version 420 core

in vec2 v_TexCoord;
out vec4 outColor;

//#TEMPORARY Until we have PBR (=
uniform sampler2D u_Diffuse;
uniform sampler2D u_Specular;
uniform sampler2D u_Normal;

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
	PointLight c_Lights[DND_MAX_LIGHTS];
};

void main()
{
	//outColor = vec4(sin(v_Time), 0.0f, cos(v_Time), 1.0f);
	outColor = texture(u_Diffuse, v_TexCoord) * vec4(sin(c_Time), 0.5f, cos(c_Time), 1.0f);
	//outColor = texture(u_Diffuse, v_TexCoord) * c_Lights[0].m_Color;
}