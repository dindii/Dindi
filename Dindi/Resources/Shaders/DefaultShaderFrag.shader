#version 420 core

#define DND_MAX_LIGHTS 1000
#define DND_CSM_LEVELS 3

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

out vec4 outColor;
in vec4 v_FragPosLightSpace[DND_CSM_LEVELS];
in vec4 v_FragPosViewSpace;

//#TEMPORARY Until we have PBR (=
uniform sampler2D u_Diffuse;
uniform sampler2D u_Specular;
uniform sampler2D u_Normal; //#TODO: Expand to properly accept normal textures other than only attribute normals (usually we will only get those as textures).
//uniform sampler2D u_ShadowMap[DND_CSM_LEVELS];
uniform sampler2DShadow u_ShadowMap[DND_CSM_LEVELS];

uniform sampler3D u_RandomAngles;

uniform float u_CSMDistances[DND_CSM_LEVELS];

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


float gamma = 2.2;

vec2 poissonDisk[26] = vec2[](
	vec2(0.4660514f,  0.4690292f),
	vec2(0.2732336f,  0.1056789f),
	vec2(0.656471f,  0.7472555f),
	vec2(0.7513531f,  0.323501f),
	vec2(0.02016692f,  0.4794935f),
	vec2(0.2959565f,  0.860905f),
	vec2(0.186644f,  0.7447153f),
	vec2(0.4804817f, 0.4359249f),
	vec2(0.5823364f, 0.7868144f),
	vec2(0.8767469f, 0.01694723f),
	vec2(0.5891053f, 0.1645824f) ,
	vec2(0.007625963f, 0.07485358f),
	vec2(0.3456824f, 0.03119616f),
	vec2(0.3016982f, 0.5634059f) ,
	vec2(0.6157057f, 0.6024066f) ,
	vec2(0.135758f, 0.4018539f) ,
	vec2(0.5412278f,  0.3263172f),
	vec2(0.2509218f,  0.6949489f),
	vec2(0.345273f,  0.8905863f)  ,
	vec2(0.5610418f, 0.8230777f),
	vec2(0.8611436f, 0.3664008f) ,
	vec2(0.6850617f, 0.09410904f),
	vec2(0.8692301f,  0.3879704f),
	vec2(0.8892156f,  0.3585771f) ,
	vec2(0.9974015f, 0.03427662f) ,
	vec2(0.06850791f, 0.9654188f)
	);

float ShadowCalculation(vec4 fragPosLightSpace, int layer)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	projCoords = projCoords * 0.5f + 0.5f;

	if (projCoords.z > 1.0)
		return 0.0;

	float shadow = 0;

	//MAIN IMP
		int foo = 0;
		
		int xf = 3;
		int yf = 3;
		
		vec2 texelSize = 1.0 / textureSize(u_ShadowMap[layer], 0);
		float currentDepth = projCoords.z;
		//vec2 rotation = texture(u_RandomAngles, vec3(fragPosLightSpace.xyz)).rg;
		float divisor = 1.0f;
		
		for (int x = -xf; x <= xf; x++)
		{
		
		for (int y = -yf; y <= yf; y++)
		{
			//vec2 offset = vec2(
			//	rotation.x * (poissonDisk[foo].x / divisor) - rotation.y * (poissonDisk[foo].y / divisor),
			//	rotation.y * (poissonDisk[foo].x / divisor) + rotation.x * (poissonDisk[foo].y / divisor));
		
			vec2 offset = (vec2(poissonDisk[foo]) / 1.0f);
		
			float closestDepth = texture(u_ShadowMap[layer], vec3(projCoords.xyz + ((vec3(x, y, 1.0f) * vec3(offset.xy, 1.0f) * vec3(texelSize.xy, 1.0f))))).r;
			//float closestDepth = texture(u_ShadowMap[layer], vec3(projCoords.xyz + vec3(x, y, 1.0f) * vec3(texelSize.xy, 1.0f))).r;
		
			//MAIN IMP
			//float cosTheta = dot(v_Normal, vec3(fragPosLightSpace.xyz));
			//float bias = 0.001f * tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
			//bias = clamp(bias, 0.00009f, 0.001f);
			//
			//if(layer > 0)
			//	bias = 0.001f;
			//MAIN IMP
			

			float cosTheta = dot(v_Normal, vec3(fragPosLightSpace.xyz));
			float bias = 0.001f * tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
			bias = clamp(bias, 0.00009f, 0.001f);

			if (layer > 0)
				bias = 0.001f;

			//Let's use it as zero at least while we don't get shadow acne.
		//	bias = 0;

			shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
			
		}
			foo++;
		}
		
		return shadow /= (xf + xf + 1) * (xf + xf + 1);
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
	
	int layer = 2;
	
	float closer = u_CSMDistances[0];
	float mid = u_CSMDistances[1];
	
	
	//
	if (abs(v_FragPosViewSpace.z) < closer)
	{
		outColor.rgb *= vec3(1.0f, 0.1f, 0.1f);
		layer = 0;
	}
	else if (abs(v_FragPosViewSpace.z) < mid)
	{
		outColor.rgb *= vec3(0.1f, 1.1f, 0.1f);
		layer = 1;
	}
	
	float shadow = ShadowCalculation(v_FragPosLightSpace[layer], layer);
	//
	//vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	

	temporaryResult += vec3(dirLight.ambient + (1.0 - shadow) * (dirLight.diffuse + dirLight.specular));
	outColor = vec4(temporaryResult.xyz, 1.0f);
	
	outColor.rgb = pow(outColor.rgb, vec3(1.0f / gamma));


	//
	if (abs(v_FragPosViewSpace.z) < closer)
	{
	//	outColor.rgb *= vec3(1.0f, 0.1f, 0.1f);
		layer = 0;
	}
	else if (abs(v_FragPosViewSpace.z) < mid)
	{
	//	outColor.rgb *= vec3(0.1f, 1.1f, 0.1f);
		layer = 1;
	}
}