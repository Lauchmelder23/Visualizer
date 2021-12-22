#pragma once

#include <string>
#include "backend/Camera.hpp"
#include "Plot3D.hpp"

struct GLFWwindow;

struct WindowData
{
	Camera* camera;
	OrthogonalCamera* orthoCam;
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
	void Launch();

private:
	GLFWwindow* window = nullptr;
	WindowData data;

	Camera camera;
	OrthogonalCamera orthoCam;
	CameraBase* activeCamera;

	glm::vec3 cubeOrientation, cubePosition, cubeScale;
	Plot3D* plot;
};