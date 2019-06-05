#include "Window.h"
#include "Assets.h"
#include "Object.h"
#include "texture_h/eyeball.h"

#define SKYBOX 0
#define SUN 1
#define MERCURY 2
#define VENUS 3
#define EARTH 4
#define MARS 5
#define JUPITER 6
#define SATURN 7
#define URANUS 8
#define NEPTUNE 9
#define PLUTO 10

#define SIZE_SPHERE			79.1f
#define SIZE_SUN			SIZE_SPHERE / 139.2f
#define SIZE_MERCURY		SIZE_SPHERE /  0.50f
#define SIZE_VENUS			SIZE_SPHERE /  1.20f
#define SIZE_EARTH			SIZE_SPHERE /  1.30f
#define SIZE_EARTHMOON		SIZE_SPHERE /  0.35f
#define SIZE_MARS			SIZE_SPHERE /  0.70f
#define SIZE_JUPITER		SIZE_SPHERE / 14.30f
#define SIZE_SATURN			SIZE_SPHERE / 12.50f
#define SIZE_URANUS			SIZE_SPHERE /  5.10f
#define SIZE_NEPTUNE		SIZE_SPHERE /  5.00f
#define SIZE_PLUTO			SIZE_SPHERE /  0.23f

#define DISTANCE_SUN						 0.00f
#define DISTANCE_MERCURY	SIZE_SPHERE +   35.98f
#define DISTANCE_VENUS		SIZE_SPHERE +   67.24f
#define DISTANCE_EARTH		SIZE_SPHERE +   92.96f
#define DISTANCE_MARS		SIZE_SPHERE +  141.60f
#define DISTANCE_JUPITER	SIZE_SPHERE +  483.80f
#define DISTANCE_SATURN		SIZE_SPHERE +  890.80f
#define DISTANCE_URANUS		SIZE_SPHERE + 1784.00f
#define DISTANCE_NEPTUNE	SIZE_SPHERE + 2793.00f
#define DISTANCE_PLUTO		SIZE_SPHERE + 3670.00f

#define GRID_LENGTH 10.0f
#define GRID_SUBLENGTH 1.0f

Object3D create_pyramid();
Object3D create_axe();
Object3D create_eyes();
Object3D create_sphere(const char* fbxfilepath, const char* texturelocation, Texture* texturedoth = nullptr, const float &scale_down = 10.0f, const glm::mat4 &model_matrix = glm::mat4(1.0f));
Object3D create_grid();
void AxeRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void PyramidRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);

void PlanetaryRotation(float aspect_ratio, Mvp_object &mvp, const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &perspective, const float &planet_rotation_earth_days, const float &sun_rotation_earth_days);
void SunRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void MercuryRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void VenusRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void EarthRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void MarsRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void JupiterRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void SaturnRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void UranusRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void NeptuneRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void PlutoRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void EarthMoonRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void skybox_uniform(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective);
void write_to_buffer(const VkDevice &device, const uint32_t &current_image, std::vector<VkDeviceMemory> &uniform_memory, const Mvp_object &mvp);

