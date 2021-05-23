#include "ObjectLoader.h"

#include <iostream>

ObjectLoader* ObjectLoader::sharedInstance = nullptr;

ObjectLoader* ObjectLoader::getInstance()
{
	if (sharedInstance == nullptr)
		sharedInstance = new ObjectLoader();
	return sharedInstance;
}

std::vector<tinyobj::shape_t> ObjectLoader::loadFile(std::string filePath)
{
	std::string err; //error text
	std::vector <tinyobj::shape_t> shapes;
	std::vector <tinyobj::material_t> materials;
	tinyobj::LoadObj(shapes, materials, err, filePath.c_str(), basepath.c_str()); //Loads the OBJ file through tinyobjloader.h

	if (!err.empty()) {//did the OBJ load correctly?
		std::cerr << err << std::endl; // `err` may contain warning message.
	}

	return shapes;
}

ObjectLoader::ObjectLoader()
{
}
