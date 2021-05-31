#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


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
#include <algorithm>

#include "GameObjectManager.h"
#include "GameScene.h"
#include "ThreadPool.h"

#include <random>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

#define WIDTH 1280
#define HEIGHT 960

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

//for testing
float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	   // positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
};
unsigned int quadVAO, quadVBO;
GLuint screenShaderProgram;
//Frame Buffer Object **1
unsigned int framebuffer;
unsigned int textureColorbuffer;
unsigned int rbo;
float loadingBar;



std::random_device randomDevice;
std::mt19937 engine(randomDevice());

int getRandomNumberInRange(int min, int max, std::mt19937& engineReference)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(engineReference);
}


Game::Game()
{
	pool = new ThreadPool("Thread Pool", 10);
	pool->startScheduler();
}

Game::~Game()
{
}

std::vector<std::string> Game::getRandomModels()
{
	int modelCount = getRandomNumberInRange(3, this->models.size(), engine);
	std::vector<std::string> returnVector;
	for(auto &s : this->models)
	{
		std::string store = s;
		returnVector.emplace_back(store);
	}

	auto rng = std::default_random_engine{};
	std::shuffle(std::begin(returnVector), std::end(returnVector), rng);

	returnVector.resize(modelCount);
	
	return returnVector;
}

void Game::init(GLFWwindow* window)
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	
	*GameObjectManager::getInstance()->getRenderingProgram() = ShaderProgram::getInstance()->createShaderProgram();
//	*GameObjectManager::getInstance()->getSkyboxRenderingProgram() = ShaderProgram::getInstance()->createSkyboxShaderProgram();

	glEnable(GL_DEPTH_TEST);
	srand(time(NULL));

	
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

	for (int i = 0; i < SCENECOUNT; i++) {//SCENECOUNT
		GameScene* scene = new GameScene(this->getRandomModels(), pool);
		scenes.push_back(scene);
		openScenes.push_back(false);
		scene->loadScene();
	}

	/*
	GameObject* ground = new GameObject();
	ground->getMesh(groundFile);
	ground->initialize(vec3(-50.0f, 0, 0), 0);
	GameObjectManager::getInstance()->addGameObject(ground);
	*/
	
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
	

	//create FBO
	//testing delete later
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);


		//Frame Buffer Object Init **2
	screenShaderProgram = ShaderProgram::getInstance()->createScreenShaderProgram();
	GLuint idk = glGetUniformLocation(screenShaderProgram, "screenTexture");
	glUniform1i(idk, 0);
	//configuration time
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);//GL_FRAMEBUFFER
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);//GL_FRAMEBUFFER
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

//	DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, DrawBuffers);
	//	
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "ERRORRRRRRRRRRRRRRRRRRRRRRRRR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		exit(69);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// draw as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	
}


