#version 430

in vec3 v_vertex;

uniform mat4 u_model;
uniform mat4 u_lightSpaceMatrix;

void main()
{
	gl_Position = u_lightSpaceMatrix * u_model * vec4(v_vertex, 1.0);
}