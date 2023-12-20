#version 420 core

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

out vec4 outColor;
in vec4 v_FragPosLightSpace;

//#TEMPORARY Until we have PBR (=
uniform sampler2D u_Diffuse;
uniform sampler2D u_Specular;
uniform sampler2D u_Normal; //#TODO: Expand to properly accept normal textures other than only attribute normals (usually we will only get those as textures).
uniform sampler2D u_ShadowMap;

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


float gamma = 2.2;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	projCoords = projCoords * 0.5f + 0.5f;

	float currentDepth = projCoords.z;
	float closestDepth = texture(u_ShadowMap, projCoords.xy).r;

	//float bias = max(0.0f * (1.0 - dot(v_Normal, vec3(fragPosLightSpace.xyz))), 0.005);
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	
	return shadow;
}

struct PackLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

PackLight CalculateLight(vec3 argLightColor, vec3 argLightPos, bool isDirLighting)
{
	vec3 color = texture(u_Diffuse, v_TexCoord).rgb * argLightColor;
	vec3 specularColor = texture(u_Specular, v_TexCoord).rgb * argLightColor;

	//ambient
	vec3 ambient = 0.05f * color;

	//diffuse calc
	vec3 lightPos = argLightPos;

	vec3 lightDir = normalize(lightPos - v_FragPos) * float(!isDirLighting);
	lightDir += normalize(lightPos) * float(isDirLighting);

	vec3 normal = normalize(v_Normal);

	float diffuseRate = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diffuseRate * color;

	//specular calc
	vec3 cameraPos = c_CameraPos.xyz;
	vec3 viewDir = normalize(cameraPos - v_FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float specularRate = 0.0f;
	specularRate = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);

	vec3 specular = vec3(0.3f) * specularRate;

	return PackLight(ambient, diffuse, specular);
}

void main()
{
	vec3 temporaryResult = vec3(0.0f, 0.0f, 0.0f);

	//I will not waste too much time setting uniforms for those tweaks or even setting properties in the light class because we are not going to
	//use blinn-phong for much time. So, that's why it is extremely hardcoded.
	for (unsigned int x = 0; x < numLights; x++)
	{
		PackLight temp = CalculateLight(c_Lights[x].m_Color.rgb, c_Lights[x].m_Position.rgb, false);

		float distance = length(c_Lights[x].m_Position.xyz - v_FragPos);
		//float attenuation = 1.0f / (distance * distance);
		float attenuation = 2.0f / (distance * (distance / 2));

		vec3 ambientWithAttenuation = temp.ambient * attenuation;
		vec3 diffuseWithAttenuation = temp.diffuse * attenuation;
		vec3 specularWithAttenuation = temp.specular * attenuation;

		temporaryResult += vec3(ambientWithAttenuation + diffuseWithAttenuation) + vec3(temp.specular * specularWithAttenuation);
	}

	//#TODO: colocar a cor do dir light no cbuffer
	PackLight dirLight = CalculateLight(vec3(1.0f, 1.0f, 1.0f), c_DirLightPos.xyz, true);
	float shadow = ShadowCalculation(v_FragPosLightSpace);
	//vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	

	temporaryResult += vec3(dirLight.ambient + (1.0 - shadow) * (dirLight.diffuse + dirLight.specular));
	outColor = vec4(temporaryResult.xyz, 1.0f);
	
	outColor.rgb = pow(outColor.rgb, vec3(1.0f / gamma));
}