void Game::display() 
{

	
	

	
	calculateLighting();

	/*for (int i = 0; i < scenes.size(); i++) {
		scenes[i]->activateScene();
	}*/


	//Framebuffer **3
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 1.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
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


	//RENDER THE FINAL FBO TO A SCREEN
	

	// Framebuffer **4
	//uncomment for blit
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glDisable(GL_DEPTH_TEST);//delete me i think
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/*
	//draw the rectangle on screen
	glUseProgram(screenShaderProgram);
//	glUseProgram(*GameObjectManager::getInstance()->getRenderingProgram());
	//uncomment to see background
	glBindVertexArray(quadVAO);
//	glBindTexture(GL_TEXTURE_2D, framebuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);//textureColorbuffer
	glDrawArrays(GL_TRIANGLES, 0, 6);
	*/


	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("FPS stats");
	{
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		static float countdown = 0.f;
		countdown -= deltaTime;
		static float fps;
		if(countdown <= 0)
		{
			countdown = 0.5f;
			fps = 1 / deltaTime;
		}
		ImGui::Text("FPS:  %.3f", fps );
	}
	ImGui::End();
	
	if(this->viewAllPressed)
	{
		ImGui::Begin("VIEW ALL WINDOW");

		
		ImGui::GetWindowDrawList()->AddImage(
			(void*)textureColorbuffer,
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + WIDTH / 2,
				ImGui::GetCursorScreenPos().y + HEIGHT / 2), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SetWindowSize({ 500,500 });

		
		ImGui::End();
	}

	
	{
		for(int i = 0; i<scenes.size() ; ++i)
		{
			this->DisplayIMGUIwindow(i , std::to_string(i));
		}
	}
	

	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	
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

bool Game::hasOtherSceneOpen(int index)
{
	for (int i = 0; i < openScenes.size(); i++)
	{
		if (i != index) {
			if (openScenes[i])
				return true;
		}
	}
	return false;
}

bool Game::checkAllIfLoaded()
{
	for(int i = 0 ; i<scenes.size() ; i++)
	{
		if((int) scenes[i]->getLoadProgress() != 1)
		{
			return false;
		}
	}
//	std::cout << "Returning true\n";
	return true;
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
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	init(window);
	
	while (!glfwWindowShouldClose(window)) {
		/*
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		*/

		
		float currentFrame = glfwGetTime();
		deltaTime += currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (deltaTime >= 1.f / 60.f)
		{
			
			processInput(window);

			processInput(window);
			display();
			glfwSwapBuffers(window);
			glfwPollEvents();
			deltaTime -= 1.f / 60.f;
			//deltaTime = 0.0f;
		}
		
		
		
		
	}
	

	//destroy imgui functions
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void Game::DisplayIMGUIwindow(int index, std::string windowName)
{
	static bool viewall = false;
	this->viewAllPressed = viewall;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ImGui::Begin(windowName.c_str());
	//	ImGui::Text("SOME TEXT 2");
	float store = scenes[index]->getLoadProgress();

	/*if(this->viewAllPressed)
	{
		std::cout << "VIEW ALL\n";
	}
	else
	{
		std::cout << "dont view all XD\n";
	}*/

	if(!this->viewAllPressed)
	{
		if (ImGui::CollapsingHeader("Viewport") && (int)store == 1 && !hasOtherSceneOpen(index))
		{

			if (!openScenes[index]) {
				openScenes[index] = true;
				scenes[index]->activateScene();
			}
			ImGui::GetWindowDrawList()->AddImage(
				(void*)textureColorbuffer,
				ImVec2(ImGui::GetCursorScreenPos()),
				ImVec2(ImGui::GetCursorScreenPos().x + WIDTH / 2,
					ImGui::GetCursorScreenPos().y + HEIGHT / 2), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::SetWindowSize({ 500,500 });

		}
		else
		{
			/*if (this->viewAllPressed)
			{
				this->viewAllPressed = false;
				for (int i = 0; i < scenes.size(); i++)
				{
					scenes[i]->allButtonPressed = false;
					openScenes[i] = false;
					scenes[i]->deactivateScene();
				}
				ImGui::SetWindowSize({ 250,150 });

			}*/
			if (openScenes[index]) {
				openScenes[index] = false;
				scenes[index]->deactivateScene();
			}
			ImGui::SetWindowSize({ 250,150 });

		}
	}
	

	if((int)store == 1 && !this->viewAllPressed)
	{
		if (ImGui::Button("Unload Scene"))
		{
			scenes[index]->unloadScene();
		}
	}
	if((int)store != 1 && !this->viewAllPressed)
	{
		if (ImGui::Button("Load Scene"))
		{
			GameScene* scene = new GameScene(this->getRandomModels(), pool);
			scenes[index] = scene;
			openScenes[index] = false;
			scene->loadScene();
		}
	}
	

	if(checkAllIfLoaded())
	{
		
		static int clicked = 0;
		std::string viewAllstring;
		if(!this->viewAllPressed)
		{
			viewAllstring = "View All";
		}
		else
		{
			viewAllstring = "Cancel View All";
		}
		
		if (ImGui::Button(viewAllstring.c_str()))
		{
			clicked++;
			
			/*
			else
			{
				for (int i = 0; i < scenes.size(); i++)
				{
					scenes[i]->allButtonPressed = false;
					openScenes[i] = false;
					scenes[i]->deactivateScene();
				}
				ImGui::SetWindowSize({ 250,150 });
			}
			*/

			if (clicked & 1)
			{

				std::cout << this->viewAllPressed << "\n";
				std::cout << "pressed view all \n";
				scenes[index]->allButtonPressed = !scenes[index]->allButtonPressed;
				viewall = true;
				//this->viewAllPressed = true;
				std::cout << this->viewAllPressed << "\n";


				for (int i = 0; i < scenes.size(); i++)
				{

					openScenes[i] = true;
					scenes[i]->activateScene();
				}


				
			}

			else
			{
				std::cout << this->viewAllPressed << "\n";
				std::cout << "pressed view all \n";
				scenes[index]->allButtonPressed = !scenes[index]->allButtonPressed;
				//this->viewAllPressed = false;
				viewall = false;
				std::cout << this->viewAllPressed << "\n";

				for (int i = 0; i < scenes.size(); i++)
				{
					scenes[i]->allButtonPressed = false;
					openScenes[i] = false;
					scenes[i]->deactivateScene();
				}
				
			}
		}
		
	}
	
	
	
	ImGui::SliderFloat("Loading Bar", &store, 0.0f, 1.0f);
	ImGui::End();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
