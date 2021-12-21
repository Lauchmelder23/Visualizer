#pragma once

#include <string>
#include "Cuboid.hpp"

struct GLFWwindow;

class Application
{
/////////////////////////////////////////////////////////
////// SINGLETON BOILERPLATE ////////////////////////////
/////////////////////////////////////////////////////////
public:
	static Application& Instance()
	{
		static Application app;
		return app;
	}


private:
	Application() = default;
	~Application();
	Application(const Application& other) = delete;

/////////////////////////////////////////////////////////
////// APPLICATION IMPLEMENTATION ///////////////////////
/////////////////////////////////////////////////////////

public:
	void Init(int width, int height, const std::string& title);
	void Launch();

private:
	GLFWwindow* window = nullptr;

	glm::vec3 cubeOrientation, cubePosition, cubeScale;
	Cuboid* cube;
};