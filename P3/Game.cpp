#include "Game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include "ShaderProgramAttachment.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <time.h>

#include "GameObjectManager.h"
#include "GameObject.h"
#include "GameScene.h"
#include "ThreadPool.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

#define WIDTH 1024
#define HEIGHT 768

vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

const float CAMERA_SPEED = 25.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float horizontalAngle = -90.0f;
float verticalAngle = 0.0f;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;
float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;
unsigned int depthMapFBO;
unsigned int depthMap;
glm::vec3 lightPos(-200.0f, 200.0f, 200.0f);

Game::Game()
{
	pool = new ThreadPool("Thread Pool", 10);
	pool->startScheduler();
}

Game::~Game()
{
}

void Game::init()
{
	*GameObjectManager::getInstance()->getRenderingProgram() = ShaderProgram::getInstance()->createShaderProgram();
	*GameObjectManager::getInstance()->getSkyboxRenderingProgram() = ShaderProgram::getInstance()->createSkyboxShaderProgram();

	glEnable(GL_DEPTH_TEST);
	srand(time(NULL));

	std::vector<std::string> models;
	models.push_back(houseFile);
	models.push_back(treeTrunkFile);
	models.push_back(leavesFile);
	models.push_back(toiletFile);
	models.push_back(fallenFile);
	models.push_back(grassFile);
	models.push_back(bunnyFile);
	models.push_back(roomFile);
	models.push_back(towelFile);
	models.push_back(showerFile);
	models.push_back(showerCaddyFile);
	models.push_back(tubFile);
	for (int i = 0; i < SCENECOUNT; i++) {
		GameScene* scene = new GameScene(models, pool);
		scenes.push_back(scene);
		scene->loadScene();
	}

	GameObject* ground = new GameObject();
	ground->getMesh(groundFile);
	ground->initialize(vec3(-50.0f, 0, 0), 0);
	GameObjectManager::getInstance()->addGameObject(ground);

	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUniform1i(glGetUniformLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "u_shadowMap"), 0);
}

void Game::display() 
{
	calculateLighting();

	for (int i = 0; i < scenes.size(); i++) {
		scenes[i]->activateScene();
	}
	
	glm::mat4 projectionMatrix;
	projectionMatrix = glm::perspective(glm::radians(fov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

	GLint uniformProjection = glGetUniformLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "u_projection");
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	GLint uniformView = glGetUniformLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "u_view");
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	GameObjectManager::getInstance()->Draw();

	//skybox->Draw(viewMatrix, projectionMatrix);
	
	glBindVertexArray(0);
}

void Game::calculateLighting()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float near_plane = 1.0f, far_plane = 500.0f;
	glm::mat4 lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, WIDTH, HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(*GameObjectManager::getInstance()->getRenderingProgram());

	GLint uniformColor = glGetUniformLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "u_lightColor");
	glUniform3fv(uniformColor, 1, &lightColor[0]);

	GLint uniformLightPos = glGetUniformLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "u_lightDir");
	glUniform3fv(uniformLightPos, 1, &lightPos[0]);

	GLint uniformCamPos = glGetUniformLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "u_viewPos");
	glUniform3fv(uniformCamPos, 1, &cameraPos[0]);

	glUniformMatrix4fv(glGetUniformLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "u_lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	//rendering the actual scene
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

}

void translateInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += CAMERA_SPEED * deltaTime * cameraFront;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * CAMERA_SPEED * deltaTime;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= CAMERA_SPEED * deltaTime * cameraFront;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * CAMERA_SPEED * deltaTime;
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraPos.y -= CAMERA_SPEED * deltaTime;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		cameraPos.y += CAMERA_SPEED * deltaTime;
	}
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	translateInput(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	horizontalAngle += xoffset;
	verticalAngle += yoffset;

	// make sure that when verticalAngle is out of bounds, screen doesn't get flipped
	if (verticalAngle > 89.0f)
		verticalAngle = 89.0f;
	if (verticalAngle < -89.0f)
		verticalAngle = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle));
	front.y = sin(glm::radians(verticalAngle));
	front.z = sin(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void Game::Run()
{
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OBJ Loader", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	init();
	
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
