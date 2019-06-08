#include "pch.h"
#include "VkObj_SemaphoresAndFences.h"

bool vk_sync_semaphore_and_fences(const VkDevice &device, std::vector<VkSemaphore> &image_available_semaphore, std::vector<VkSemaphore> &render_finished_semaphore, std::vector<VkFence> &fences)
{
	//Resize based on the max frames (Double Buffer)
	image_available_semaphore.resize(MAX_FRAMES);
	render_finished_semaphore.resize(MAX_FRAMES);
	fences.resize(MAX_FRAMES);

	//Semaphore Info Create
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Fence Info Create
	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//Create all the Semaphores and Fences needed
	for (unsigned int i = 0; i < MAX_FRAMES; ++i)
	{
		CHECK_VKRESULT(ias, vkCreateSemaphore(device, &semaphore_create_info, nullptr, &image_available_semaphore[i]), "Semaphore Creation for Image Available Semaphore has Failed at index " << i << "!");
		CHECK_VKRESULT(rfs, vkCreateSemaphore(device, &semaphore_create_info, nullptr, &render_finished_semaphore[i]), "Semaphore Creation for Render Finished Semaphore has Failed at index " << i << "!");
		CHECK_VKRESULT(f, vkCreateFence(device, &fence_create_info, nullptr, &fences[i]), "Fence Creation has Failed at index " << i << "!");
	}

	//Semaphores and Fences has been created successfully!
	return true;
}

bool vk_create_semaphore(const VkDevice &device, std::vector<VkSemaphore> &image_available_semaphore, std::vector<VkSemaphore> &render_finished_semaphore)
{
	//Resize based on the max frames (Double Buffer)
	image_available_semaphore.resize(MAX_FRAMES);
	render_finished_semaphore.resize(MAX_FRAMES);

	//Semaphore Info Create
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Create all the Semaphores and Fences needed
	for (unsigned int i = 0; i < MAX_FRAMES; ++i)
	{
		CHECK_VKRESULT(ias, vkCreateSemaphore(device, &semaphore_create_info, nullptr, &image_available_semaphore[i]), "Semaphore Creation for Image Available Semaphore has Failed at index " << i << "!");
		CHECK_VKRESULT(rfs, vkCreateSemaphore(device, &semaphore_create_info, nullptr, &render_finished_semaphore[i]), "Semaphore Creation for Render Finished Semaphore has Failed at index " << i << "!");
	}

	return true;
}
