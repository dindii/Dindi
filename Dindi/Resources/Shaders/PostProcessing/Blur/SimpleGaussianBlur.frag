#version 420 core

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

in vec2 v_TexCoords;

out float OutputColor;

uniform float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

uniform sampler2D u_SSAOTexture;

uniform float u_IsVertical;
void main()
{
	vec2 texSize = 1.0f / textureSize(u_SSAOTexture, 0);
	
	if(u_IsVertical > 0)
	{
		OutputColor = texture2D(u_SSAOTexture, vec2(v_TexCoords)).r * weight[0];

		for (int i=1; i<3; i++) 
		{	
		    OutputColor += texture2D(u_SSAOTexture, (v_TexCoords + vec2(0.0, offset[i]) * texSize.y)).r * weight[i];
		    OutputColor += texture2D(u_SSAOTexture, (v_TexCoords - vec2(0.0, offset[i])  * texSize.y)).r  * weight[i]; 
		}
	}
	else
	{
		OutputColor = texture2D(u_SSAOTexture, vec2(v_TexCoords)).r * weight[0];
		
		for (int i=1; i<3; i++) 
		{	
		    OutputColor += texture2D(u_SSAOTexture, (v_TexCoords + vec2(offset[i], 0.0) * texSize.x)).r * weight[i];
		    OutputColor += texture2D(u_SSAOTexture, (v_TexCoords - vec2(offset[i], 0.0)  * texSize.x)).r  * weight[i];
		}
	}
}