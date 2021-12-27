#include "Application.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Colormaps.hpp"

#ifdef NDEBUG	
	#define FULLSCREEN
#endif

Application::~Application()
{
	
}


void Application::Quit()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	if (window != nullptr)
	{
		delete topology;

		manager.Clear();

		glfwDestroyWindow(window);
		window = nullptr;
	}

	glfwTerminate();
}

void Application::Init(int width, int height, const std::string& title)
{
	// Initialize GLFW
	if (window == nullptr)
		glfwInit();

	int windowWidth = width, windowHeight = height;
	GLFWmonitor* monitor = NULL;

#ifdef FULLSCREEN
	monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	windowWidth = mode->width;
	windowHeight = mode->height;
#endif

	// Create GLFW window
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), monitor, NULL);
	if (window == nullptr)
	{
		const char* errorbuf;
		int errorcode = glfwGetError(&errorbuf);

		glfwTerminate();

		std::stringstream errorstream;
		errorstream << "Failed to create GLFWwindow (" << errorcode << "): \n" << errorbuf << std::endl;
		throw std::runtime_error(errorstream.str());
	}

	glfwMakeContextCurrent(window);

	// Set up OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwDestroyWindow(window);
		window = nullptr;

		glfwTerminate();

		throw std::runtime_error("Failed to initialize GLAD");
	}

	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);

	// Register GLFW callbacks
	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow* window, int width, int height)
		{
			WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

			data->aspectRatio = (float)width / (float)height;

			glViewport(0, 0, width, height);
		}
	);

	glfwSetKeyCallback(window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			// Close window when pressing ESC
			if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
			{
				glfwSetWindowShouldClose(window, true);
			}
		}
	);

	glfwSetWindowUserPointer(window, &data);

	// Set up ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");

	ImGui::StyleColorsDark();

	float aspectRatio = (float)windowWidth / (float)windowHeight;
	camera = OrbitingCamera(glm::vec3(0.0f, 0.0f, 0.0f), 6.0f);
	camera.SetPerspective(100.0f, aspectRatio, 0.01f, 100.0f);
	pitch = 65.0f;
	yaw = 265.0f;
	distance = 10.0f;

	data.camera = &camera;
	data.aspectRatio = (float)width / (float)height;

	topology = new ScrollingPlot(
		manager, 
		glm::vec2(15.0f, 15.0f), 
		glm::uvec2(500, 500),
		glm::vec2(-1.0f, 1.0f),
		0.001f,
		// [](float t, float y)
		// {
		// 	return cos(t) + 0.5f * cos(2.0f * t) + 0.1f * sin(1.3f * t) + 0.3f * cos(3.5f * t) + 2.0f * (y * y) - 2.0f;
		// }
		[](float t, float y)
		{
			// Weierstraß
			float z = 0.0f;
			for(unsigned k = 1; k < 100; k++)
			{
				unsigned int twoK = std::pow(2, k);
				z += twoK * sin(twoK * t) * cos(twoK * y) / std::pow(3, k);
			}

			return z;
		}
	);

	colormap = 3;
	topology->SetColormap(colormaps[colormap]);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
}

void Application::Launch()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	
		camera.SetPosition(pitch, yaw, distance);

		topology->SetHeightMapping(enableHeightMap);
		topology->SetColorMapping(enableColorMap);
		if(enableScroll)
			topology->StepForward(3);

		if(orthogonal)
			camera.SetOrthogonal(-width / 2.0f * data.aspectRatio, width / 2.0f * data.aspectRatio, -width / 2.0, width / 2.0f, -1.0f, 100.0f);
		else
			camera.SetPerspective(fov, data.aspectRatio, 0.01f, 100.0f);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Draw(*topology);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Debug");

		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::SliderFloat("Yaw", &yaw, 0.0f, 360.0f);
			ImGui::SliderFloat("Pitch", &pitch, 1.0f, 179.0f);
			ImGui::SliderFloat("Distance", &distance, 1.0f, 14.0f);

			ImGui::Checkbox("Orthogonal", &orthogonal);

			if(ImGui::TreeNode("Perspective Settings"))
			{
				ImGui::SliderFloat("FOV", &fov, 30.0f, 110.0f);
				ImGui::TreePop();
			}

			if(ImGui::TreeNode("Orthogonal Settings"))
			{
				ImGui::SliderFloat("Width", &width, 5.0f, 50.0f);
				ImGui::TreePop();
			}
		}

		if(ImGui::CollapsingHeader("Topology"))
		{
			ImGui::Checkbox("Heightmap", &enableHeightMap);
			ImGui::Checkbox("Colormap", &enableColorMap);
			ImGui::Checkbox("Scrolling", &enableScroll);

			ImGui::ListBox("Colormap", &colormap, colormapNames.data(), colormapNames.size());
			topology->SetColormap(colormaps[colormap]);
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}
