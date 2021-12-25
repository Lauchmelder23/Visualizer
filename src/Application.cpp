#include "Application.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

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

			float aspectRatio = (float)width / (float)height;
			data->camera->Update(100.0f, aspectRatio, 0.01f, 100.0f);

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
	camera.Update(100.0f, aspectRatio, 0.01f, 100.0f);
	pitch = -50.0f;
	yaw = 0.0f;

	data.camera = &camera;

	topology = new Topology(glm::vec2(15.0f, 7.5f), glm::uvec2(200, 100));
	glm::uvec2 size = topology->GetSize();

	float* pixels = topology->GetTopology();
	for (unsigned int y = 0; y < size.y; y++)
	{
		for (unsigned int x = 0; x < size.x; x++)
		{
			pixels[y * size.x + x] = 0.5f + (cos(x * glm::two_pi<float>() / ((float)size.x * 0.5f)) + cos(y * 0.1f)) * 0.25f;
		}
	}

	topology->MakeTexture();

	glfwWindowHint(GLFW_SAMPLES, 4);

	// glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
}

void Application::Launch()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	
		camera.SetPosition(pitch, yaw);

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
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}
