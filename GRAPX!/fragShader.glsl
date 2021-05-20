#version 430

out vec4 fragColor;

in vec3 f_position;
in vec3 f_color;
in vec2 f_uv;
in vec3 f_normal;
in vec3 f_cameraPos;

uniform sampler2D u_texture; //this is a pixel picker
//uniform vec3 lightPos;  

void main(void)
{ 
	//Light Distance
	vec3 lightPos = vec3(0.f, 5.f, 0.f);

	//Ambient Light
	float ambientStrength = 0.1;
	vec3 ambientLight = vec3(0.1f,0.1f,0.1f) * ambientStrength;


	//Diffuse Light
	vec3 diffuseColor = vec3(1.f, 1.f, 1.f);
	vec3 norm = normalize(f_normal);
	vec3 posToLight = normalize(lightPos - f_position);
	float diffuse = max(dot(norm, posToLight), 0.0);
	vec3 diffuseFinal = diffuseColor * diffuse;

	//specular
	float intensity = 0.3;
	vec3 lightToPositionDirection = normalize(lightPos - f_position);//f_position
	vec3 reflectiveDirection = normalize(reflect(lightToPositionDirection, norm ));
	vec3 posToViewDirection = normalize(f_position - f_cameraPos);
	float specularConstant = pow(max(dot(posToViewDirection, reflectiveDirection), 0), 1);//30
	vec3 specularFinal = vec3(1.f, 1.f, 1.f) * specularConstant * intensity;
	



	//main stuffs here
	vec3 textureColor = vec3(texture(u_texture,f_uv));
	vec3 result = (ambientLight + diffuseFinal + specularFinal) * textureColor;
	fragColor = vec4(result,1.0f);

}