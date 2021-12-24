#pragma once

#include <string>
#include "OrbitingCamera.hpp"
#include "Shapes.hpp"

struct GLFWwindow;

struct WindowData
{
	lol::Camera* camera;
};

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
	void Quit();
	void Launch();

private:
	GLFWwindow* window = nullptr;
	WindowData data;

	OrbitingCamera camera;
	float pitch, yaw;

	glm::vec3 cubeOrientation, cubePosition, cubeScale;
	std::vector<Shape*> shapes;
	std::shared_ptr<lol::Texture> texture;
};