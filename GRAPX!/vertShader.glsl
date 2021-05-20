#version 430

in vec3 v_vertex;
in vec3 v_color;
in vec3 a_vertex;
in vec3 v_normal;
in vec2 v_uv;

uniform mat4 u_model, u_view, u_projection;
uniform vec3 v_cameraPos;

out vec3 f_position;
out vec3 f_color;
out vec2 f_uv;
out vec3 f_normal;
out vec3 f_cameraPos;

void main(void) {

	//position
	vec3 position = vec3(u_model * vec4(v_vertex,1.0)); //vec4(u_model * vec4(v_vertex,1.0f)).xyz;
	gl_Position = u_projection * u_view * u_model * vec4( position, 1.0f); //the correct one
	f_position = position;

	//uv
	f_uv = v_uv;

	//NORMALS
	vec3 normal = inverse(transpose(mat3(u_model))) * v_normal;  //inverse(transpose(mat3(u_model))) * v_normal; 
	f_normal = normal;

	//position
	//gl_Position = u_projection * u_view * u_model * vec4( position, 1.0f); //the correct one

	//pass the camera position to fragment shader
	f_cameraPos = v_cameraPos;
	
}





	//f_normal = mat3(u_model) * v_normal; //added the mat(u_model) so that the normal is in world space
	//f_normal = transpose(mat3(u_model)) * v_normal;
	//f_normal = vec4(u_model * vec4(v_normal,1.0f)).xyz;
