#include <iostream>
#include "Application.hpp"

int main(int argc, char** argv)
{
	Application& app = Application::Instance();

	try
	{
		app.Init(1280, 720, "Visualizer");
	}
	catch (const std::runtime_error& err)
	{
		std::cerr << "Application initialization failed\n\n" << err.what();
		return 1;
	}

	app.Launch();

	return 0;
}