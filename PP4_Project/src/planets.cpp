#include "pch.h"
#include "planets.h"
#include "objects/Camera.h"
#include "texture_h/celestial.h"
#include "texture_h/axeTexture.h"
#include "texture_h/fighter.h"

float flagtime = 0.0f;

Object* create_pyramid()
{
	std::vector<Vertex> pyramid_vertex = {
		{ { 0.0f, -0.5f,  0.0f, 1.0f }, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_RED	,	{0.5f, 0.0f} },	//Top point
		{ { 0.5f,  0.5f,  0.5f, 1.0f }, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_GREEN ,	{1.0f, 1.0f} },	//Bottom Vertex 1
		{ {-0.5f,  0.5f,  0.5f, 1.0f }, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_BLUE  ,	{0.0f, 1.0f} },	//Bottom Vertex 2
		{ { 0.5f,  0.5f, -0.5f, 1.0f }, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_CYAN  ,	{0.0f, 1.0f} },	//Bottom Vertex 3
		{ {-0.5f,  0.5f, -0.5f, 1.0f }, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_YELLOW,	{1.0f, 1.0f} }	//Bottom Vertex 4;
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
	Object *Ship = new Object("assets\\Trident-A10.fmd", ".\\assets\\misc\\Trident_UV_Dekol_Color.png");
	Ship->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -2.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Ship->uniform_function = AxeRotation;
	return Ship;
}
Object* create_fighter_ship()
{
	LOG("Creating Fighter Plane")
	Texture* myFighterTexture = new Texture("assets\\misc\\fighter_texture.jpg");

	uint32_t verts = sizeof(fighter_data) / sizeof(_OBJ_VERT_);
	uint32_t inds = sizeof(fighter_indicies) / sizeof(unsigned int);
	LOG("Fighter Plane Vertices: " << verts)
	LOG("Fighter Plane Indices: " << inds)

	Object *myFighter = new Object(fighter_data, verts, fighter_indicies, inds, myFighterTexture);
	myFighter->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(165.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	myFighter->uniform_function = AxeRotation;
	LOG("Fighter Plane Finished" << std::endl)

	return myFighter;
}
Object* create_grid()
{
	std::vector<Vertex> grid_vertices;
	std::vector<uint32_t> grid_indices;
	uint32_t index = 0;

	LOG("Creating Grid")
	for (float i = -GRID_LENGTH; i <= GRID_LENGTH; i += GRID_SUBLENGTH)
	{
		//Setup 
		grid_vertices.resize(grid_vertices.size() + 8);

		//Vertical
		grid_vertices[index + 0] = { { i + 0.0f, 0.0f, -GRID_LENGTH, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_WHITE, {0.0f, 0.0f} };
		grid_vertices[index + 1] = { { i + 0.1f, 0.0f, -GRID_LENGTH, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_WHITE, {0.0f, 0.0f} };
		grid_vertices[index + 2] = { { i + 0.0f, 0.0f,  GRID_LENGTH, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_WHITE, {0.0f, 0.0f} };
		grid_vertices[index + 3] = { { i + 0.1f, 0.0f,  GRID_LENGTH, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_WHITE, {0.0f, 0.0f} };
		grid_indices.push_back(index + 0);
		grid_indices.push_back(index + 1);
		grid_indices.push_back(index + 2);
		grid_indices.push_back(index + 1);
		grid_indices.push_back(index + 3);
		grid_indices.push_back(index + 2);

		//Horizontal
		grid_vertices[index + 4] = { { -GRID_LENGTH, 0.0f, i + 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_WHITE, {0.0f, 0.0f} };
		grid_vertices[index + 5] = { { -GRID_LENGTH, 0.0f, i + 0.1f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_WHITE, {0.0f, 0.0f} };
		grid_vertices[index + 6] = { {  GRID_LENGTH, 0.0f, i + 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_WHITE, {0.0f, 0.0f} };
		grid_vertices[index + 7] = { {  GRID_LENGTH, 0.0f, i + 0.1f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, V_COLOR_WHITE, {0.0f, 0.0f} };
		grid_indices.push_back(index + 6);
		grid_indices.push_back(index + 7);
		grid_indices.push_back(index + 4);
		grid_indices.push_back(index + 7);
		grid_indices.push_back(index + 5);
		grid_indices.push_back(index + 4);
		index += 8;
	}
	LOG("Grid Vertices: " << grid_vertices.size())
	LOG("Grid Indices: " << grid_indices.size())

	Texture *grid_texture = new Texture(".\\assets\\misc\\white_pixel.png");
	Object *Grid = new Object(grid_vertices, grid_indices, grid_texture);

	Grid->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Grid->uniform_function = UniformMVP_Basic;

	LOG("Finished Grid" << std::endl)
	return Grid;
}
Object* create_square(const char* texture, float x1, float x2, float y1, float y2)
{
	std::vector<Vertex> square_vertices =
	{
		{ { x1, 0.0f, y1, 1.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, V_COLOR_WHITE, {0.0f, 0.0f} },
		{ { x1, 0.0f, y2, 1.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, V_COLOR_WHITE, {1.0f, 0.0f} },
		{ { x2, 0.0f, y1, 1.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, V_COLOR_WHITE, {0.0f, 1.0f} },
		{ { x2, 0.0f, y2, 1.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, V_COLOR_WHITE, {1.0f, 1.0f} }
	};

	std::vector<uint32_t> square_indices = 
	{
		0, 1, 2,
		1, 3, 2
	};

	Texture *square_texture = new Texture(texture);
	Object *Square = new Object(square_vertices, square_indices, square_texture);

	Square->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Square->uniform_function = AxeRotation;

	return Square;
}
Object* create_sphere(const char* fbmfilepath, const char* texturelocation, const char *normal_filepath)
{
	Object *mySphere = new Object(fbmfilepath, texturelocation, normal_filepath);
	return mySphere;
}
Object* create_ball()
{
	Object *Ship = new Object("assets\\Circle.fmd", ".\\assets\\misc\\white_pixel.png");
	Ship->model_matrix = glm::mat4(1.0f);
	Ship->uniform_function = BallMVP_Basic;
	return Ship;
}
Object* create_flag()
{
	Object *Flag = new Object("assets\\Flag.fmd", ".\\assets\\misc\\japanese_flag.png");
	Flag->model_matrix = glm::mat4(1.0f);
	Flag->uniform_function = flag_uniform;
	return Flag;
}

void UniformMVP_Basic(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_MVP mvp;

	mvp.model = obj.model_matrix;
	mvp.view = camera.view_inverse;
	mvp.projection = camera.perspective;
	mvp.projection[1][1] = -mvp.projection[1][1];
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);

	obj.model_matrix = mvp.model;
}
void BallMVP_Basic(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_MVP mvp;

	mvp.model = obj.model_matrix;
	mvp.view = camera.view_inverse;
	mvp.projection = glm::mat4(1.0f);
	mvp.projection[1][1] = -mvp.projection[1][1];
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);

	obj.model_matrix = mvp.model;
}

void AxeRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	UBO_Ships ubo;

	ubo.mvp.model = obj.model_matrix;
	ubo.mvp.view = camera.view_inverse;
	ubo.mvp.projection = camera.perspective;
	ubo.mvp.projection[1][1] = -ubo.mvp.projection[1][1];

	ubo.TI_modelview = glm::transpose(glm::inverse(ubo.mvp.model));

	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f) * (float)myTime.SmoothDelta(), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.light1_pos = rot * camera.point_light;
	ubo.light1_color = camera.point_light_color;

	ubo.light2_pos = camera.dir_light;
	ubo.light2_color = camera.dir_light_color;

	ubo.light3_pos = camera.spot_light;
	ubo.light3_dir = camera.spot_light_dir;
	ubo.light3_color = camera.spot_light_color;

	ubo.strengths = glm::vec4(0.25f, 0.75f, camera.attenuation, 0.25f);
	ubo.cone_strength = camera.spot_light_strengths;

	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, ubo);

	obj.model_matrix = ubo.mvp.model;
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
	Uniform_Planets ubo;
//	PlanetaryRotation(context.swapchain.swapchain_aspect, ubo, obj, camera, 59.0f, 87.97f);

	//Rotation around the Sun (Origin)
	ubo.mvp.model = obj.model_matrix;
	
	//Transposed-Inversed Model
	ubo.TI_model = glm::transpose(glm::inverse(ubo.mvp.model));
	obj.model_matrix = ubo.mvp.model;

	ubo.mvp.view = camera.view_inverse;
	ubo.mvp.projection = camera.perspective;
	ubo.mvp.projection[1][1] = -ubo.mvp.projection[1][1];
	ubo.light_color = glm::vec4(1.0f, 1.0f, 1.0f, 0.25f);

	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, ubo);
}
void VenusRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Planets mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 243.0f, 224.7f, true);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void EarthRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Planets mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 1.0f, 365.25f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void MarsRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Planets mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 1.02749125f, 365.25f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void JupiterRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Planets mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.4125f, 4330.6f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void SaturnRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Planets mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.444f, 10755.7f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void UranusRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Planets mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.71805f, 30687.0f, true);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void NeptuneRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Planets mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 0.67125f, 59757.8f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}
void PlutoRotation(const VkObj_Context &context, Object &obj, Camera &camera)
{
	Uniform_Planets mvp;
	PlanetaryRotation(context.swapchain.swapchain_aspect, mvp, obj, camera, 6.39f, 90520.0f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, mvp);
}

void PlanetaryRotation(float aspect_ratio, Uniform_Planets &ubo, Object &obj, Camera &camera, const float &planet_rotation_earth_days, const float &sun_rotation_earth_days, bool planet_clockwise)
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

	if (planet_clockwise)
		planet_rotation = -planet_rotation;

	//Rotation around the Sun (Origin)
	ubo.mvp.model = glm::rotate(glm::mat4(1.0f), -sun_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	//Translation for model
	ubo.mvp.model *= obj.model_matrix;

	//Rotation on itself
	ubo.mvp.model *= glm::rotate(glm::mat4(1.0f), -planet_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	//Transposed-Inversed Model
	ubo.TI_model = glm::transpose(glm::inverse(ubo.mvp.model));
	obj.model_matrix = ubo.mvp.model;

	ubo.mvp.view = camera.view_inverse;
	ubo.mvp.projection = camera.perspective;
	ubo.mvp.projection[1][1] = -ubo.mvp.projection[1][1];
	ubo.light_color = glm::vec4(1.0f, 1.0f, 1.0f, 0.25f);
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

void flag_uniform(const VkObj_Context &context, Object &obj, Camera &camera)
{
	flagtime += (float)myTime.SmoothDelta() * 3.0f;

	UBO_Flag ubo;

	ubo.mvp.model = obj.model_matrix;
	ubo.mvp.view = camera.view_inverse;
	ubo.mvp.projection = camera.perspective;
	ubo.mvp.projection[1][1] = -ubo.mvp.projection[1][1];
	ubo.info = glm::vec4(flagtime, 0.0f, 0.0f, 0.0f);
	write_to_buffer(context.device.logical, context.swapchain.image_index, obj.uniform_memory, ubo);

	obj.model_matrix = ubo.mvp.model;
}