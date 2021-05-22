#pragma once
#include <vector>

#include "tiny_obj_loader.h"

class ObjectLoader
{
public:
	static ObjectLoader* getInstance();
	std::vector<tinyobj::shape_t> loadFile(std::string filePath);
	
private:
	ObjectLoader();
	ObjectLoader(ObjectLoader const&) {};             // copy constructor is private
	ObjectLoader& operator=(ObjectLoader const&) {};  // assignment operator is private
	static ObjectLoader* sharedInstance;

	std::string basepath = "assets/";
};

