#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>

class TextureManager
{
public:
	static TextureManager* getInstance();

	void loadPNGTexture(std::string filePath);
	void loadJPGTexture(std::string filePath);

	GLuint getTexture(int index);
	
private:
	TextureManager();
	TextureManager(TextureManager const&) {};             // copy constructor is private
	TextureManager& operator=(TextureManager const&) {};  // assignment operator is private
	static TextureManager* sharedInstance;

	std::vector<GLuint> textures;
};

