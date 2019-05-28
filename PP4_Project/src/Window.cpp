#include "Window.h"

Window::Window(unsigned int width, unsigned int height, const char* title)
{
	prv_WinWidth = width;
	prv_WinHeight = height;
	prv_WinTitle = title;
	Vobj = new VulkanObj();
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

void Window::setup_object_list(const std::vector<Object3D>& initial_objects)
{
	for (uint32_t i = 0; i < CAST(uint32_t, initial_objects.size()); ++i)
		Vobj->add_to_object_list(initial_objects[i]);
}

void Window::Init()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("Iniitalizing SDL Failed!");
		return;
	}

	SDL_Vulkan_LoadLibrary(0);

	//Create SDL Window
	window = SDL_CreateWindow(prv_WinTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, prv_WinWidth, prv_WinHeight, SDL_WINDOW_VULKAN);
	if (window == nullptr)
	{
		LOG("Creating the window has failed!");
		return;
	}

	//Setting up Vulkan
	if (!Vobj->init(prv_WinTitle, window, win_width, win_height))
	{
		LOG("Vulkan has failed its initialization.");
		//glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	// 	//Initialize glfw
// 	if (!glfwInit())
// 	{
// 		LOG("Initializing GLFW Failed!");
// 		return;
// 	}
// 
// 	//Creating a GLFWwindow
// 	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
// 	window = glfwCreateWindow(prv_WinWidth, prv_WinHeight, prv_WinTitle, nullptr, nullptr);
// 	if (!window)
// 	{
// 		LOG("Window Creation Failed!");
// 		return;
// 	}
// 	glfwMakeContextCurrent(window);
// 	glfwSetWindowUserPointer(window, this);
// 	
// 	glfwSetFramebufferSizeCallback(window, WindowResizeCallback);
// 
}

void Window::MainLoop()
{
	SDL_Event e;
	if (window)
	{
		bool quit = false;
		while (!quit)
		{
			SDL_PollEvent(&e);
			if (e.type == SDL_QUIT)
				quit = true;

			DrawFrames();
		}

		Vobj->idle_device();
	}

}

void Window::Cleanup()
{
	if (Vobj)	Vobj->cleanup();

	if (window) {
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

}

void Window::DrawFrames()
{
//	Vobj->update();
	Vobj->draw();
}

void Window::WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	Window* win =(Window*)glfwGetWindowUserPointer(window);
	
	win->win_width = width;
	win->win_height = height;

	win->Vobj->reset_swapchain(width, height);
}
