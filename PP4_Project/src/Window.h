#ifndef WINDOWS_H
#define WINDOWS_H

#include "VulkanObj.h"
#include <memory>

class Window
{
public:
	//Constructor and Destructor
	Window(unsigned int width, unsigned int height, const char* title);
	~Window();

	//GLFW Methods
	inline const unsigned int get_width() { return prv_WinWidth; }
	inline const unsigned int get_height() { return prv_WinHeight; }
	inline void set_width(unsigned int width) { prv_WinWidth = width; }
	inline void set_height(unsigned int height) { prv_WinHeight = height; }
	__declspec(property(get = get_width, put = set_width)) unsigned int win_width;
	__declspec(property(get = get_height, put = set_height)) unsigned int win_height;
	
	void run();
	void setup_object_list(const std::vector<Object>& initial_objects);

private:
	//Private Variables, GLFW
	unsigned short prv_WinWidth;
	unsigned short prv_WinHeight;
	const char* prv_WinTitle;
	GLFWwindow* window = nullptr;
	
	//Private Variables, Vulkan
	VulkanObj* Vobj = nullptr;

	//Private Methods, GLFW
	void Init();
	void MainLoop();
	void Cleanup();
	void DrawFrames();
	static void WindowResizeCallback(GLFWwindow* window, int width, int height);
};

#endif
