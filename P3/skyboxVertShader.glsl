#version 430

in vec3 v_vertex;

out vec3 TexCoords;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main()
{
    TexCoords = v_vertex;
    vec4 pos = u_projection * u_view * u_model * vec4(v_vertex, 1.0);
    gl_Position = pos.xyww;
}  