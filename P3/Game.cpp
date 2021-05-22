#include "Game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "ShaderProgramAttachment.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <time.h>

#include "Skybox.h"
#include "boxLoader.h"
#include "IETThread.h"

using namespace std;
using namespace glm;

#define WIDTH 1024
#define HEIGHT 768

GLuint renderingProgram;
GLuint lakeRenderingProgram;

vector<GLuint> VBO, VAO, EBO;//buffers for all objects

vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
//vec3 lightPosition = vec3(1.0f, -1.0f, -1.0f); unused direction vector

string basepath = "assets/";
string poolFile = basepath + "pool.obj"; //enclose the obj filename.type in the string
string treeTrunkFile = basepath + "treetrunk.obj";
string treeLeavesFile = basepath + "leaves.obj";
string fallenFile = basepath + "fallentree.obj";
string houseFile = basepath + "house.obj";
string grassFile = basepath + "grass.obj";
string groundFile = basepath + "ground.obj";

string waterFile = basepath + "SmallWaves.png";
string floorFile = basepath + "Grass 1.png";
string woodenBoardsFile = basepath + "Wooden Boards 4.png";
string leavesFile = basepath + "Leaves 1.png";
string barkFile = basepath + "Bark 2.png";

vector<GLuint> texture;

vector <int> meshVertexCounts;
vector <int> meshIndicesCount;
vector <vec3> objectLocations;
vector <float> objectRotations;

int treeCount = 0;
int fallenTreeCount = 0;
int grassCount = 0;

int poolStartIndex;
int treeStartIndex;
int fallenStartIndex;
int grassStartIndex;
int houseStartIndex;
int groundStartIndex;
int skyboxStartIndex;

const float CAMERA_SPEED = 25.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool isPerspectiveMode = true;

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

Skybox* skybox;
loadWaiter* waiter;

Game::Game()
{
	waiter = new loadWaiter();
}

Game::~Game()
{
}

vector<tinyobj::shape_t> loadFile(string filePath) {
	string err; //error text
	vector <tinyobj::shape_t> shapes;
	vector <tinyobj::material_t> materials;
	tinyobj::LoadObj(shapes, materials, err, filePath.c_str(), basepath.c_str()); //Loads the OBJ file through tinyobjloader.h

	if (!err.empty()) {//did the OBJ load correctly?
		std::cerr << err << std::endl; // `err` may contain warning message.
	}

	return shapes;
}

void initializeSkybox() {
	
}

