#pragma once

#include <string>
#include "OrbitingCamera.hpp"
#include "Topology.hpp"

struct GLFWwindow;

struct WindowData
{
	OrbitingCamera* camera;
	float aspectRatio;
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
	float pitch, yaw, distance;
	bool orthogonal = false;
	float fov = 100.0f;
	float width = 10.0f;

	bool enableHeightMap = true;
	bool enableColorMap = true;
	bool enableScroll = false;
	

	Topology* topology;
};