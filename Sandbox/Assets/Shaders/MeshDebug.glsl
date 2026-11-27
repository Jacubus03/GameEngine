#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
	v_Normal = mat3(u_Transform) * a_Normal;
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 410 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec2 v_TexCoord;

// 0 = normals, 1 = uv
uniform int u_Mode;

void main()
{
	if (u_Mode == 0)
	{
		color = vec4(normalize(v_Normal) * 0.5 + 0.5, 1.0);
	}
	else
	{
		float checker = mod(floor(v_TexCoord.x * 8.0) + floor(v_TexCoord.y * 8.0), 2.0);
		color = vec4(vec3(v_TexCoord, 0.0) * (0.6 + 0.4 * checker), 1.0);
	}
}
