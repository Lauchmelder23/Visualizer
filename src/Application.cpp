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

	// Create GLFW window
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
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

	glViewport(0, 0, width, height);

	// Register GLFW callbacks
	glfwSetFramebufferSizeCallback(window, 
		[](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		}
	);
}

void Application::Launch()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}
}
