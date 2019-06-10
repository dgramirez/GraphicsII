#include "pch.h"
#include "planets.h"
#include "objects/Camera.h"
#include "texture_h/celestial.h"
#include "texture_h/axeTexture.h"
#include "texture_h/fighter.h"

Object* create_pyramid()
{
	std::vector<Vertex> pyramid_vertex = {
		{ { 0.0f, -0.5f,  0.0f }, V_COLOR_RED	, {0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Top point
		{ { 0.5f,  0.5f,  0.5f }, V_COLOR_GREEN , {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 1
		{ {-0.5f,  0.5f,  0.5f }, V_COLOR_BLUE  , {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 2
		{ { 0.5f,  0.5f, -0.5f }, V_COLOR_CYAN  , {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 3
		{ {-0.5f,  0.5f, -0.5f }, V_COLOR_YELLOW, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }	//Bottom Vertex 4;
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

	Object *pyramid = new Object(pyramid_vertex, pyramid_index, celestial);

	pyramid->texture->swizzle(VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A, VK_COMPONENT_SWIZZLE_R);

	pyramid->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, 0.0f));
	pyramid->uniform_function = PyramidRotation;

	return pyramid;
}
Object* create_normal_ship()
{
	Object *Ship = new Object("assets\\Trident-A10.FBX", ".\\assets\\misc\\", 500.0f);
	Ship->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -2.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Ship->uniform_function = AxeRotation;
	return Ship;
}
Object* create_fighter_ship()
{
	Texture* myFighterTexture = new Texture("assets\\misc\\fighter_texture.jpg");

	uint32_t verts = sizeof(fighter_data) / sizeof(_OBJ_VERT_);
	uint32_t inds = sizeof(fighter_indicies) / sizeof(unsigned int);

	Object *myFighter = new Object(fighter_data, verts, fighter_indicies, inds, myFighterTexture);
	myFighter->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(165.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	myFighter->uniform_function = AxeRotation;

	return myFighter;
}
Object* create_grid()
{
	std::vector<Vertex> grid_vertices;
	std::vector<uint32_t> grid_indices;
	uint32_t index = 0;
	for (float i = -GRID_LENGTH; i <= GRID_LENGTH; i += GRID_SUBLENGTH)
	{
		//Setup 
		grid_vertices.resize(grid_vertices.size() + 8);

		//Vertical
		grid_vertices[index + 0] = { { i + 0.0f, 0.0f, -GRID_LENGTH }, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 1] = { { i + 0.1f, 0.0f, -GRID_LENGTH }, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 2] = { { i + 0.0f, 0.0f,  GRID_LENGTH }, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 3] = { { i + 0.1f, 0.0f,  GRID_LENGTH }, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_indices.push_back(index + 0);
		grid_indices.push_back(index + 1);
		grid_indices.push_back(index + 2);
		grid_indices.push_back(index + 1);
		grid_indices.push_back(index + 3);
		grid_indices.push_back(index + 2);

		//Horizontal
		grid_vertices[index + 4] = { { -GRID_LENGTH, 0.0f, i + 0.0f }, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 5] = { { -GRID_LENGTH, 0.0f, i + 0.1f }, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 6] = { {  GRID_LENGTH, 0.0f, i + 0.0f }, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 7] = { {  GRID_LENGTH, 0.0f, i + 0.1f }, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_indices.push_back(index + 6);
		grid_indices.push_back(index + 7);
		grid_indices.push_back(index + 4);
		grid_indices.push_back(index + 7);
		grid_indices.push_back(index + 5);
		grid_indices.push_back(index + 4);
		index += 8;
	}

	Texture *grid_texture = new Texture(".\\assets\\misc\\white_pixel.png");
	Object *Grid = new Object(grid_vertices, grid_indices, grid_texture);

	Grid->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Grid->uniform_function = AxeRotation;

	return Grid;
}
Object* create_sphere(const char* fbxfilepath, const char* texturelocation, Texture* texturedoth, const float &scale_down, const glm::mat4 &model_matrix)
{
	Object *mySphere = new Object(fbxfilepath, texturelocation, scale_down);
	mySphere->model_matrix = model_matrix;

	return mySphere;
}

void AxeRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_MVP mvp;

	mvp.model = obj.model_matrix;
	mvp.view = camera.view_inverse;
	mvp.projection = camera.perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];

	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);

	obj.model_matrix = mvp.model;
}
void PyramidRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_MVP mvp;

	mvp.model = obj.model_matrix * glm::rotate(glm::mat4(1.0f), (float)myTime.Delta() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mvp.view = camera.view_inverse;
	mvp.projection = camera.perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];

	obj.model_matrix = mvp.model;

	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}

void SunRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_MVP mvp;

	float earth_rotation_time = (float)myTime.Delta() * glm::radians(72.0f); //1 Second = 72 Degrees Rotation. 5 Seconds = 360 Degrees Rotation. 1 Earth Day = 5 Seconds Simulation
	float planet_rotation = 0.0f;
	float sun_rotation = 0.0f;

	planet_rotation = earth_rotation_time * 0.37037f;
	sun_rotation = 0;

	if (InputController::stop_rot)
	{
		planet_rotation *= 365.0f;
		sun_rotation *= 365.0f;
	}

	//Rotation around the Sun (Origin)
	mvp.model = glm::rotate(glm::mat4(1.0f), -sun_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	//Translation for model
	mvp.model *= obj.model_matrix;

	//Rotation on itself
	mvp.model *= glm::rotate(glm::mat4(1.0f), -planet_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	//Transposed-Inversed Model
	obj.model_matrix = mvp.model;

	mvp.view = camera.view_inverse;
	mvp.projection = camera.perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];

	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void MercuryRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 59.0f, 87.97f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void VenusRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 243.0f, 224.7f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void EarthRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 1.0f, 365.25f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void MarsRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 1.02749125f, 365.25f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void JupiterRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.4125f, 4330.6f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void SaturnRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.444f, 10755.7f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void UranusRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.71805f, 30687.0f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void NeptuneRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.67125f, 59757.8f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void PlutoRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 6.39f, 90520.0f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}

void PlanetaryRotation(float aspect_ratio, Uniform_Object &mvp, Object &obj, Camera &camera, const float &planet_rotation_earth_days, const float &sun_rotation_earth_days)
{
	float earth_rotation_time = (float)myTime.Delta() * glm::radians(72.0f); //1 Second = 72 Degrees Rotation. 5 Seconds = 360 Degrees Rotation. 1 Earth Day = 5 Seconds Simulation
	float planet_rotation = 0.0f;
	float sun_rotation = 0.0f;

	if (planet_rotation_earth_days == 0)
		planet_rotation = 0;
	else
		planet_rotation = earth_rotation_time / planet_rotation_earth_days;

	if (sun_rotation_earth_days == 0)
		sun_rotation = 0;
	else
		sun_rotation = earth_rotation_time / sun_rotation_earth_days;

	if (InputController::stop_rot)
	{
		planet_rotation *= 365.0f;
		sun_rotation *= 365.0f;
	}

	//Rotation around the Sun (Origin)
	mvp.model = glm::rotate(glm::mat4(1.0f), -sun_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	//Translation for model
	mvp.model *= obj.model_matrix;

	//Rotation on itself
	mvp.model *= glm::rotate(glm::mat4(1.0f), -planet_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	//Transposed-Inversed Model
	mvp.model_transposed_inversed = glm::transpose(glm::inverse(mvp.model));
	obj.model_matrix = mvp.model;

	mvp.view = camera.view_inverse;
	mvp.projection = camera.perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];
	mvp.light_color = glm::vec4(1.0f, 1.0f, 1.0f, 0.25f);
}

void skybox_uniform(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_MVP mvp;
	mvp.model = glm::mat4(1.0f);
	mvp.view = camera.view_inverse;
	mvp.view[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	mvp.projection = glm::infinitePerspective(glm::radians(45.0f), context.swapchain.swapchain_aspect, 0.1f);
	mvp.projection[1][1] = -mvp.projection[1][1];

	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}

void write_to_buffer(const VkDevice &device, const uint32_t &current_image, std::vector<VkDeviceMemory> &uniform_memory, const Uniform_Object &mvp)
{
	void* data;
	vkMapMemory(device, uniform_memory[current_image], 0, sizeof(Uniform_Object), 0, &data);
	memcpy(data, &mvp, sizeof(Uniform_Object));
	vkUnmapMemory(device, uniform_memory[current_image]);
}

void write_to_buffer(const VkDevice &device, const uint32_t &current_image, std::vector<VkDeviceMemory> &uniform_memory, const Uniform_MVP &mvp)
{
	void* data;
	vkMapMemory(device, uniform_memory[current_image], 0, sizeof(Uniform_MVP), 0, &data);
	memcpy(data, &mvp, sizeof(Uniform_MVP));
	vkUnmapMemory(device, uniform_memory[current_image]);
}