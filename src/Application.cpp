#include "Application.hpp"

#include <iostream>
#include <bitset>
#include <stdexcept>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SDL2/SDL.h>
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
		delete spectrogram;

		manager.Clear();

		glfwDestroyWindow(window);
		window = nullptr;
	}

	SDL_Quit();
	glfwTerminate();
}

void Application::Init(int width, int height, const std::string& title)
{
	// Initialize GLFW
	if (window == nullptr)
	{
		glfwInit();
		SDL_Init(SDL_INIT_AUDIO);
	}

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

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

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
	yaw = 85.0f;
	distance = 10.0f;

	data.camera = &camera;
	data.aspectRatio = (float)width / (float)height;

	spectrogram = new Spectrogram(
		manager,
		glm::vec2(5.0f, 5.0f),
		glm::uvec2(200, 2000),
		AudioFile("res/payday.wav")
	);

	colormap = 3;
	spectrogram->SetColormap(colormaps[colormap]);

	// glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	frameTimerStart = std::chrono::system_clock::now();
}

void Application::Launch()
{
	while (!glfwWindowShouldClose(window))
	{
		unsigned int frametime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - frameTimerStart).count();
		frameTimerStart = std::chrono::system_clock::now();
		float fps = 1000.0f / frametime;

		glfwPollEvents();
	
		camera.SetPosition(pitch, yaw, distance);

		spectrogram->SetHeightMapping(enableHeightMap);
		spectrogram->SetColorMapping(enableColorMap);
		if(enableScroll)
			spectrogram->Update();

		if(orthogonal)
			camera.SetOrthogonal(-width / 2.0f * data.aspectRatio, width / 2.0f * data.aspectRatio, -width / 2.0, width / 2.0f, -1.0f, 100.0f);
		else
			camera.SetPerspective(fov, data.aspectRatio, 0.01f, 100.0f);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Draw(*spectrogram);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Debug");

		ImGui::Text("FPS: %f", fps);

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
			spectrogram->SetColormap(colormaps[colormap]);
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}