void initializeObjectFromFile(string filePath, float r, float g, float b, vec3 position, float rotation) {
	int objectNumber = meshVertexCounts.size();

	VBO.push_back(0);
	VAO.push_back(0);
	EBO.push_back(0);
	objectLocations.push_back(position);
	objectRotations.push_back(rotation);

	string err; //error text
	vector <tinyobj::shape_t> shapes;
	vector <tinyobj::material_t> materials;
	tinyobj::LoadObj(shapes, materials, err, filePath.c_str(), basepath.c_str()); //Loads the OBJ file through tinyobjloader.h

	if (!err.empty()) {//did the OBJ load correctly?
		std::cerr << err << std::endl; // `err` may contain warning message.
	}
	meshVertexCounts.push_back(shapes[0].mesh.positions.size());
	meshIndicesCount.push_back(shapes[0].mesh.indices.size());

	glGenVertexArrays(1, &VAO[objectNumber]);
	glBindVertexArray(VAO[objectNumber]);

	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), &(shapes[0].mesh.positions[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_vertex"));
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_vertex"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size() * sizeof(float), &(shapes[0].mesh.normals[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_normal"));
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_normal"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.texcoords.size() * sizeof(float), &(shapes[0].mesh.texcoords[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_tex"));
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_tex"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	/*std::vector<vec3> colors;
	for (int i = 0; i < shapes[0].mesh.positions.size() / 3; i++) {
		colors.push_back(vec3(r, g, b));
	}
	//actual buffer work
	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &(colors[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_color"));
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_color"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

	glGenBuffers(1, &EBO[objectNumber]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[objectNumber]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(uint), &(shapes[0].mesh.indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initializeLake(string filePath, float r, float g, float b, vec3 position, float rotation) {
	int objectNumber = meshVertexCounts.size();

	VBO.push_back(0);
	VAO.push_back(0);
	EBO.push_back(0);
	objectLocations.push_back(position);
	objectRotations.push_back(rotation);

	string err; //error text
	vector <tinyobj::shape_t> shapes;
	vector <tinyobj::material_t> materials;
	tinyobj::LoadObj(shapes, materials, err, filePath.c_str(), basepath.c_str()); //Loads the OBJ file through tinyobjloader.h

	if (!err.empty()) {//did the OBJ load correctly?
		std::cerr << err << std::endl; // `err` may contain warning message.
	}
	meshVertexCounts.push_back(shapes[0].mesh.positions.size());
	meshIndicesCount.push_back(shapes[0].mesh.indices.size());

	glGenVertexArrays(1, &VAO[objectNumber]);
	glBindVertexArray(VAO[objectNumber]);

	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), &(shapes[0].mesh.positions[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(lakeRenderingProgram, "v_vertex"));
	glVertexAttribPointer(glGetAttribLocation(lakeRenderingProgram, "v_vertex"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size() * sizeof(float), &(shapes[0].mesh.normals[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(lakeRenderingProgram, "v_normal"));
	glVertexAttribPointer(glGetAttribLocation(lakeRenderingProgram, "v_normal"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.texcoords.size() * sizeof(float), &(shapes[0].mesh.texcoords[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(lakeRenderingProgram, "v_tex"));
	glVertexAttribPointer(glGetAttribLocation(lakeRenderingProgram, "v_tex"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::vector<vec3> colors;
	for (int i = 0; i < shapes[0].mesh.positions.size() / 3; i++) {
		colors.push_back(vec3(r, g, b));
	}
	//actual buffer work
	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &(colors[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(lakeRenderingProgram, "v_color"));
	glVertexAttribPointer(glGetAttribLocation(lakeRenderingProgram, "v_color"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &EBO[objectNumber]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[objectNumber]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(uint), &(shapes[0].mesh.indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initializeLoadedObject(vector<tinyobj::shape_t> shapes, float r, float g, float b, vec3 position) {
	int objectNumber = meshVertexCounts.size();

	VBO.push_back(0);
	VAO.push_back(0);
	EBO.push_back(0);
	objectLocations.push_back(position);

	meshVertexCounts.push_back(shapes[0].mesh.positions.size());
	meshIndicesCount.push_back(shapes[0].mesh.indices.size());

	glGenVertexArrays(1, &VAO[objectNumber]);
	glBindVertexArray(VAO[objectNumber]);

	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), &(shapes[0].mesh.positions[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_vertex"));
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_vertex"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size() * sizeof(float), &(shapes[0].mesh.normals[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_normal"));
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_normal"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (shapes[0].mesh.texcoords.size() > 0) {
		glGenBuffers(1, &VBO[objectNumber]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
		glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.texcoords.size() * sizeof(float), &(shapes[0].mesh.texcoords[0]), GL_STATIC_DRAW);

		glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_tex"));
		glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_tex"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/*std::vector<vec3> colors;
	for (int i = 0; i < shapes[0].mesh.positions.size() / 3; i++) {
		colors.push_back(vec3(r, g, b));
	}
	//actual buffer work
	glGenBuffers(1, &VBO[objectNumber]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[objectNumber]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &(colors[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_color"));
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_color"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

	glGenBuffers(1, &EBO[objectNumber]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[objectNumber]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(uint), &(shapes[0].mesh.indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initializeTrees(string trunkPath, string leavesPath, vec3 minPosition, vec3 maxPosition, int numTrees) {
	treeCount = numTrees;
	treeStartIndex = VAO.size();

	vector<tinyobj::shape_t> treeShapes = loadFile(trunkPath);
	vector<tinyobj::shape_t> leavesShapes = loadFile(leavesPath);

	for (int i = 0; i < numTrees; i++) {
		float xPos = rand() % (int)(maxPosition.x - minPosition.x) + minPosition.x;
		float zPos = rand() % (int)(maxPosition.z - minPosition.z) + minPosition.z;
		float rotation = 0;
		objectRotations.push_back(rotation);
		objectRotations.push_back(rotation);
		initializeLoadedObject(treeShapes, 1.0f, 1.0f, 1.0f, vec3(xPos, 0, zPos));
		initializeLoadedObject(leavesShapes, 1.0f, 1.0f, 1.0f, vec3(xPos, 4.25f, zPos));
	}
}

void initializeFallenTrees(string fallenPath, vec3 minPosition, vec3 maxPosition, int numTrees) {
	fallenTreeCount = numTrees;
	fallenStartIndex = VAO.size();

	vector<tinyobj::shape_t> treeShapes = loadFile(fallenPath);

	for (int i = 0; i < numTrees; i++) {
		float xPos = rand() % (int)(maxPosition.x - minPosition.x) + minPosition.x;
		float zPos = rand() % (int)(maxPosition.z - minPosition.z) + minPosition.z;
		objectRotations.push_back(rand() % 360);
		initializeLoadedObject(treeShapes, 1.0f, 1.0f, 1.0f, vec3(xPos, 0.5f, zPos));
	}
}

void initializeGrass(string grassPath, vec3 minPosition, vec3 maxPosition, int numGrass) {
	grassCount = numGrass;
	grassStartIndex = VAO.size();

	vector<tinyobj::shape_t> grassShapes = loadFile(grassPath);

	for (int i = 0; i < numGrass; i++) {
		float xPos = rand() % (int)(maxPosition.x - minPosition.x) + minPosition.x;
		float zPos = rand() % (int)(maxPosition.z - minPosition.z) + minPosition.z;
		objectRotations.push_back(rand() % 360);
		initializeLoadedObject(grassShapes, 0.5647f, 0.933f, 0.5647f, vec3(xPos, 0.25f, zPos));
	}
}

void loadPNGTexture(string filePath) {
	texture.push_back(0);
	glGenTextures(1, &texture.back());
	glBindTexture(GL_TEXTURE_2D, texture.back()); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void loadJPGTexture(string filePath) {
	texture.push_back(0);
	glGenTextures(1, &texture.back());
	glBindTexture(GL_TEXTURE_2D, texture.back()); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void drawObject(int index, GLuint program) {
	glBindVertexArray(VAO[index]);

	GLuint model_loc = glGetUniformLocation(program, "u_model");
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, objectLocations[index]);
	model = glm::rotate(model, objectRotations[index], vec3(0, 1, 0));
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[index]);//rebind indices buffer from init() before drawing
	glDrawElements(GL_TRIANGLES, meshIndicesCount[index], GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//unbind
}

void init(GLFWwindow* window) {
	renderingProgram = ShaderProgram::getInstance()->createShaderProgram();
	lakeRenderingProgram = ShaderProgram::getInstance()->createShadowlessShaderProgram();
	skybox = new Skybox();

	glEnable(GL_DEPTH_TEST);
	srand(time(NULL));

	poolStartIndex = 0;
	initializeLake(poolFile, 0, 1.0f, 1.0f, vec3(0, -8.0f, -150.0f), 0);
	houseStartIndex = 1;
	initializeObjectFromFile(houseFile, 1.0f, 1.0f, 1.0f, vec3(50.0f, 0.25f, 0), acos(0.0) * 3);
	groundStartIndex = 2;
	initializeObjectFromFile(groundFile, 1.0f, 1.0f, 1.0f, vec3(-50.0f, 0, 0), 0);
	initializeTrees(treeTrunkFile, treeLeavesFile, vec3(-200.0f, 0, -50.0f), vec3(-100.0f, 0, 100.0f), 100);
	initializeFallenTrees(fallenFile, vec3(-150.0f, 0, -25.0f), vec3(-50.0f, 0, 100.0f), 25);
	initializeGrass(grassFile, vec3(-200.0f, 0, -25.0f), vec3(100.0f, 0, 100.0f), 500);

	glUseProgram(renderingProgram);
	glUniform1i(glGetUniformLocation(renderingProgram, "texture1"), 1);

	loadPNGTexture(waterFile);
	loadPNGTexture(floorFile);
	loadPNGTexture(woodenBoardsFile);
	loadPNGTexture(leavesFile);
	loadJPGTexture(barkFile);

	glBindVertexArray(0);

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

	glUniform1i(glGetUniformLocation(renderingProgram, "u_shadowMap"), 0);

	skyboxStartIndex = VAO.size();
	skybox->initialize(meshVertexCounts.size(), &VAO, &VBO, &EBO, &meshVertexCounts, &meshIndicesCount, &objectLocations, &objectRotations);

	glUseProgram(lakeRenderingProgram);
	glUniform1i(glGetUniformLocation(lakeRenderingProgram, "texture1"), 1);
	glUniform1i(glGetUniformLocation(lakeRenderingProgram, "u_shadowMap"), 0);
}

void display(GLFWwindow* window, double currentTime) {

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 500.0f;
	lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, WIDTH, HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(renderingProgram);

	glm::mat4 projectionMatrix(1.0);
	if (isPerspectiveMode)
		projectionMatrix = glm::perspective(glm::radians(fov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	else
		projectionMatrix = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, 0.1f, 100.0f);
	GLint uniformProjection = glGetUniformLocation(renderingProgram, "u_projection");
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraFront);
	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
	glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	GLint uniformView = glGetUniformLocation(renderingProgram, "u_view");
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	GLint uniformColor = glGetUniformLocation(renderingProgram, "u_lightColor");
	glUniform3fv(uniformColor, 1, &lightColor[0]);

	GLint uniformLightPos = glGetUniformLocation(renderingProgram, "u_lightDir");
	glUniform3fv(uniformLightPos, 1, &lightPos[0]);

	GLint uniformCamPos = glGetUniformLocation(renderingProgram, "u_viewPos");
	glUniform3fv(uniformCamPos, 1, &cameraPos[0]);

	glUniformMatrix4fv(glGetUniformLocation(renderingProgram, "u_lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	//rendering the actual scene
	{glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	drawObject(houseStartIndex, renderingProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	drawObject(groundStartIndex, renderingProgram);

	for (int i = 0; i < treeCount; i++) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[4]);//trunk
		drawObject(i * 2 + treeStartIndex, renderingProgram);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[3]);//leaves
		drawObject(i * 2 + 1 + treeStartIndex, renderingProgram);
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	for (int i = 0; i < fallenTreeCount; i++) {
		drawObject(i + fallenStartIndex, renderingProgram);
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	for (int i = 0; i < grassCount; i++) {
		drawObject(i + grassStartIndex, renderingProgram);
	}

	glUseProgram(lakeRenderingProgram);
	glUniformMatrix4fv(glGetUniformLocation(lakeRenderingProgram, "u_projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(glGetUniformLocation(lakeRenderingProgram, "u_view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3fv(glGetUniformLocation(lakeRenderingProgram, "u_lightColor"), 1, &lightColor[0]);
	glUniform3fv(glGetUniformLocation(lakeRenderingProgram, "u_lightDir"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(lakeRenderingProgram, "u_viewPos"), 1, &cameraPos[0]);
	glUniformMatrix4fv(glGetUniformLocation(lakeRenderingProgram, "u_lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	drawObject(poolStartIndex, lakeRenderingProgram);

	skybox->Draw(viewMatrix, projectionMatrix, VAO[skyboxStartIndex]);
	}

	glBindVertexArray(0);
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
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		isPerspectiveMode = true;
	else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		isPerspectiveMode = false;

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

	init(window); //Initializes data to be drawn

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void loadWaiter::OnFinishedExecution()
{
	this->finishedLoading = true;
	std::cout << "done" << std::endl;
}
