#include "GameObjectManager.h"

GameObjectManager* GameObjectManager::sharedInstance = nullptr;

GameObjectManager* GameObjectManager::getInstance()
{
    if (sharedInstance == nullptr)
        sharedInstance = new GameObjectManager();
    return sharedInstance;
}

std::vector<GLuint>* GameObjectManager::getVBO()
{
    return &VBO;
}

std::vector<GLuint>* GameObjectManager::getVAO()
{
    return &VAO;
}

std::vector<GLuint>* GameObjectManager::getEBO()
{
    return &EBO;
}

std::vector<int>* GameObjectManager::getMeshVertexCounts()
{
    return &meshVertexCounts;
}

std::vector<int>* GameObjectManager::getMeshIndicesCount()
{
    return &meshIndicesCount;
}

std::vector<glm::vec3>* GameObjectManager::getObjectLocations()
{
    return &objectLocations;
}

std::vector<float>* GameObjectManager::getObjectRotations()
{
    return &objectRotations;
}

GLuint* GameObjectManager::getRenderingProgram()
{
    return &renderingProgram;
}

GameObjectManager::GameObjectManager()
{
}
