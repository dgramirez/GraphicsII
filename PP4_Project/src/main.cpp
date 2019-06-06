#include "Window.h"
#include "objects/base_files/Object.h"
#include "objects/Camera.h"

Object* create_pyramid();
Object* create_axe();
Object* create_eyes();
Object* create_sphere(const char* fbxfilepath, const char* texturelocation, Texture* texturedoth = nullptr, const float &scale_down = 10.0f, const glm::mat4 &model_matrix = glm::mat4(1.0f));
Object* create_grid();

void AxeRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void PyramidRotation(const VkObj_Context &context, Object &obj, Camera &camera);

void PlanetaryRotation(float aspect_ratio, Mvp_object &mvp, Object &obj, Camera &camera, const float &planet_rotation_earth_days, const float &sun_rotation_earth_days);
void SunRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void MercuryRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void VenusRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void EarthRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void MarsRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void JupiterRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void SaturnRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void UranusRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void NeptuneRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void PlutoRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void EarthMoonRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void skybox_uniform(const VkObj_Context &context, Object &obj, Camera &camera);
void write_to_buffer(const VkDevice &device, const uint32_t &current_image, std::vector<VkDeviceMemory> &uniform_memory, const Mvp_object &mvp);

int main(int argc, char* args[])
{
	Window* myWindow = new Window(800, 600, "My New Window");

	Object *pyramid = create_pyramid();
	Object *eyes = create_eyes();
	Object *axe = create_axe();
	Object *grid = create_grid();
//	Object terrain = create_terrain();
	Object *Skybox = new Object(".\\assets\\solarsystem\\SpaceSkybox.fbx", ".\\assets\\solarsystem\\texture\\", 0.1f);
	Skybox->uniform_function = skybox_uniform;

	std::vector<Object*> Object_List;
	Object_List.push_back(Skybox);
	Object_List.push_back(create_sphere(".\\assets\\solarsystem\\Sun.fbx",		".\\assets\\solarsystem\\texture\\", nullptr, SIZE_SUN));
	Object_List.push_back(create_sphere(".\\assets\\solarsystem\\Mercury.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_MERCURY));
	Object_List.push_back(create_sphere(".\\assets\\solarsystem\\Venus.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_VENUS));
	Object_List.push_back(create_sphere(".\\assets\\solarsystem\\Earth.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_EARTH));
	Object_List.push_back(create_sphere(".\\assets\\solarsystem\\Mars.fbx",		".\\assets\\solarsystem\\texture\\", nullptr, SIZE_MARS));
	Object_List.push_back(create_sphere(".\\assets\\solarsystem\\Jupiter.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_JUPITER));
	Object_List.push_back(create_sphere(".\\assets\\solarsystem\\Saturn.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_SATURN));
	Object_List.push_back(create_sphere(".\\assets\\solarsystem\\Uranus.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_URANUS));
	Object_List.push_back(create_sphere(".\\assets\\solarsystem\\Neptune.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_NEPTUNE));
	Object_List.push_back(create_sphere(".\\assets\\solarsystem\\Pluto.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_PLUTO));

	Object_List[SUN]->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_SUN, 0.0f, 0.0f));
	Object_List[SUN]->uniform_function = SunRotation;

	Object_List[MERCURY]->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_MERCURY, 0.0f, 0.0f));
	Object_List[MERCURY]->uniform_function = MercuryRotation;

	Object_List[VENUS]->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_VENUS, 0.0f, 0.0f));
	Object_List[VENUS]->uniform_function = VenusRotation;

	Object_List[EARTH]->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_EARTH, 0.0f, 0.0f));
	Object_List[EARTH]->uniform_function = EarthRotation;

	Object_List[MARS]->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_MARS, 0.0f, 0.0f));
	Object_List[MARS]->uniform_function = MarsRotation;

	Object_List[JUPITER]->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_JUPITER, 0.0f, 0.0f));
	Object_List[JUPITER]->uniform_function = JupiterRotation;

	Object_List[SATURN]->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_SATURN, 0.0f, 0.0f));
	Object_List[SATURN]->uniform_function = SaturnRotation;

	Object_List[URANUS]->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_URANUS, 0.0f, 0.0f));
	Object_List[URANUS]->uniform_function = UranusRotation;

	Object_List[NEPTUNE]->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_NEPTUNE, 0.0f, 0.0f));
	Object_List[NEPTUNE]->uniform_function = NeptuneRotation;

	Object_List[PLUTO]->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_PLUTO, 0.0f, 0.0f));
	Object_List[PLUTO]->uniform_function = PlutoRotation;

	Object_List.push_back(eyes);
	Object_List.push_back(axe);
	Object_List.push_back(grid);

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

	LOG("Deleting objects");
	for (uint32_t i = 0; i < Object_List.size(); ++i)
		delete Object_List[i];
	
#if _DEBUG
	LOG("\n\nPress the enter key once or twice to exit.")
	std::cin.ignore().get();
#endif

	return 0;
}

