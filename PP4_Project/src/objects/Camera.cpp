#include "Camera.h"

void Camera::init(glm::mat4 translation, glm::mat4 rotation, bool infinite_perspective /*= true*/, float fov /*= 45.0f*/, float _near /*= 0.1*/, float _far /*= 50.0f*/)
{
	prv_View = glm::mat4(1.0f);
	prv_ViewInv = glm::inverse(prv_View);

	fov = fov;
	prv_Nearplane = _near;
	prv_Farplane = _far;
	prv_Viewspd = 1.0f;
	prv_InfinitePerspective = infinite_perspective;

	if (prv_InfinitePerspective)
		prv_Perspective = glm::infinitePerspective(fov, myContext.swapchain.swapchain_aspect, prv_Nearplane);
	else
		prv_Perspective = glm::perspective(fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
}

void Camera::update(const SDL_Event &e)
{
	Update_FunctionButtons(e);
	Update_CommandButtons(e);
	Update_CameraMovement(e);
}

void Camera::Update_FunctionButtons(const SDL_Event &e)
{
	if (e.type == SDL_KEYDOWN)
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_F1)
		{
			prv_InfinitePerspective = true;
			prv_Nearplane = 0.1f;
			prv_Perspective = glm::infinitePerspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane);
		}

		if (key == SDLK_F2)
		{
			prv_InfinitePerspective = false;
			prv_Nearplane = 0.1f;
			prv_Farplane = 100.0f;
			prv_Perspective = glm::perspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
		}

		if (e.key.keysym.sym == SDLK_KP_PLUS)
		{
			prv_Viewspd += 0.25f;
		}

		if (e.key.keysym.sym == SDLK_KP_MINUS)
		{
			prv_Viewspd -= 0.25f;
			if (prv_Viewspd < 0.25f)
				prv_Viewspd = 0.25f;
		}

	}
}

void Camera::Update_CommandButtons(const SDL_Event &e)
{
	if (InputController::Lshift)
	{
		if (InputController::zCommand)
		{
			prv_Nearplane -= 5.0f * (float)myTime.Delta();

			if (prv_Nearplane < 0.1f)
				prv_Nearplane = 0.1f;

			if (prv_InfinitePerspective)
				prv_Perspective = glm::infinitePerspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane);
			else
				prv_Perspective = glm::perspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
		}

		if (InputController::xCommand)
		{
			prv_Nearplane += 5.0f * (float)myTime.Delta();

			if (prv_Nearplane > prv_Farplane * 0.9f)
				prv_Nearplane = prv_Farplane * 0.9f;

			if (prv_InfinitePerspective)
				prv_Perspective = glm::infinitePerspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane);
			else
				prv_Perspective = glm::perspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
		}
	}
	if (InputController::Lctrl && !prv_InfinitePerspective)
	{
		if (InputController::zCommand)
		{
			prv_Farplane -= 5.0f * (float)myTime.Delta();

			if (prv_Farplane * 0.9f < prv_Nearplane)
				prv_Farplane += 1.0f * (float)myTime.Delta();

			prv_Perspective = glm::perspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
		}

		if (InputController::xCommand)
		{
			prv_Farplane += 5.0f * (float)myTime.Delta();

			if (prv_Farplane > 10000.0f)
				prv_Farplane = 10000.0f;

			prv_Perspective = glm::perspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
		}
	}
	if (InputController::Lctrl)
	{
		if (InputController::findpluto)
		{
			glm::vec3 eye = { prv_View[3].x, prv_View[3].y, prv_View[3].z };
			glm::vec3 center = { 0.0f, 0.0f, 0.0f };
			glm::vec3 up = { 0.0f, 0.0f, 1.0f };
			prv_ViewInv = glm::lookAt(eye, center, up);
			prv_View = inverse(prv_ViewInv);
		}
	}
	if (InputController::Lalt)
	{
		if (InputController::zCommand)
		{
			prv_Fov -= glm::radians(5.0f * (float)myTime.Delta());
			if (prv_Fov < 45.0f)
				prv_Fov = 45.0f;

			if (prv_InfinitePerspective)
				prv_Perspective = glm::infinitePerspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane);
			else
				prv_Perspective = glm::perspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
		}

		if (InputController::xCommand)
		{
			prv_Fov += glm::radians(5.0f * (float)myTime.Delta());
			if (prv_Fov > 175.0f)
				prv_Fov = 175.0f;

			if (prv_InfinitePerspective)
				prv_Perspective = glm::infinitePerspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane);
			else
				prv_Perspective = glm::perspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
		}
	}
}

void Camera::Update_CameraMovement(const SDL_Event &e)
{
	float rotspeed = float(glm::radians(60.0f) * myTime.SmoothDelta());
	float movspd = prv_Viewspd * float(5.0f * myTime.SmoothDelta());

	if (InputController::r_negYaw)
	{
		prv_View = glm::rotate(prv_View, rotspeed, glm::vec3(0, 1, 0));
		prv_ViewInv = glm::inverse(prv_View);
	}
	if (InputController::r_posYaw)
	{
		prv_View = glm::rotate(prv_View, -rotspeed, glm::vec3(0, 1, 0));
		prv_ViewInv = glm::inverse(prv_View);
	}
	if (InputController::r_posPitch)
	{
		prv_View = glm::rotate(prv_View, rotspeed, glm::vec3(1, 0, 0));
		prv_ViewInv = glm::inverse(prv_View);
	}
	if (InputController::r_negPitch)
	{
		prv_View = glm::rotate(prv_View, -rotspeed, glm::vec3(1, 0, 0));
		prv_ViewInv = glm::inverse(prv_View);
	}
	if (InputController::r_negRoll)
	{
		prv_View = glm::rotate(prv_View, rotspeed, glm::vec3(0, 0, 1));
		prv_ViewInv = glm::inverse(prv_View);
	}
	if (InputController::r_posRoll)
	{
		prv_View = glm::rotate(prv_View, -rotspeed, glm::vec3(0, 0, 1));
		prv_ViewInv = glm::inverse(prv_View);
	}


	if (InputController::m_forward)
	{
		prv_View = glm::translate(prv_View, glm::vec3(0.0f, 0.0f, -movspd));
		prv_ViewInv = glm::inverse(prv_View);
	}
	if (InputController::m_back)
	{
		prv_View = glm::translate(prv_View, glm::vec3(0.0f, 0.0f, movspd));
		prv_ViewInv = glm::inverse(prv_View);
	}
	if (InputController::m_left)
	{
		prv_View = glm::translate(prv_View, glm::vec3(-movspd, 0.0f, 0.0f));
		prv_ViewInv = glm::inverse(prv_View);
	}
	if (InputController::m_right)
	{
		prv_View = glm::translate(prv_View, glm::vec3(movspd, 0.0f, 0.0f));
		prv_ViewInv = glm::inverse(prv_View);
	}
	if (InputController::m_up)
	{
		prv_View = glm::translate(prv_View, glm::vec3(0.0f, movspd, 0.0f));
		prv_ViewInv = glm::inverse(prv_View);
	}
	if (InputController::m_down)
	{
		prv_View = glm::translate(prv_View, glm::vec3(0.0f, -movspd, 0.0f));
		prv_ViewInv = glm::inverse(prv_View);
	}
}
