#include "Skybox.h"

#include <string>
#include <vector>
#include <fstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include "iostream"

using namespace glm;

Skybox::Skybox()
{
	skyboxRenderingProgram = createSkyboxShaderProgram();
}

Skybox::~Skybox()
{
}

std::string Skybox::readShaderSource(const char* filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);
	std::string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint Skybox::createSkyboxShaderProgram()
{
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertShaderStr = readShaderSource("skyboxVertShader.glsl");
	std::string fragShaderStr = readShaderSource("skyboxFragShader.glsl");

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

void Skybox::initialize(int objectNumber, std::vector<GLuint>* VAO, std::vector<GLuint>* VBO, std::vector<GLuint>* EBO, std::vector<int>* meshVertexCounts,
	std::vector<int>* meshIndicesCount, std::vector<glm::vec3>* objectLocations, std::vector<float>* objectRotations)
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	VBO->push_back(0);
	VAO->push_back(0);
	EBO->push_back(0);
	objectLocations->push_back(vec3(0, 0, 0));
	objectRotations->push_back(0);
	meshVertexCounts->push_back(108);
	meshIndicesCount->push_back(36);

	glGenVertexArrays(1, &(*VAO)[objectNumber]);
	glGenBuffers(1, &(*VBO)[objectNumber]);
	glBindVertexArray((*VAO)[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, (*VBO)[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(skyboxRenderingProgram, "v_vertex"));
	glVertexAttribPointer(glGetAttribLocation(skyboxRenderingProgram, "v_vertex"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces{
	basepath + "skybox/right.jpg",
	basepath + "skybox/left.jpg",
	basepath + "skybox/bottom.jpg",
	basepath + "skybox/top.jpg",
	basepath + "skybox/front.jpg",
	basepath + "skybox/back.jpg"
	};

	skyboxTexture = loadCubemap(faces);
	glUseProgram(skyboxRenderingProgram);
	glUniform1i(glGetUniformLocation(skyboxRenderingProgram, "skybox"), 0);
}

GLuint Skybox::loadCubemap(std::vector<std::string> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void Skybox::Draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, GLuint VAO)
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glUseProgram(skyboxRenderingProgram);
	GLuint model_loc = glGetUniformLocation(skyboxRenderingProgram, "u_model");
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, acos(0.0f) * 2.0f, vec3(1, 0, 0));
	model = glm::scale(model, vec3(100.0f));
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	viewMatrix = mat4(mat3(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(skyboxRenderingProgram, "u_view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(skyboxRenderingProgram, "u_projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}
