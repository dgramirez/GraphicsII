#ifndef PLANETS_H
#define PLANETS_H

#pragma region DEFINES
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
#define HFILE_SHIP 11
#define MODEL_SHIP 12
#define GRID 13
#define PYRAMID 14
#define SQUARE 15
#define FLAG 16
#define BALL 17

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
#pragma endregion

#include "objects/base_files/Object.h"
class Camera;
class VkObj_Context;
struct Uniform_Planets;

#pragma region non-planets
Object* create_pyramid();
Object* create_normal_ship();
Object* create_fighter_ship();
Object* create_square(const char* texture, float x1, float x2, float y1, float y2);
Object* create_grid();
Object* create_flag();
Object* create_ball();
#pragma endregion


Object* create_sphere(const char* fbxfilepath, const char* texturelocation, Texture* texturedoth = nullptr, const float &scale_down = 10.0f, const glm::mat4 &model_matrix = glm::mat4(1.0f));

void UniformMVP_Basic(const VkObj_Context &context, Object &obj, Camera &camera);
void BallMVP_Basic(const VkObj_Context &context, Object &obj, Camera &camera);

void AxeRotation(const VkObj_Context &context, Object &obj, Camera &camera);
void PyramidRotation(const VkObj_Context &context, Object &obj, Camera &camera);

void PlanetaryRotation(float aspect_ratio, Uniform_Planets &ubo, Object &obj, Camera &camera, const float &planet_rotation_earth_days, const float &sun_rotation_earth_days, bool planet_clockwise = false);
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
void flag_uniform(const VkObj_Context &context, Object &obj, Camera &camera);
//void write_to_buffer(const VkDevice &device, const uint32_t &current_image, std::vector<VkDeviceMemory> &uniform_memory, const Uniform_Planets &ubo);
//void write_to_buffer(const VkDevice &device, const uint32_t &current_image, std::vector<VkDeviceMemory> &uniform_memory, const Uniform_MVP &mvp);

template <typename T>
void write_to_buffer(const VkDevice &device, const uint32_t &current_image, std::vector<VkDeviceMemory> &uniform_memory, const T &mvp)
{
	void* data;
	vkMapMemory(device, uniform_memory[current_image], 0, sizeof(T), 0, &data);
	memcpy(data, &mvp, sizeof(T));
	vkUnmapMemory(device, uniform_memory[current_image]);
}
#endif