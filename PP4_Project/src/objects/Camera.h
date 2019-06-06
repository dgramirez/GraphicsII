#ifndef CAMERA_H
#define CAMERA_H

#include "../Defines.h"
#include "../VulkanObj/VkObj_Context.h"
#include "../InputController.h"

class Camera
{
public:
	Camera() = default;
	void init(glm::mat4 translation, glm::mat4 rotation, bool infinite_perspective = true, float zoom = 45.0f, float _near = 0.1, float _far = 50.0f);
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

private:
	glm::mat4 prv_View;
	glm::mat4 prv_ViewInv;
	glm::mat4 prv_Perspective;

	void Update_FunctionButtons(const SDL_Event &e);
	void Update_CommandButtons(const SDL_Event &e);
	void Update_CameraMovement(const SDL_Event &e);

	bool  prv_InfinitePerspective;
	float prv_Fov;
	float prv_Nearplane;
	float prv_Farplane;
	float prv_Viewspd;
};

#endif