int main(int argc, char* args[])
{
	Window* myWindow = new Window(800, 600, "My New Window");

	Object3D pyramid = create_pyramid();
	Object3D eyes = create_eyes();
	Object3D axe = create_axe();
	Object3D grid = create_grid();
//	Object3D terrain = create_terrain();
	Object3D Skybox = Object3D(".\\assets\\solarsystem\\SpaceSkybox.fbx", ".\\assets\\solarsystem\\texture\\", nullptr, 0.1f, nullptr);
	Skybox.uniformFctn = skybox_uniform;

	float planetscale = 75.0f;
	std::vector<Object3D> Object_List;
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

	Object_List[SUN].world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_SUN, 0.0f, 0.0f));
	Object_List[SUN].uniformFctn = SunRotation;

	Object_List[MERCURY].world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_MERCURY, 0.0f, 0.0f));
	Object_List[MERCURY].uniformFctn = MercuryRotation;

	Object_List[VENUS].world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_VENUS, 0.0f, 0.0f));
	Object_List[VENUS].uniformFctn = VenusRotation;

	Object_List[EARTH].world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_EARTH, 0.0f, 0.0f));
	Object_List[EARTH].uniformFctn = EarthRotation;

	Object_List[MARS].world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_MARS, 0.0f, 0.0f));
	Object_List[MARS].uniformFctn = MarsRotation;

	Object_List[JUPITER].world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_JUPITER, 0.0f, 0.0f));
	Object_List[JUPITER].uniformFctn = JupiterRotation;

	Object_List[SATURN].world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_SATURN, 0.0f, 0.0f));
	Object_List[SATURN].uniformFctn = SaturnRotation;

	Object_List[URANUS].world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_URANUS, 0.0f, 0.0f));
	Object_List[URANUS].uniformFctn = UranusRotation;

	Object_List[NEPTUNE].world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_NEPTUNE, 0.0f, 0.0f));
	Object_List[NEPTUNE].uniformFctn = NeptuneRotation;

	Object_List[PLUTO].world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_PLUTO, 0.0f, 0.0f));
	Object_List[PLUTO].uniformFctn = PlutoRotation;

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

	pyramid.world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	pyramid.uniformFctn = PyramidRotation;

	return pyramid;
}
// Object3D create_two_squares()
// {
// 	std::vector<Vertex> two_squares_vertices = {
// 		{{-0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
// 		{{ 0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
// 		{{ 0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
// 		{{-0.5f,  0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
// 
// 		{{-0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
// 		{{ 0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
// 		{{ 0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
// 		{{-0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}
// 	};
// 
// 	const std::vector<uint32_t> two_squares_indices = {
// 	0, 1, 2,
// 	2, 3, 0,
// 	4, 5, 6,
// 	6, 7, 4
// 	};
// 
// 	Texture* celestial = new Texture(celestial_width, celestial_height, (void*)celestial_pixels, celestial_numlevels, false);
// 
// 	Object3D two_squares(two_squares_vertices, two_squares_indices, celestial);
// 	two_squares.uniformFctn = SquareFlag;
// 	return two_squares;
// }
Object3D create_axe()
{
	Object3D axe("Axe.fbx", nullptr, nullptr, 10);
	axe.world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	axe.uniformFctn = AxeRotation;
	return axe;
}
Object3D create_eyes()
{
	Texture* myEyeTexture = new Texture("assets\\misc\\Eye_D.jpg");

	uint32_t verts = sizeof(eyeball_data) / sizeof(_OBJ_VERT_);
	uint32_t inds = sizeof(eyeball_indicies) / sizeof(unsigned int);

	Object3D myEyes(eyeball_data, verts, eyeball_indicies, inds, myEyeTexture);
	myEyes.world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -3.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	myEyes.uniformFctn = PyramidRotation;

	return myEyes;
}
Object3D create_terrain()
{
	Object3D myTerrain("Axe.fbx"); //Placeholder
	
	return myTerrain;
}
Object3D create_grid()
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

	Texture* grid_texture = new Texture(".\\assets\\misc\\white_pixel.png");
	Object3D Grid(grid_vertices, grid_indices, grid_texture);

	Grid.world_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Grid.uniformFctn = AxeRotation;

	return Grid;

}
Object3D create_earth()
{
	Object3D myEarth("earth2.fbx", nullptr, nullptr, 75);
	myEarth.world_matrix = glm::mat4(1.0f);
	myEarth.uniformFctn = SunRotation;

	return myEarth;
}
Object3D create_sun()
{
	Object3D MySun(".\\assets\\solarsystem\\Sun.fbx", ".\\assets\\solarsystem\\texture\\", nullptr, 1.0f);
	MySun.world_matrix = glm::mat4(1.0f);
	MySun.uniformFctn = SunRotation;

	return MySun;
}

Object3D create_sphere(const char* fbxfilepath, const char* texturelocation, Texture* texturedoth, const float &scale_down, const glm::mat4 &model_matrix)
{
	Object3D mySphere(fbxfilepath, texturelocation, texturedoth, scale_down);
	mySphere.world_matrix = model_matrix;

	return mySphere;
}

void AxeRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

	Mvp_object mvp;

	mvp.model = model;
	mvp.view = view;
	//	mvp.projection = glm::perspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 20.0f);
	mvp.projection = perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];

	write_to_buffer(device, current_image, uniform_memory, mvp);
}
void PyramidRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

	Mvp_object mvp;

	mvp.model = model * glm::rotate(glm::mat4(1.0f), delta_time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mvp.view = view;
//	mvp.projection = glm::perspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 20.0f);
	mvp.projection = perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];
	write_to_buffer(device, current_image, uniform_memory, mvp);
}

void SunRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	PlanetaryRotation(swapchain_extent.width / (float)swapchain_extent.height, mvp, model, view, perspective, 27.0f, 0.0f);
	write_to_buffer(device, current_image, uniform_memory, mvp);
}
void MercuryRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	PlanetaryRotation(swapchain_extent.width / (float)swapchain_extent.height, mvp, model, view, perspective, 59.0f, 87.97f);
	write_to_buffer(device, current_image, uniform_memory, mvp);
}
void VenusRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	PlanetaryRotation(swapchain_extent.width / (float)swapchain_extent.height, mvp, model, view, perspective, 243.0f, 224.7f);
	write_to_buffer(device, current_image, uniform_memory, mvp);
}
void EarthRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	PlanetaryRotation(swapchain_extent.width / (float)swapchain_extent.height, mvp, model, view, perspective, 1.0f, 365.25f);
	write_to_buffer(device, current_image, uniform_memory, mvp);
}
void MarsRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	PlanetaryRotation(swapchain_extent.width / (float)swapchain_extent.height, mvp, model, view, perspective, 1.02749125f, 365.25f);
	write_to_buffer(device, current_image, uniform_memory, mvp);
}
void JupiterRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	PlanetaryRotation(swapchain_extent.width / (float)swapchain_extent.height, mvp, model, view, perspective, 0.4125f, 4330.6f);
	write_to_buffer(device, current_image, uniform_memory, mvp);
}
void SaturnRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	write_to_buffer(device, current_image, uniform_memory, mvp);
}
void UranusRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	PlanetaryRotation(swapchain_extent.width / (float)swapchain_extent.height, mvp, model, view, perspective, 0.71805f, 30687.0f);
	write_to_buffer(device, current_image, uniform_memory, mvp);
}
void NeptuneRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	PlanetaryRotation(swapchain_extent.width / (float)swapchain_extent.height, mvp, model, view, perspective, 0.67125f, 59757.8f);
	write_to_buffer(device, current_image, uniform_memory, mvp);
}
void PlutoRotation(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	PlanetaryRotation(swapchain_extent.width / (float)swapchain_extent.height, mvp, model, view, perspective, 6.39f, 90520.0f);
	write_to_buffer(device, current_image, uniform_memory, mvp);
}

void PlanetaryRotation(float aspect_ratio, Mvp_object &mvp, const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &perspective, const float &planet_rotation_earth_days, const float &sun_rotation_earth_days)
{
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

	float earth_rotation_time = delta_time * glm::radians(72.0f); //1 Second = 72 Degrees Rotation. 5 Seconds = 360 Degrees Rotation. 1 Earth Day = 5 Seconds Simulation
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
	mvp.model = glm::rotate(glm::mat4(1.0f), sun_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	//Translation for model
	mvp.model *= model;

	//Rotation on itself
	mvp.model *= glm::rotate(glm::mat4(1.0f), planet_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	mvp.view = view;
	mvp.projection = perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];
}

void skybox_uniform(const VkDevice& device, const VkExtent3D& swapchain_extent, const uint32_t& current_image, const glm::mat4 &model, std::vector<VkDeviceMemory> &uniform_memory, const glm::mat4 &view, const glm::mat4 &perspective)
{
	Mvp_object mvp;
	mvp.model = glm::mat4(1.0f);
	mvp.view = view;
	mvp.view[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	mvp.projection = glm::infinitePerspective(glm::radians(45.0f), swapchain_extent.width / (float)swapchain_extent.height, 0.1f);
	mvp.projection[1][1] = -mvp.projection[1][1];

	void* data;
	vkMapMemory(device, uniform_memory[current_image], 0, sizeof(Mvp_object), 0, &data);
	memcpy(data, &mvp, sizeof(Mvp_object));
	vkUnmapMemory(device, uniform_memory[current_image]);
}

void write_to_buffer(const VkDevice &device, const uint32_t &current_image, std::vector<VkDeviceMemory> &uniform_memory, const Mvp_object &mvp)
{
	void* data;
	vkMapMemory(device, uniform_memory[current_image], 0, sizeof(Mvp_object), 0, &data);
	memcpy(data, &mvp, sizeof(Mvp_object));
	vkUnmapMemory(device, uniform_memory[current_image]);
}
