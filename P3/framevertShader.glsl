#version 330 core
//out vec4 FragColor;

//in vec2 TexCoords;

//uniform sampler2D screenTexture;


layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

void main()
{
 //   vec3 col = texture(screenTexture, TexCoords).rgb;
 //   FragColor = vec4(col, 1.0);
	  TexCoords = aTexCoords;
	  gl_position = vec4(aPos.x, aPos.y, 0.0, 1.0);

} 