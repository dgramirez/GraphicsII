#ifndef VULKAN_OBJ2_H
#define VULKAN_OBJ2_H

#include "VulkanObj/VkObj_Context.h"
#include "VulkanObj/VkObj_SemaphoresAndFences.h"
#include "objects/base_files/Object.h"
#include "objects/Camera.h"

class VulkanObj
{
public:
	VulkanObj();
	~VulkanObj();

	bool init(const char* title, SDL_Window* window, unsigned short win_width, unsigned short win_height);
	void draw_frames();
	void draw();
	void cleanup();
	void idle_device();
	void reset_swapchain();
	void setup_object_list(uint32_t size);
	void add_to_object_list(Object* object);
	void update(const SDL_Event &e);
	void update_window_title();

private:
	VkObj_SemaphoresAndFences prv_SemaphoreAndFences;
	VkObj_Context context;

	void start_frame();
	void end_frame();
	Camera camera;


	uint32_t prv_Frame = 0;
	std::vector<Object*> prv_ObjectList;

	void CleanupSwapchain();
};

#endif