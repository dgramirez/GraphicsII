#ifndef CAMERA_H
#define CAMERA_H

#include "../VulkanObj/VkObj_Context.h"
#include "../InputController.h"
#include "base_files/Object.h"

class Camera
{
public:
	Camera() = default;
	void init(glm::mat4 translation, bool infinite_perspective = true, float rotation = 0.0f, float zoom = 45.0f, float _near = 0.1, float _far = 50.0f);
	void set_object_list(std::vector<Object*> *object_list);
	void set_planet_lookup(const uint32_t &x);
	void update(const SDL_Event &e);

	glm::mat4 get_view() { return prv_View; }
	__declspec(property(get = get_view)) glm::mat4 view;

	glm::mat4 get_view_inverse() { return prv_ViewInv; }
	__declspec(property(get = get_view_inverse)) glm::mat4 view_inverse;

	glm::mat4 get_perspective() { return prv_Perspective; }
	__declspec(property(get = get_perspective)) glm::mat4 perspective;

	bool is_infinite_perspective() { return prv_InfinitePerspective; }
	__declspec(property(get = is_infinite_perspective)) bool infinite_perspective;

	float get_fov() { return glm::degrees(prv_Fov); }
	__declspec(property(get = get_fov)) float fov;

	float get_nearplane() { return prv_Nearplane; }
	__declspec(property(get = get_nearplane)) float nearplane;

	float get_farplane() { return prv_Farplane; }
	__declspec(property(get = get_farplane)) float farplane;

	float get_viewspd() { return prv_Viewspd; }
	__declspec(property(get = get_viewspd)) float viewspd;

	float get_attenuation() { return prv_Attenuation; }
	__declspec(property(get = get_attenuation)) float attenuation;

	uint32_t get_planet_lookup() { return prv_PlanetLookup; }
	__declspec(property(get = get_planet_lookup, put = set_planet_lookup)) uint32_t planet_lookup;

	const char* get_planet_name() { return prv_PlanetName; }
	__declspec(property(get = get_planet_name)) const char* planet_name;

	glm::vec4 get_dirlight() { return prv_DirLight; }
	__declspec(property(get = get_dirlight)) glm::vec4 dir_light;

	glm::vec4 get_dirlightcolor() { return prv_DirLightColor; }
	__declspec(property(get = get_dirlightcolor)) glm::vec4 dir_light_color;

	glm::vec4 get_pointlight() { return prv_PointLight; }
	__declspec(property(get = get_pointlight)) glm::vec4 point_light;

	glm::vec4 get_pointlightcolor() { return prv_PointLightColor; }
	__declspec(property(get = get_pointlightcolor)) glm::vec4 point_light_color;

	glm::vec4 get_spotlight() { return prv_SpotLight; }
	__declspec(property(get = get_spotlight)) glm::vec4 spot_light;

	glm::vec4 get_spotlightcolor() { return prv_SpotLightColor; }
	__declspec(property(get = get_spotlightcolor)) glm::vec4 spot_light_color;

	glm::vec4 get_spotlight_dir() { return prv_SpotLightDir; }
	__declspec(property(get = get_spotlight_dir)) glm::vec4 spot_light_dir;

	glm::vec4 get_spotlight_strength() { return prv_SpotLightStrengths; }
	__declspec(property(get = get_spotlight_strength)) glm::vec4 spot_light_strengths;

	std::vector<Object*>* get_object() { return pObjectList; }
	__declspec(property(get = get_object)) std::vector<Object*> *object_list;

private:
	glm::mat4 prv_View;
	glm::mat4 prv_ViewInv;
	glm::mat4 prv_Perspective;
	std::vector<Object*> *pObjectList;
	uint32_t prv_PlanetLookup;
	const char* prv_PlanetName;

	glm::vec4 prv_PointLight;
	glm::vec4 prv_PointLightColor;

	glm::vec4 prv_DirLight;
	glm::vec4 prv_DirLightColor;
	
	glm::vec4 prv_SpotLight;
	glm::vec4 prv_SpotLightDir;
	glm::vec4 prv_SpotLightColor;
	glm::vec4 prv_SpotLightStrengths;
	
	const float prv_DirLightDistance = 10.0f;
	float prv_DirLightY;

	void Update_FunctionButtons(const SDL_Event &e);
	void Update_CommandButtons(const SDL_Event &e);
	void Update_CameraMovement(const SDL_Event &e);
	void Normalize_View_Matrix();

	void LookAtPlanet();

	bool  prv_InfinitePerspective;
	float prv_Fov;
	const glm::vec2 prv_FovMinMax = { glm::radians(45.0f) , glm::radians(75.0f) };
	float prv_Nearplane;
	float prv_Farplane;
	float prv_Viewspd;
	float prv_Attenuation;
	bool f1_released = false;
	bool f2_released = false;
	bool t_released = false;
	bool normalize_view = false;
};

#endif