Object* create_pyramid()
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

	Object *pyramid = new Object(pyramid_vertex, pyramid_index, celestial);

	pyramid->texture->swizzle(VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A, VK_COMPONENT_SWIZZLE_R);

	pyramid->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	pyramid->uniform_function = PyramidRotation;

	return pyramid;
}
Object* create_axe()
{
	Object *axe = new Object("Axe.fbx","", 10.0f);
	axe->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
	axe->uniform_function = AxeRotation;
	return axe;
}
Object* create_eyes()
{
	Texture* myEyeTexture = new Texture("assets\\misc\\Eye_D.jpg");

	uint32_t verts = sizeof(eyeball_data) / sizeof(_OBJ_VERT_);
	uint32_t inds = sizeof(eyeball_indicies) / sizeof(unsigned int);

	Object *myEyes = new Object(eyeball_data, verts, eyeball_indicies, inds, myEyeTexture);
	myEyes->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -3.0f));
	myEyes->uniform_function = PyramidRotation;

	return myEyes;
}
Object* create_grid()
{
	std::vector<Vertex> pyramid_vertex = {
	{ { 0.0f, -0.5f,  0.0f, 1.0f}, V_COLOR_RED	 , {0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Top point
	{ { 0.5f,  0.5f,  0.5f, 1.0f}, V_COLOR_GREEN , {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 1
	{ {-0.5f,  0.5f,  0.5f, 1.0f}, V_COLOR_BLUE  , {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 2
	{ { 0.5f,  0.5f, -0.5f, 1.0f}, V_COLOR_CYAN  , {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 3
	{ {-0.5f,  0.5f, -0.5f, 1.0f}, V_COLOR_YELLOW, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }	//Bottom Vertex 4;
	};

	std::vector<Vertex> grid_vertices;
	std::vector<uint32_t> grid_indices;
	uint32_t index = 0;
	for (float i = -GRID_LENGTH; i <= GRID_LENGTH; i += GRID_SUBLENGTH)
	{
		//Setup 
		grid_vertices.resize(grid_vertices.size() + 8);

		//Vertical
		grid_vertices[index + 0] = { { i + 0.0f, 0.0f, -GRID_LENGTH, 1.0f}, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 1] = { { i + 0.1f, 0.0f, -GRID_LENGTH, 1.0f}, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 2] = { { i + 0.0f, 0.0f,  GRID_LENGTH, 1.0f}, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 3] = { { i + 0.1f, 0.0f,  GRID_LENGTH, 1.0f}, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_indices.push_back(index + 0);
		grid_indices.push_back(index + 1);
		grid_indices.push_back(index + 2);
		grid_indices.push_back(index + 1);
		grid_indices.push_back(index + 3);
		grid_indices.push_back(index + 2);

		//Horizontal
		grid_vertices[index + 4] = { { -GRID_LENGTH, 0.0f, i + 0.0f, 1.0f}, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 5] = { { -GRID_LENGTH, 0.0f, i + 0.1f, 1.0f}, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 6] = { {  GRID_LENGTH, 0.0f, i + 0.0f, 1.0f}, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		grid_vertices[index + 7] = { {  GRID_LENGTH, 0.0f, i + 0.1f, 1.0f}, V_COLOR_WHITE , {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
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

	Grid->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f,0.0f,1.0f));
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
	Mvp_object mvp;

	mvp.model = obj.model_matrix;
	mvp.view = camera.view_inverse;
	//	mvp.projection = glm::perspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 20.0f);
	mvp.projection = camera.perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];

	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void PyramidRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;

	mvp.model = obj.model_matrix * glm::rotate(glm::mat4(1.0f), (float)myTime.Delta() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mvp.view = camera.view_inverse;
//	mvp.projection = glm::perspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 20.0f);
	mvp.projection = camera.perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];

	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}

void SunRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 27.0f, 0.0f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void MercuryRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 59.0f, 87.97f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void VenusRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 243.0f, 224.7f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void EarthRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 1.0f, 365.25f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void MarsRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 1.02749125f, 365.25f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void JupiterRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.4125f, 4330.6f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void SaturnRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.444f, 10755.7f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void UranusRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.71805f, 30687.0f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void NeptuneRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.67125f, 59757.8f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void PlutoRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 6.39f, 90520.0f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}

void PlanetaryRotation(float aspect_ratio, Mvp_object &mvp, Object &obj, Camera &camera, const float &planet_rotation_earth_days, const float &sun_rotation_earth_days)
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

	obj.model_matrix = mvp.model;

	mvp.view = camera.view_inverse;
	mvp.projection = camera.perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];
}

void skybox_uniform(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Mvp_object mvp;
	mvp.model = glm::mat4(1.0f);
	mvp.view = camera.view_inverse;
	mvp.view[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	mvp.projection = glm::infinitePerspective(glm::radians(45.0f), context.swapchain.swapchain_aspect, 0.1f);
	mvp.projection[1][1] = -mvp.projection[1][1];

	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}

void write_to_buffer(const VkDevice &device, const uint32_t &current_image, std::vector<VkDeviceMemory> &uniform_memory, const Mvp_object &mvp)
{
	void* data;
	vkMapMemory(device, uniform_memory[current_image], 0, sizeof(Mvp_object), 0, &data);
	memcpy(data, &mvp, sizeof(Mvp_object));
	vkUnmapMemory(device, uniform_memory[current_image]);
}
