#version 330 core

in float v_Time;
in vec2 v_TexCoord;
out vec4 outColor;

//#TEMPORARY Until we have PBR (=
uniform sampler2D u_Diffuse;
uniform sampler2D u_Specular;
uniform sampler2D u_Normal;

void main()
{
	//outColor = vec4(sin(v_Time), 0.0f, cos(v_Time), 1.0f);
	outColor = texture(u_Diffuse, v_TexCoord);
}