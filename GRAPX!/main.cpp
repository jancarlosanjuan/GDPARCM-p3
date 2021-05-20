#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "ShaderProgramAttachment.h"

//image loader???
#include<SOIL2/SOIL2.h>

//for matrix
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

//for texture
#include "imageloader.h"

//fps camera

#define DEFAULT_DIRECTION_FORWARD (glm::vec3(0,0,1));
#define DEFAULT_DIRECTION_UP (glm::vec3(0,1,0));
#define DEFAULT_DIRECTION_RIGHT (glm::vec3(1,0,0));


using namespace std;
using namespace glm;

typedef struct {
	GLfloat f1;
	GLfloat f2;
	GLfloat f3;

}COLORS;


#define WIDTH 1024 //600
#define HEIGHT 768
//define something here. This contains the NUMBER OF "OBJECT CONTAINERS." What are these containers called?
#define numVAOs 20
//This is a variable that stores the program generated from the shader. Declare it here.
GLuint renderingProgram;
GLuint skyboxrenderingProgram;
//This is where we declare the "OBJECT CONTAINERS." Declare it here.
GLuint vao[numVAOs];

GLuint uniformModel, uniformView, uniformProjection;

GLuint VAO;

//Object1
GLuint VBO, EBO, TBO, NBO;
vector <tinyobj::shape_t> shapes; 
vector <tinyobj::material_t> materials;
GLuint texture_id;

//Object2
GLuint VBO1, EBO1, CBO1, TBO1, NBO1;
vector <tinyobj::shape_t> shapes1;
vector <tinyobj::material_t> materials1;
GLuint texture_id1;

//Object3
GLuint VBO2, EBO2, CBO2, TBO2, NBO2;
vector <tinyobj::shape_t> shapes2;
vector <tinyobj::material_t> materials2;
GLuint texture_id2;

//Object4
GLuint VBO3, EBO3, CBO3, TBO3, NBO3;
vector <tinyobj::shape_t> shapes3;
vector <tinyobj::material_t> materials3;
GLuint texture_id3;

//Object5
GLuint VBO5, EBO5, CBO5, TBO5, NBO5;
vector <tinyobj::shape_t> shapes5;
vector <tinyobj::material_t> materials5;
GLuint texture_id5;

//Object6
GLuint VBO6, EBO6, TBO6, NBO6;
vector <tinyobj::shape_t> shapes6;
vector <tinyobj::material_t> materials6;
GLuint texture_id6;

//Object7
GLuint VBO7, EBO7, TBO7, NBO7;
vector <tinyobj::shape_t> shapes7;
vector <tinyobj::material_t> materials7;
GLuint texture_id7;

//Object8
GLuint VBO8, EBO8, TBO8, NBO8;
vector <tinyobj::shape_t> shapes8;
vector <tinyobj::material_t> materials8;
GLuint texture_id8;

//Object9
GLuint VBO9, EBO9, TBO9, NBO9;
vector <tinyobj::shape_t> shapes9;
vector <tinyobj::material_t> materials9;
GLuint texture_id9;

//Object9
GLuint VBO10, EBO10, TBO10, NBO10;
vector <tinyobj::shape_t> shapes10;
vector <tinyobj::material_t> materials10;
GLuint texture_id10;

//Object11
GLuint VBO11, EBO11, TBO11, NBO11;
vector <tinyobj::shape_t> shapes11;
vector <tinyobj::material_t> materials11;
GLuint texture_id11;

//Object12
GLuint VBO12, EBO12, TBO12, NBO12;
vector <tinyobj::shape_t> shapes12;
vector <tinyobj::material_t> materials12;
GLuint texture_id12;

//Object13
GLuint VBO13, EBO13, CBO13, TBO13, NBO13;
vector <tinyobj::shape_t> shapes13;
vector <tinyobj::material_t> materials13;
GLuint texture_id13;

//Object14
GLuint VBO14, EBO14, CBO14, TBO14, NBO14;
vector <tinyobj::shape_t> shapes14;
vector <tinyobj::material_t> materials14;
GLuint texture_id14;

