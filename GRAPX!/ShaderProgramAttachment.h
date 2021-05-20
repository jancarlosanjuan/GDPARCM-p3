#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

string readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint createShaderProgram() {

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);


	string vertShaderStr = readShaderSource("vertShader.glsl");
	string fragShaderStr = readShaderSource("fragShader.glsl");

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	glCompileShader(fShader);

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);

	return vfProgram;
}

GLuint createskyboxShaderProgram() {

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//skybox
	GLuint skyboxvShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint skyboxfShader = glCreateShader(GL_FRAGMENT_SHADER);

	//skybox
	string skyboxvertShaderStr = readShaderSource("skyboxvertShader.glsl");
	string skyboxfragShaderStr = readShaderSource("skyboxfragShader.glsl");

	//skybox
	const char* skyboxvertShaderSrc = skyboxvertShaderStr.c_str();
	const char* skyboxfragShaderSrc = skyboxfragShaderStr.c_str();

	//skybox
	glShaderSource(skyboxvShader, 1, &skyboxvertShaderSrc, NULL);
	glShaderSource(skyboxfShader, 1, &skyboxfragShaderSrc, NULL);

	glCompileShader(skyboxvShader);
	glCompileShader(skyboxfShader);

	GLuint skyboxvfProgram = glCreateProgram();
	glAttachShader(skyboxvfProgram, skyboxvShader);
	glAttachShader(skyboxvfProgram, skyboxfShader);
	glLinkProgram(skyboxvfProgram);

	return skyboxvfProgram;
}