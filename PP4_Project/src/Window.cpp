#include "Window.h"

Window::Window(unsigned int width, unsigned int height, const char* title)
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
	glfwSetWindowUserPointer(window, this);
	
	glfwSetWindowSizeCallback(window, WindowResizeCallback);

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

		Vobj->idle_device();
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

void Window::WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	Window* win =(Window*)glfwGetWindowUserPointer(window);
	
	win->win_width = width;
	win->win_height = height;

	win->Vobj->reset_swapchain(width, height);
}
