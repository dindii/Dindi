#version 420 core

#define DND_MAX_LIGHTS 5
#define DND_CSM_LEVELS 3

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;
in float v_ViewSpaceFragmentDistance;

uniform sampler2D u_Diffuse;
uniform sampler2D u_Specular;
uniform sampler2D u_Normal;


uniform vec4 u_Color;

in mat3 v_TBN;

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

	gPosition = vec4(v_FragPos.xyz, v_ViewSpaceFragmentDistance);

	//Use default magenta texture in case we don't have a diffuse texture for it. Debug cube meshes for lights also uses this to set the color of the cube with the color of the light (debugging purposes)
	gAlbedoSpec.rgb = u_Color.rgb;
	
	if(textureSize(u_Diffuse, 0).x > 1)
	{
		gAlbedoSpec.rgb = texture(u_Diffuse, v_TexCoord).rgb;
	}
	
	gAlbedoSpec.a = 1.0f;

	if (textureSize(u_Specular, 0).x > 1)
	{
		gAlbedoSpec.a = texture(u_Specular, v_TexCoord).r;
	}

	//In case we have Normal map, transform from tangent space to world and output. Otherwise, use the natural normal of the mesh.
	if (textureSize(u_Normal, 0).x > 1)
	{
		vec3 tempNormal = texture(u_Normal, v_TexCoord).rgb;
		tempNormal = tempNormal * 2.0 - 1.0;
		gNormal = vec4(normalize(v_TBN * tempNormal).xyz, 1.0f);
	}
	else
	{
		gNormal = vec4(normalize(v_Normal).xyz, 1.0f);
	}
}