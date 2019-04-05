#include "Window.h"

Window::Window(unsigned short width, unsigned short height, const char* title)
{
	prv_WinWidth = width;
	prv_WinHeight = height;
	prv_WinTitle = title;
}


Window::~Window()
{
	Cleanup();
}

void Window::run()
{
	Init();
	MainLoop();
	Cleanup();
}

void Window::Init()
{
	//Initialize glfw
	if (!glfwInit())
	{
		LOG("Initializing GLFW Failed!");
		return;
	}

	//Creating a GLFWwindow
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(prv_WinWidth, prv_WinHeight, prv_WinTitle, nullptr, nullptr);
	if (!window)
	{
		LOG("Window Creation Failed!");
		return;
	}
	glfwMakeContextCurrent(window);

	//Setting up Vulkan
	Vobj = new VulkanObj();
	if (!Vobj->init(prv_WinTitle, window, win_width, win_height))
	{
		LOG("Vulkan has failed its initialization.");
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void Window::MainLoop()
{
	if (window)
	{
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			DrawFrames();
		}
	}

}

void Window::Cleanup()
{
	if (Vobj)	Vobj->cleanup();

	if (window) {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

}

void Window::DrawFrames()
{
	Vobj->draw_frames();
}