//Object15
GLuint VBO15, EBO15, CBO15, TBO15, NBO15;
vector <tinyobj::shape_t> shapes15;
vector <tinyobj::material_t> materials15;
GLuint texture_id15;

//Object16
GLuint VBO16, EBO16, CBO16, TBO16, NBO16;
vector <tinyobj::shape_t> shapes16;
vector <tinyobj::material_t> materials16;
GLuint texture_id16;

//Object17
GLuint VBO17, EBO17, CBO17, TBO17, NBO17;
vector <tinyobj::shape_t> shapes17;
vector <tinyobj::material_t> materials17;
GLuint texture_id17;

//Object17
GLuint VBO18, EBO18, CBO18, TBO18, NBO18;
vector <tinyobj::shape_t> shapes18;
vector <tinyobj::material_t> materials18;
GLuint texture_id18;

//SKYBOX
unsigned int textureID;



//Init Matrices of the object (teapot)
glm::mat4 model(1.f);
glm::vec3 position(0.f);
glm::vec3 rotation(0.f);
glm::vec3 scales(1.f);

glm::vec3 WhichWayIsUp(0.f, 1.f, 0.f);
glm::vec3 WhichWayIsFront(0.f, 0.f, -1.f);

glm::vec3 cameraPosition(0.f, 0.f, 2.0f);
glm::vec3 cameraTarget(0.0f, 0.0f, 0.f);
glm::vec3 cameraDirection = glm::normalize(cameraPosition - cameraTarget);
glm::vec3 cameraRight = glm::normalize(glm::cross(WhichWayIsUp, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

//this needs a position, target and up vector respectively. 
glm::mat4 view = 
	glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));



glm::mat4 ViewMatrix(1.f);
glm::mat4 projectionmatrix(1.f);
bool isPerspective;

float FOV = 90.f; //this is in degrees => convert to radians;
GLfloat near = 0.1f;
GLfloat far = 100.f;

double lastX = 1024/2, lastY = 768/2;
bool isMouseMoved = true;
double yawed = -90.f;
double pitched = 0.f;

//TIME
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//Skybox
unsigned int skyboxVAO, skyboxVBO;
unsigned int cubemapTexture;
vector <std::string> faces{
	"assets/Skybox/right.jpg",
	"assets/Skybox/left.jpg",
	"assets/Skybox/top.jpg",
	"assets/Skybox/bottom.jpg",
	"assets/Skybox/front.jpg",
	"assets/Skybox/back.jpg"
};

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


