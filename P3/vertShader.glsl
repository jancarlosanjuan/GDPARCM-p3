#version 430

in vec3 v_vertex;
in vec3 v_normal;
in vec3 v_color;
in vec2 v_tex;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_lightSpaceMatrix;

out vec3 f_color;
out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;
out vec4 FragPosLightSpace;

void main(void) {

	gl_Position = u_projection * u_view * u_model * vec4(v_vertex, 1.0f);
	f_color = v_color;
	normal = transpose(inverse(mat3(u_model))) * v_normal;
	fragPos = vec3(u_model * vec4(v_vertex, 1.0f));
	FragPosLightSpace = u_lightSpaceMatrix * vec4(fragPos, 1.0);
	texCoord = vec2(v_tex.x, v_tex.y);
}