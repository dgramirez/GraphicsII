#ifndef WINDOWS_H
#define WINDOWS_H

class VulkanObj;
class Object;
union SDL_Event;
struct SDL_Surface;
struct SDL_Window;
#include <vector>


class Window
{
public:
	//Constructor and Destructor
	Window(unsigned int width, unsigned int height, const char* title);
	~Window();

	//SDL Methods
	inline const int get_width() const { return prv_WinWidth; }
	inline const int get_height() const { return prv_WinHeight; }
	inline void set_width(unsigned int width) { prv_WinWidth = width; }
	inline void set_height(unsigned int height) { prv_WinHeight = height; }
	__declspec(property(get = get_width, put = set_width)) int win_width;
	__declspec(property(get = get_height, put = set_height)) int win_height;
	
	void run();
	void setup_object_list(std::vector<Object*> *initial_objects);

private:
	//Private Variables, SDL
	SDL_Surface* surface = nullptr;
	SDL_Window* window = nullptr;
	const char* prv_WinTitle;
	bool prv_Fullscreen;
	bool prv_Render;
	bool prv_Focused;
	int prv_WinWidth;
	int prv_WinHeight;
	
		
	//Private Variables, Vulkan
	VulkanObj* Vobj = nullptr;

	//Private Methods, SDL
	void Init();
	void MainLoop();
	void Cleanup();
	void DrawFrames(const SDL_Event &e);

	bool SDL_CheckEvent(SDL_Event &e, const uint64_t &mask);
	void SDL_EventWindow(SDL_Event &e);
	void SDL_EventKeyDown(SDL_Event &e);
	void SDL_EventKeyUp(SDL_Event &e);
};

#endif
