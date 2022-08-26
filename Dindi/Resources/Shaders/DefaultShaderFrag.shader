#version 420 core

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

out vec4 outColor;

//#TEMPORARY Until we have PBR (=
uniform sampler2D u_Diffuse;
uniform sampler2D u_Specular;
uniform sampler2D u_Normal; //#TODO: Expand to properly accept normal textures other than only attribute normals (usually we will only get those as textures).

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
	vec4 c_CameraPos;
	PointLight c_Lights[DND_MAX_LIGHTS];
};

void main()
{
	//outColor = vec4(sin(v_Time), 0.0f, cos(v_Time), 1.0f);
	//outColor = texture(u_Diffuse, v_TexCoord) * vec4(sin(c_Time), 0.5f, cos(c_Time), 1.0f);

	float lightConstantAttenuation  = 1.0f;
	float lightLinearAttenuation    = 0.09f;
	float lightQuadraticAttenuation = 0.032f;

	vec3 temporaryResult = vec3(0.0f, 0.0f, 0.0f);

	//I will not waste too much time setting uniforms for those tweaks or even setting properties in the light class because we are not going to
	//use blinn-phong for much time. So, that's why it is extremely hardcoded.
	for (unsigned int x = 0; x < numLights; x++)
	{
		vec3 color = texture(u_Diffuse, v_TexCoord).rgb * c_Lights[x].m_Color.rgb;
		vec3 specularColor = texture(u_Specular, v_TexCoord).rgb * c_Lights[x].m_Color.rgb;

		//ambient
		vec3 ambient = 0.05f * color;
		
		//diffuse calc
		vec3 lightPos = c_Lights[x].m_Position.xyz;
		vec3 lightDir = normalize(lightPos - v_FragPos);
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

		float distance = length(lightPos - v_FragPos);
		float attenuation = 1.0f / (lightConstantAttenuation + lightLinearAttenuation * distance + lightQuadraticAttenuation * (distance * distance));

		//vec3 ambientWithAttenuation  = ambient  * attenuation;
		//vec3 diffuseWithAttenuation  = diffuse  * attenuation;
		//vec3 specularWithAttenuation = specular * attenuation;

		vec3 ambientWithAttenuation  = ambient  * 1;
		vec3 diffuseWithAttenuation  = diffuse  * 1;
		vec3 specularWithAttenuation = specular * 1;


		temporaryResult += vec3(ambientWithAttenuation + diffuseWithAttenuation) + vec3(specularColor * specularWithAttenuation);
	}

	outColor = vec4(temporaryResult.xyz, 1.0f);
}