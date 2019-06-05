#include "Window.h"
#include "VulkanObj.h"
#include <memory>

Window::Window(unsigned int width, unsigned int height, const char* title)
{
	prv_WinWidth = width;
	prv_WinHeight = height;
	prv_WinTitle = title;
	prv_Render = true;
	prv_Fullscreen = false;
	prv_Focused = true;
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
	window = SDL_CreateWindow(prv_WinTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, prv_WinWidth, prv_WinHeight, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
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

}

void Window::MainLoop()
{
	SDL_Event e;
	if (window)
	{
		bool quit = false;
		while (!quit)
		{
			myTime.Signal();
			SDL_PollEvent(&e);
			quit = SDL_CheckEvent(e, MASK_BIT_1 | MASK_BIT_2 | MASK_BIT_3);

			if (prv_Render)
				DrawFrames(e);
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

void Window::DrawFrames(const SDL_Event &e)
{
	Vobj->update(e);
	Vobj->draw();
}

bool Window::SDL_CheckEvent(SDL_Event &e, const uint64_t &mask)
{
	if (e.type == SDL_QUIT)
		return true;
	if ((mask & MASK_BIT_1) && e.type == SDL_WINDOWEVENT)
		SDL_EventWindow(e);
	if ((mask & MASK_BIT_2) && e.type == SDL_KEYDOWN)
		SDL_EventKeyDown(e);
	if ((mask & MASK_BIT_3) && e.type == SDL_KEYUP)
		SDL_EventKeyUp(e);

	return false;
}

void Window::SDL_EventWindow(SDL_Event &e)
{
	if (e.type == SDL_WINDOWEVENT)
	{

		switch (e.window.event)
		{
		case SDL_WINDOWEVENT_RESIZED:
		case SDL_WINDOWEVENT_SIZE_CHANGED:
		case SDL_WINDOWEVENT_RESTORED:
		case SDL_WINDOWEVENT_MAXIMIZED:
			prv_Focused = true;
			SDL_GetWindowSize(window, &prv_WinWidth, &prv_WinHeight);
			Vobj->reset_swapchain();
			break;

		case SDL_WINDOWEVENT_MINIMIZED:
			prv_Focused = false;
			while (!prv_Focused)
			{
				SDL_GetWindowSize(window, &prv_WinWidth, &prv_WinHeight);
				SDL_PollEvent(&e);
				SDL_EventWindow(e);
			}
			Vobj->reset_swapchain();
			break;

		case SDL_WINDOWEVENT_FOCUS_LOST:
			break;

		case SDL_WINDOWEVENT_CLOSE:
			SDL_Event q;
			q.type = SDL_QUIT;
			SDL_PushEvent(&q);
			break;
		}
	}
}

void Window::SDL_EventKeyDown(SDL_Event &e)
{
	SDL_Keycode key = e.key.keysym.sym;

	if (key == SDLK_F11)
	{
		if (prv_Fullscreen)
			SDL_SetWindowFullscreen(window, 0);
		else
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

		SDL_GetWindowSize(window, &prv_WinWidth, &prv_WinHeight);

		while (!prv_WinHeight || !prv_WinWidth)
		{
			SDL_GetWindowSize(window, &prv_WinWidth, &prv_WinHeight);
			SDL_PollEvent(&e);
		}
		Vobj->reset_swapchain();
		prv_Fullscreen = !prv_Fullscreen;
	}

	if (key == SDLK_LEFT || key == SDLK_KP_4)
		InputController::r_negYaw = true;
	if (key == SDLK_RIGHT || key == SDLK_KP_6)
		InputController::r_posYaw = true;
	if (key == SDLK_UP || key == SDLK_KP_8)
		InputController::r_posPitch = true;
	if (key == SDLK_DOWN || key == SDLK_KP_2)
		InputController::r_negPitch = true;
	if (key == SDLK_RCTRL || key == SDLK_KP_7)
		InputController::r_negRoll = true;
	if (key == SDLK_KP_0 || key == SDLK_KP_9)
		InputController::r_posRoll = true;

	if (key == SDLK_w)
		InputController::m_forward = true;
	if (key == SDLK_s)
		InputController::m_back = true;
	if (key == SDLK_a)
		InputController::m_left = true;
	if (key == SDLK_d)
		InputController::m_right = true;
	if (key == SDLK_e)
		InputController::m_up = true;
	if (key == SDLK_q)
		InputController::m_down = true;

	if (key == SDLK_F5)
		InputController::stop_rot = !InputController::stop_rot;

	if (key == SDLK_LSHIFT)
		InputController::Lshift = true;
	if (key == SDLK_RSHIFT)
		InputController::Rshift = true;
	if (key == SDLK_LCTRL)
		InputController::Lctrl = true;
	if (key == SDLK_RCTRL)
		InputController::Rctrl = true;
	if (key == SDLK_LALT)
		InputController::Lalt = true;
	if (key == SDLK_RALT)
		InputController::Ralt = true;

	if (key == SDLK_z)
		InputController::zCommand = true;
	if (key == SDLK_x)
		InputController::xCommand = true;
	if (key == SDLK_f)
		InputController::findpluto = true;
}

void Window::SDL_EventKeyUp(SDL_Event &e)
{
	SDL_Keycode key = e.key.keysym.sym;

	if (key == SDLK_LEFT || key == SDLK_KP_4)
		InputController::r_negYaw = false;
	if (key == SDLK_RIGHT || key == SDLK_KP_6)
		InputController::r_posYaw = false;
	if (key == SDLK_UP || key == SDLK_KP_8)
		InputController::r_posPitch = false;
	if (key == SDLK_DOWN || key == SDLK_KP_2)
		InputController::r_negPitch = false;
	if (key == SDLK_RCTRL || key == SDLK_KP_7)
		InputController::r_negRoll = false;
	if (key == SDLK_KP_0 || key == SDLK_KP_9)
		InputController::r_posRoll = false;

	if (key == SDLK_w)
		InputController::m_forward = false;
	if (key == SDLK_s)
		InputController::m_back = false;
	if (key == SDLK_a)
		InputController::m_left = false;
	if (key == SDLK_d)
		InputController::m_right = false;
	if (key == SDLK_e)
		InputController::m_up = false;
	if (key == SDLK_q)
		InputController::m_down = false;

	if (key == SDLK_LSHIFT)
		InputController::Lshift = false;
	if (key == SDLK_RSHIFT)
		InputController::Rshift = false;
	if (key == SDLK_LCTRL)
		InputController::Lctrl	= false;
	if (key == SDLK_RCTRL)
		InputController::Rctrl	= false;
	if (key == SDLK_LALT)
		InputController::Lalt	= false;
	if (key == SDLK_RALT)
		InputController::Ralt	= false;

	if (key == SDLK_z)
		InputController::zCommand = false;
	if (key == SDLK_x)
		InputController::xCommand = false;
	if (key == SDLK_f)
		InputController::findpluto = false;
}
