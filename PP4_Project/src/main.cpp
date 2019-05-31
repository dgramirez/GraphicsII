#include "Window.h"
#include "Assets.h"
#include "Object.h"

Object3D create_pyramid();
Object3D create_two_squares();
Object3D create_axe();
Object3D create_terrain();
Object3D create_earth();
void AxeRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view);
void PyramidRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view);
void SquareFlag(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view);
void EarthRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view);

int main(int argc, char* args[])
{
	Window* myWindow = new Window(800, 600, "My New Window");

	Object3D pyramid = create_pyramid();
//	Object3D two_squares = create_two_squares();
	Object3D axe = create_axe();
//	Object3D terrain = create_terrain();
	Object3D earth = create_earth();

	std::vector<Object3D> Object_List;
	Object_List.push_back(axe);
	Object_List.push_back(pyramid);
	Object_List.push_back(earth);

	myWindow->setup_object_list(Object_List);

#ifdef _DEBUG
	try
	{
		myWindow->run();
	}
	catch (const std::exception& e)
	{
		LOG(e.what())
	}
#else
	myWindow->run();
#endif
#if _DEBUG
	LOG("\n\nPress the enter key once or twice to exit.")
	std::cin.ignore().get();
#endif

	return 0;
}

Object3D create_pyramid()
{
	std::vector<Vertex> pyramid_vertex = {
		{ { 0.0f, -0.5f,  0.0f, 1.0f}, V_COLOR_RED	 , {0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Top point
		{ { 0.5f,  0.5f,  0.5f, 1.0f}, V_COLOR_GREEN , {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 1
		{ {-0.5f,  0.5f,  0.5f, 1.0f}, V_COLOR_BLUE  , {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 2
		{ { 0.5f,  0.5f, -0.5f, 1.0f}, V_COLOR_CYAN  , {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 3
		{ {-0.5f,  0.5f, -0.5f, 1.0f}, V_COLOR_YELLOW, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }	//Bottom Vertex 4;
	};

	const std::vector<uint32_t> pyramid_index = {
	0, 1, 2,
	0, 2, 4,
	0, 3, 1,
	0, 4, 3,
	1, 3, 4,
	1, 4, 2
	};

	Texture* celestial = new Texture(celestial_width, celestial_height, (void*)celestial_pixels, celestial_numlevels, TEXTURE_TYPE_DOTH);

	Object3D pyramid(pyramid_vertex, pyramid_index, celestial);

	pyramid.texture->swizzle(VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A, VK_COMPONENT_SWIZZLE_R);

	pyramid.world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -3.0f));
	pyramid.uniformFctn = PyramidRotation;

	return pyramid;
}
Object3D create_two_squares()
{
	std::vector<Vertex> two_squares_vertices = {
		{{-0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},

		{{-0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}
	};

	const std::vector<uint32_t> two_squares_indices = {
	0, 1, 2,
	2, 3, 0,
	4, 5, 6,
	6, 7, 4
	};

	Texture* celestial = new Texture(celestial_width, celestial_height, (void*)celestial_pixels, celestial_numlevels, false);

	Object3D two_squares(two_squares_vertices, two_squares_indices, celestial);
	two_squares.uniformFctn = SquareFlag;
	return two_squares;
}
Object3D create_axe()
{
	Object3D axe("Axe.fbx", nullptr, 10);
	axe.world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	axe.uniformFctn = AxeRotation;
	return axe;
}
Object3D create_terrain()
{
	Object3D myTerrain("Axe.fbx"); //Placeholder
	
	return myTerrain;
}
Object3D create_earth()
{
	Object3D myEarth("earth2.fbx", nullptr, 75);
	myEarth.world_matrix = glm::mat4(1.0f);
	myEarth.uniformFctn = EarthRotation;

	return myEarth;
}

void AxeRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view)
{
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

	Mvp_object mvp;

	mvp.model = model;
	mvp.view = view;
	mvp.projection = glm::infinitePerspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f);
	mvp.projection[1][1] = -mvp.projection[1][1];

	void* data;
	vkMapMemory(device, uniform_memory[current_image], 0, sizeof(Mvp_object), 0, &data);
	memcpy(data, &mvp, sizeof(Mvp_object));
	vkUnmapMemory(device, uniform_memory[current_image]);
}
void PyramidRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view)
{
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

	Mvp_object mvp;

	mvp.model = model * glm::rotate(glm::mat4(1.0f), delta_time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mvp.view = view;
//	mvp.projection = glm::perspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 20.0f);
	mvp.projection = glm::infinitePerspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f);
	mvp.projection[1][1] = -mvp.projection[1][1];

	void* data;
	vkMapMemory(device, uniform_memory[current_image], 0, sizeof(Mvp_object), 0, &data);
	memcpy(data, &mvp, sizeof(Mvp_object));
	vkUnmapMemory(device, uniform_memory[current_image]);
}
void SquareFlag(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view)
{
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

	Mvp_object mvp;

#if USE_PYRAMID
	//mvp.model = glm::rotate(glm::mat4(1.0f), delta_time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mvp.model = model
		* glm::rotate(glm::mat4(1.0f), delta_time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f))
		* glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
#else
	mvp.model = glm::rotate(glm::mat4(1.0f), delta_time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
#endif
	mvp.view = view;
//	mvp.projection = glm::perspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 100.0f);
	mvp.projection = glm::infinitePerspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f);
	
	mvp.projection[1][1] = -mvp.projection[1][1];

	void* data;
	vkMapMemory(device, uniform_memory[current_image], 0, sizeof(Mvp_object), 0, &data);
	memcpy(data, &mvp, sizeof(Mvp_object));
	vkUnmapMemory(device, uniform_memory[current_image]);
}
void EarthRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view)
{
	float delta = (float)myTime.TotalTime();
	Mvp_object mvp;

	mvp.model = model * glm::rotate(glm::mat4(1.0f), delta * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mvp.view = view;
	mvp.projection = glm::infinitePerspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f);

	mvp.projection[1][1] = -mvp.projection[1][1];

	void* data;
	vkMapMemory(device, uniform_memory[current_image], 0, sizeof(Mvp_object), 0, &data);
	memcpy(data, &mvp, sizeof(Mvp_object));
	vkUnmapMemory(device, uniform_memory[current_image]);
}
