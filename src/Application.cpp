#include "Application.hpp"

#include <stdexcept>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Application::~Application()
{
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
	if(window == nullptr)
		glfwInit();

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	// Create GLFW window
	window = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, NULL);
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

	glViewport(0, 0, mode->width, mode->height);

	// Register GLFW callbacks
	glfwSetFramebufferSizeCallback(window, 
		[](GLFWwindow* window, int width, int height)
		{
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


	// TODO: Remove, this should probably be done elsewhere
	model = VAOFactory::Produce(
		{
			-0.5f, -0.5f,
			0.0f, 0.5f,
			0.5f, -0.5f
		},
		{
			0, 1, 2
		},
		{
			{ 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0 }
		}
	);

	shader = ShaderFactory::Produce(
		R"(
		#version 460 core

		layout (location = 0) in vec2 aPos;

		void main()
		{
			gl_Position = vec4(aPos, 0.0, 1.0);
		}
		)",
		R"(
		#version 460 core

		out vec4 FragColor;

		void main()
		{
			FragColor = vec4(0.5f, 0.0f, 0.8f, 1.0f);
		}
		)"
	);

	if (!shader->Good())
	{
		throw std::runtime_error("Shader creation failed");
	}
}

void Application::Launch()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader->Use();
		model->Render();

		glfwSwapBuffers(window);
	}
}
