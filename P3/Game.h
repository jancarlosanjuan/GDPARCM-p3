#pragma once



#include <string>
#include <vector>

class GameScene;
class ThreadPool;
class GLFWwindow;
class Game
{
public:
	Game();
	~Game();

	void Run();
	void DisplayIMGUIwindow(int index, std::string windowName);
private:
	void init(GLFWwindow* window);
	void display();

	void calculateLighting();

	bool hasOtherSceneOpen(int index);

	int const SCENECOUNT = 5;

	std::string basepath = "assets/";
	std::string treeTrunkFile = basepath + "treetrunk.obj";
	std::string leavesFile = basepath + "leaves.obj";
	std::string fallenFile = basepath + "fallentree.obj";
	std::string houseFile = basepath + "house.obj";
	std::string grassFile = basepath + "grass.obj";
	std::string groundFile = basepath + "ground.obj";
	std::string toiletFile = basepath + "Final_Objects/toilet.obj";
	std::string bunnyFile = basepath + "Final_Objects/bunny.obj";
	std::string roomFile = basepath + "Final_Objects/ShowerRoom.obj";
	std::string towelFile = basepath + "Final_Objects/Folded_Towels.obj";
	std::string showerFile = basepath + "Final_Objects/Shower.obj";
	std::string showerCaddyFile = basepath + "Final_Objects/ShowerCaddy.obj";
	std::string tubFile = basepath + "Final_Objects/Tub.obj";

	std::vector<GameScene*> scenes;
	std::vector<bool> openScenes;
	
	ThreadPool* pool;
};