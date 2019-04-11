#ifndef VKOBJ_SEMAPHORESANDFENCES_H
#define VKOBJ_SEMAPHORESANDFENCES_H

#include "VkObj_Shared.h"

struct VkObj_SemaphoresAndFences {
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> fences;
};

bool vk_sync_semaphore_and_fences(const VkDevice &device, std::vector<VkSemaphore> &image_available_semaphore, std::vector<VkSemaphore> &render_finished_semaphore, std::vector<VkFence> &fences);

#endif