void addObject(int index,GLuint& VBO, GLuint& EBO, GLuint& TBO,GLuint& NBO, string name, vector<tinyobj::shape_t>& shape, vector<tinyobj::material_t>& material, string matname, string extension, GLuint& texid) {
	
	glBindVertexArray(vao[index]);
	
	string basepath = "assets/Final_Objects/";
	string inputfile = basepath + name + ".obj";

	//vector<tinyobj::shape_t> shape;
	//vector<tinyobj::material_t> material;
	string err;
	tinyobj::LoadObj(shape, material, err, inputfile.c_str(), basepath.c_str()); 
	if (!err.empty()) {
		std::cerr << err << std::endl;
	}
	else
		std::cout << "Loaded " << inputfile << " with shapes: " << shape.size() << std::endl; 

	//begin initializing the stuff
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, shape[0].mesh.positions.size() * sizeof(GLfloat), &(shape[0].mesh.positions[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_vertex"));
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_vertex"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//GL_ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape[0].mesh.indices.size() * sizeof(GLuint), &(shape[0].mesh.indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//suraj sharma is key
	if (shape[0].mesh.texcoords.size() != 0) {
		glGenBuffers(1, &TBO);
		glBindBuffer(GL_ARRAY_BUFFER, TBO);
		glBufferData(GL_ARRAY_BUFFER, shape[0].mesh.texcoords.size() * sizeof(GLfloat), &(shape[0].mesh.texcoords[0]), GL_STATIC_DRAW);
		glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_uv"));
		glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_uv"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind buffer
	}

	int image_width = 0;
	int image_height = 0;
	string base = "assets/Textures/";
	string path = base + matname + extension;
	unsigned char* image = SOIL_load_image(path.c_str(), &image_width, &image_height, NULL, SOIL_LOAD_RGBA);//tiles_sample.png
	//texture ID
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	
	//image loader checker
	if (image) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		//if texture doesnt fit just repeat. This is just for the x
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //set options//GL_REPEAT
										//if texture doesnt fit just repeat. This is just for the y//GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//anti-aliasing lang
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		

		std::cout << "TEXTURE LOADING SUCCESS" << std::endl;
	}
	else {
		std::cout << "ERROR TEXTURE LOADING FAILED" << std::endl;
	}
	
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0); //unbind the texture
	SOIL_free_image_data(image); //removes any loaded part in the image/data
	
	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, shape[0].mesh.normals.size() * sizeof(GLfloat), &(shape[0].mesh.normals[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(renderingProgram, "v_normal"));
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);  //unbind after everythings done my boi
}
void drawObject(int index, GLuint& VBO, GLuint& EBO, GLuint& TBO, GLuint& NBO, vector<tinyobj::shape_t>& shape) {
	glBindVertexArray(vao[index]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_vertex"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_uv"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glVertexAttribPointer(glGetAttribLocation(renderingProgram, "v_normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, shape[0].mesh.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
unsigned int loadCubemap(vector <std::string>& faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			glActiveTexture(0);
			glBindTexture(GL_TEXTURE_2D, 0); //unbind the texture					//not sure here
			SOIL_free_image_data(data); //removes any loaded part in the image/data
		}
		else {
			cout << "Cubemap texture failed to load at path: " << faces[i] << endl;
			SOIL_free_image_data(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureID;
}

void init(GLFWwindow* window) {
	
	//abstracted
	renderingProgram = createShaderProgram();


	glGenVertexArrays(numVAOs, vao);


	addObject(0,VBO, EBO,TBO, NBO, "Floor2", shapes, materials,"Tiles",".jpg",texture_id);
	
	addObject(1, VBO1, EBO1, TBO1, NBO1, "Walls", shapes1, materials1, "tile_gloss", ".jpg", texture_id1);
	addObject(2, VBO2, EBO2, TBO2, NBO2, "roof", shapes2, materials2, "Tiles", ".jpg", texture_id2);
	//add ceiling here//2
	//add light1 here//3
	//add light2 here//4
	addObject(5, VBO5, EBO5, TBO5, NBO5, "Toilet", shapes5, materials5, "Toilet", ".jpg", texture_id5);
	addObject(6, VBO6, EBO6, TBO6, NBO6, "ShowerRoom", shapes6, materials6, "Toilet", ".jpg", texture_id6);
	addObject(7, VBO7, EBO7, TBO7, NBO7, "Cabinet", shapes7, materials7, "wood", ".jpg", texture_id7);
	addObject(8, VBO8, EBO8, TBO8, NBO8, "Shower", shapes8, materials8, "Metal", ".JPG", texture_id8);
	addObject(9, VBO9, EBO9, TBO9, NBO9, "ShowerCaddy", shapes9, materials9, "Toilet", ".jpg", texture_id9);
	addObject(10, VBO10, EBO10, TBO10, NBO10, "Tissue", shapes10, materials10, "scratch", ".png", texture_id10);
	addObject(11, VBO11, EBO11, TBO11, NBO11, "Towel", shapes11, materials11, "Toilet", ".jpg", texture_id11);
	addObject(12, VBO12, EBO12, TBO12, NBO12, "Tub", shapes12, materials12, "Toilet", ".jpg", texture_id12);
	addObject(13, VBO13, EBO13, TBO13, NBO13, "Mirror", shapes13, materials13, "Glass_Final", ".png", texture_id13);
	addObject(14, VBO14, EBO14, TBO14, NBO14, "Door", shapes14, materials14, "wood", ".jpg", texture_id14);
	addObject(15, VBO15, EBO15, TBO15, NBO15, "Shelves", shapes15, materials15, "wall_not_final", ".jpg", texture_id15);
	addObject(16, VBO16, EBO16, TBO16, NBO16, "Sink", shapes16, materials16, "Toilet", ".jpg", texture_id16);
	addObject(17, VBO17, EBO17, TBO17, NBO17, "Folded_Towels", shapes17, materials17, "towel2", ".jpg", texture_id17);
	addObject(18, VBO18, EBO18, TBO18, NBO18, "Faucet", shapes18, materials18, "Metal", ".jpg", texture_id18);
	

	
	//initialize matrices
	//translating
	model = glm::mat4(1.f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	model = glm::scale(model, scales);

	ViewMatrix = glm::mat4(1.f);
	ViewMatrix = glm::lookAt(cameraPosition, cameraPosition + WhichWayIsFront, WhichWayIsUp);
	projectionmatrix = glm::mat4(1.f);
                                          
	projectionmatrix = glm::perspective(glm::radians(FOV), (GLfloat)WIDTH / (GLfloat)HEIGHT, near, far);
	isPerspective = true;
	//projectionmatrix = glm::ortho(-2.f, 2.f, -1.5f, 1.5f, -1.f, 1.f);

	//initialize your light bruh
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//float ydistance = 100;
	//glm::vec3 lightPosition(1.f * ydistance, 1.f * ydistance, 2.f *ydistance);
	//glUniform3fv(glGetUniformLocation(renderingProgram, "lightPos"), 1, glm::value_ptr(lightPosition));
	//cameraPos
	glm::vec3 camera = cameraPosition;
	glUniform3fv(glGetUniformLocation(renderingProgram, "v_cameraPos"), 1, glm::value_ptr(camera));



	//SKYBOX TIME
	skyboxrenderingProgram = createskyboxShaderProgram();

	//Initialize Skybox texture
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	//start binding stuff
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//load textures
	cubemapTexture = loadCubemap(faces);


	//Initialize cursor to disable on screen
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
};


int rotationaxis = 0;
int mode = 0; //true rotation, false translation

void keyboard(GLFWwindow * window,int key, int scancode, int action, int mod) {
	/*
	//mode selection
	if (key == GLFW_KEY_Q && action == GLFW_PRESS && mode == 2) {
		rotationaxis += 1;
		if (rotationaxis > 3) {
			rotationaxis = 1;
		}
		std::cout << "ROTATION AXIS: " << rotationaxis << "	(z=1 , x=2, y=3)" << std::endl;
	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS && mode != 2) {
		mode = 2;
		rotationaxis = 1;

		std::cout << "MODE IS: " << mode << "	(1 = translate, 2 = rotation, 3 = scaling)" << std::endl;
		std::cout << "ROTATION AXIS: " << rotationaxis << "	(z=1 , x=2, y=3)" << std::endl;
	}
	
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		mode = 1;
		std::cout << "MODE IS: " << mode << "	(1 = translate, 2 = rotation, 3 = scaling)" << std::endl;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		mode = 3;
		std::cout << "MODE IS: " << mode << "	(1 = translate, 2 = rotation, 3 = scaling)" << std::endl;
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		exit(0);
	}

	if (mode == 1) {
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
//			std::cout << "W" << std::endl;
			yoffset += 0.03;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			yoffset -= 0.03;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			xoffset -= 0.03;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			xoffset += 0.03;
		}
	}
	
	else if (mode == 2) {
		if (rotationaxis == 1) {
			if (key == GLFW_KEY_A && action == GLFW_PRESS) {
				zangle += 30;
//				std::cout << "A" << std::endl;
			}
			if (key == GLFW_KEY_D && action == GLFW_PRESS) {
				zangle -= 30;
			}

			if (zangle > 360) { zangle = 0; }
			if (zangle < 0) { zangle = 360; }
		}
		
		if (rotationaxis == 2) {
			if (key == GLFW_KEY_A && action == GLFW_PRESS) {
				xangle += 30;
			}
			if (key == GLFW_KEY_D && action == GLFW_PRESS) {
				xangle -= 30;
			}

			if (xangle > 360) { xangle = 0; }
			if (xangle < 0) { xangle = 360; }
		}

		if (rotationaxis == 3) {
			if (key == GLFW_KEY_A && action == GLFW_PRESS) {
				yangle += 30;
			}
			if (key == GLFW_KEY_D && action == GLFW_PRESS) {
				yangle -= 30;
			}

			if (yangle > 360) { yangle = 0; }
			if (yangle < 0) { yangle = 360; }
		}
	}
	
	else if ( mode == 3 ) {
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			scalefactor += 0.1;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			scalefactor -= 0.1;
		}

	}
	*/
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		exit(0);
		float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPosition += cameraSpeed * WhichWayIsFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPosition -= cameraSpeed * WhichWayIsFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPosition -= glm::normalize(glm::cross(WhichWayIsFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPosition += glm::normalize(glm::cross(WhichWayIsFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPosition -= glm::normalize(glm::cross(WhichWayIsFront, cameraRight)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPosition += glm::normalize(glm::cross(WhichWayIsFront, cameraRight)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		if (isPerspective) {
			//set to ortho
			//projectionmatrix = glm::ortho(-2.f, 2.f, -1.5f, 1.5f, -1.f, 1.f);
			cout << "PROJECTION SET TO ORTHO" << endl;
		}
		else {
			//set to perspective
			//projectionmatrix = glm::perspective(glm::radians(FOV), (GLfloat)WIDTH / (GLfloat)HEIGHT, near, far);
			cout << "PROJECTION SET TO PERSPECTIVE" << endl;
		}
		isPerspective = !isPerspective;
	}
		
}

void updateTime() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}
glm::vec3 setCameraDirection(float yaw, float pitch) {
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	return direction;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (isMouseMoved)
	{
		lastX = xpos;
		lastY = ypos;
		isMouseMoved = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yawed += xoffset;
	pitched += yoffset;

	if (pitched > 89.0f)
		pitched = 89.0f;
	if (pitched < -89.0f)
		pitched = -89.0f;

	glm::vec3 direction = setCameraDirection(yawed, pitched);
	direction.x = cos(glm::radians(yawed)) * cos(glm::radians(pitched));
	direction.y = sin(glm::radians(pitched));
	direction.z = sin(glm::radians(yawed)) * cos(glm::radians(pitched));
	WhichWayIsFront = glm::normalize(direction);
	return;
}

void display(GLFWwindow* window, double currentTime) { //This is our main loop. Therefore, it will be executed per frame. (Page Flipping)
	
	glUseProgram(renderingProgram);
	//update time
	updateTime();

	//enable depth test and culling
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	

	//get keyboard input before rendering
	double xpos, ypos;
	glfwGetCursorPos(window,&xpos, &ypos);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, keyboard);
	processInput(window);

	
	//////////////////////////////////////////////////////////////////dont mind this
	const float radius = 10.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;
	glm::mat4 view;
	//view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	view = glm::lookAt(cameraPosition, cameraPosition + WhichWayIsFront, cameraUp);

	projectionmatrix = glm::mat4(1.0);
	if (!isPerspective) {
		//set to ortho
		
		projectionmatrix = glm::ortho(-20.f, 20.f, -10.5f, 10.5f, near, far);
	}
	else {
		//set to perspective
		projectionmatrix = glm::perspective(glm::radians(FOV), (GLfloat)WIDTH / (GLfloat)HEIGHT, near, far);
	}
	uniformProjection = glGetUniformLocation(renderingProgram, "u_projection");
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionmatrix));

	ViewMatrix = glm::mat4(1.0);
	ViewMatrix = glm::lookAt(cameraPosition, cameraPosition + WhichWayIsFront, WhichWayIsUp);
	uniformView = glGetUniformLocation(renderingProgram, "u_view");
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
	//////////////////////////////////////////////////////////////////

	//translating
	model = glm::mat4(1.f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	model = glm::scale(model, scales);
	
	//initialize uniforms //pass to fuckin vert shader
	GLuint model_loc = glGetUniformLocation(renderingProgram, "u_model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));


	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//update uniforms
	GLuint u_texture = glGetUniformLocation(renderingProgram, "u_texture");
	glUniform1i(u_texture, 0);
	//activate texture using suraj sharma way 
	glActiveTexture(GL_TEXTURE0);
	
	glBindTexture(GL_TEXTURE_2D, texture_id);
	drawObject(0, VBO, EBO, TBO, NBO,shapes);
	glBindTexture(GL_TEXTURE_2D, texture_id1);
	drawObject(1, VBO1, EBO1, TBO1, NBO1, shapes1);
	glBindTexture(GL_TEXTURE_2D, texture_id2);
	drawObject(2, VBO2, EBO2, TBO2, NBO2, shapes2);
	//insert 2,3,4 here
	glBindTexture(GL_TEXTURE_2D, texture_id5);
	drawObject(5, VBO5, EBO5, TBO5, NBO5, shapes5);
	glBindTexture(GL_TEXTURE_2D, texture_id6);
	drawObject(6, VBO6, EBO6, TBO6, NBO6, shapes6);
	glBindTexture(GL_TEXTURE_2D, texture_id7);
	drawObject(7, VBO7, EBO7, TBO7, NBO7, shapes7);
	glBindTexture(GL_TEXTURE_2D, texture_id8);
	drawObject(8, VBO8, EBO8, TBO8, NBO8, shapes8);
	glBindTexture(GL_TEXTURE_2D, texture_id9);
	drawObject(9, VBO9, EBO9, TBO9, NBO9, shapes9);
	glBindTexture(GL_TEXTURE_2D, texture_id10);
	drawObject(10, VBO10, EBO10, TBO10, NBO10, shapes10);
	glBindTexture(GL_TEXTURE_2D, texture_id11);
	drawObject(11, VBO11, EBO11, TBO11, NBO11, shapes11);
	glBindTexture(GL_TEXTURE_2D, texture_id12);
	drawObject(12, VBO12, EBO12, TBO12, NBO12, shapes12);
	glBindTexture(GL_TEXTURE_2D, texture_id13);
	drawObject(13, VBO13, EBO13, TBO13, NBO13, shapes13);
	glBindTexture(GL_TEXTURE_2D, texture_id14);
	drawObject(14, VBO14, EBO14, TBO14, NBO14, shapes14);
	glBindTexture(GL_TEXTURE_2D, texture_id15);
	drawObject(15, VBO15, EBO15, TBO15, NBO15, shapes15);
	glBindTexture(GL_TEXTURE_2D, texture_id16);
	drawObject(16, VBO16, EBO16, TBO16, NBO16, shapes16);
	glBindTexture(GL_TEXTURE_2D, texture_id17);
	drawObject(17, VBO17, EBO17, TBO17, NBO17, shapes17);
	glBindTexture(GL_TEXTURE_2D, texture_id18);
	drawObject(18, VBO18, EBO18, TBO18, NBO18, shapes18);
	


	//draw skybox here
	glDepthFunc(GL_LEQUAL);
	glUseProgram(skyboxrenderingProgram);
	uniformProjection = glGetUniformLocation(skyboxrenderingProgram, "projection");
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionmatrix));
	uniformView = glGetUniformLocation(skyboxrenderingProgram, "view");
	view = glm::mat4(glm::mat3(ViewMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
	//skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}		

int main(void) {

	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OBJ Loader", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	init(window); //Initializes data to be drawn
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1); //something to add para ma read yung process input blahblah

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime()); //Draws per frame.
		glfwSwapBuffers(window);
		glFlush();
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
	
}