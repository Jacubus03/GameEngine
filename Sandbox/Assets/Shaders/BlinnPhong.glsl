#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_WorldPosition;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
	vec4 worldPosition = u_Transform * vec4(a_Position, 1.0);

	v_WorldPosition = worldPosition.xyz;
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * worldPosition;
}

#type fragment
#version 410 core

layout(location = 0) out vec4 color;

in vec3 v_WorldPosition;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPosition;
uniform vec3 u_Color;

uniform sampler2D u_Texture;
uniform int u_UseTexture;

uniform float u_AmbientStrength;
uniform float u_SpecularStrength;
uniform float u_Shininess;

void main()
{
	vec3 N = normalize(v_Normal);
	vec3 L = normalize(u_LightPosition - v_WorldPosition);
	vec3 V = normalize(u_CameraPosition - v_WorldPosition);
	vec3 H = normalize(L + V); // Blinn-Phong half vector

	float diffuse = max(dot(N, L), 0.0);
	float specular = diffuse > 0.0 ? pow(max(dot(N, H), 0.0), u_Shininess) : 0.0;

	vec3 albedo = u_Color;

	if (u_UseTexture == 1)
		albedo *= texture(u_Texture, v_TexCoord).rgb;

	vec3 result = (u_AmbientStrength + diffuse) * albedo * u_LightColor
		+ u_SpecularStrength * specular * u_LightColor;

	color = vec4(result, 1.0);
}
