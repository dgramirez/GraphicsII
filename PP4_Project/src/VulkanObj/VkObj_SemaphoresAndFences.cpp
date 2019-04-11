#include "VkObj_SemaphoresAndFences.h"

bool vk_sync_semaphore_and_fences(const VkDevice &device, std::vector<VkSemaphore> &image_available_semaphore, std::vector<VkSemaphore> &render_finished_semaphore, std::vector<VkFence> &fences)
{
	image_available_semaphore.resize(MAX_FRAMES_FLIGHT);
	render_finished_semaphore.resize(MAX_FRAMES_FLIGHT);
	fences.resize(MAX_FRAMES_FLIGHT);

	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkResult r;
	for (unsigned int i = 0; i < MAX_FRAMES_FLIGHT; ++i)
	{
		r = vkCreateSemaphore(device, &semaphore_create_info, nullptr, &image_available_semaphore[i]);
		if (r) {
			LOG("Semaphore Creation for Image Available Semaphore has Failed!");
			return false;
		}

		r = vkCreateSemaphore(device, &semaphore_create_info, nullptr, &render_finished_semaphore[i]);
		if (r) {
			LOG("Semaphore Creation for Render Finished Semaphore has Failed!");
			return false;
		}

		r = vkCreateFence(device, &fence_create_info, nullptr, &fences[i]); 
		if (r){
			LOG("Fence Creation has Failed!");
			return false;
		}
	}

	return true;
}
