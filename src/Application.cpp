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
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	for (Shape* shape : shapes)
		delete shape;

	if (window != nullptr) 
	{
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
	pitch = camera.GetAngles().x;
	yaw = camera.GetAngles().y;

	Shape* shape = new Cube();
	shape->Move(glm::vec3(0.0f, -2.0f, 0.0f));
	shape->Rotate(glm::vec3(1.0f, 1.0f, 1.0f), 60);
	shapes.push_back(shape);

	shape = new Cube();
	shape->Move(glm::vec3(0.0f, 2.0f, 0.0f));
	shape->Rotate(glm::vec3(0.5f, 1.0f, 1.2f), 60);
	shapes.push_back(shape);

	shape = new Pyramid();
	shape->Move(glm::vec3(0.0f, 0.0f, 3.0f));
	shape->Rotate(glm::vec3(1.0f, 0.0f, 0.0f), -90);
	shapes.push_back(shape);

	shape = new Pyramid();
	shape->Move(glm::vec3(0.0f, 0.0f, -3.0f));
	shape->Rotate(glm::vec3(1.0f, 0.3f, 1.2f), 120);
	shapes.push_back(shape);

	cubePosition = glm::vec3(0.0f);
	cubeOrientation = glm::vec3(0.0f);
	cubeScale = glm::vec3(1.0f);

	data.camera = &camera;

	glfwWindowHint(GLFW_SAMPLES, 4);

	// glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
}

void Application::Launch()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// plot->SetPosition(cubePosition);
		// plot->SetRotation(cubeOrientation);
		// plot->SetScale(cubeScale);

		camera.SetPosition(pitch, yaw);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		for (Shape* shape : shapes)
			camera.Draw(*shape);

		ImGui::Begin("Debug");

		/*if (ImGui::CollapsingHeader("Plot"))
		{
			ImGui::SliderFloat3("Position", &(cubePosition[0]), -2.0f, 2.0f);
			ImGui::SliderFloat3("Orientation", &(cubeOrientation[0]), -glm::pi<float>(), glm::pi<float>());
			ImGui::SliderFloat3("Scale", &(cubeScale[0]), 0.0f, 2.0f);
		}*/

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
