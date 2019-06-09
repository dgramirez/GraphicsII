#include "pch.h"
#include "Camera.h"
#include "../planets.h"

void Camera::init(glm::mat4 translation, bool infinite_perspective, float rotation, float fov, float _near , float _far)
{
	prv_View = translation * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f + rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	prv_ViewInv = glm::inverse(prv_View);

	prv_Fov = glm::radians(fov);
	prv_Nearplane = _near;
	prv_Farplane = _far;
	prv_Viewspd = 1.0f;
	prv_InfinitePerspective = infinite_perspective;

	if (prv_InfinitePerspective)
	{
		prv_Farplane = HUGE_VALF;
		prv_Perspective = glm::infinitePerspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane);
	}
	else
	{
		prv_Farplane = _far;
		prv_Perspective = glm::perspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
	}

}

void Camera::set_object_list(std::vector<Object*> *object_list)
{
	pObjectList = object_list;
}

void Camera::set_planet_lookup(const uint32_t &x)
{
	if (x > PLUTO)
		prv_PlanetLookup = MERCURY;
	else if (x < MERCURY)
		prv_PlanetLookup = PLUTO;
	else
		prv_PlanetLookup = x;

	switch (prv_PlanetLookup)
	{
	case MERCURY:
		prv_PlanetName = "Mercury";
		break;
	case VENUS:
		prv_PlanetName = "Venus";
		break;
	case EARTH:
		prv_PlanetName = "Earth";
		break;
	case MARS:
		prv_PlanetName = "Mars";
		break;
	case JUPITER:
		prv_PlanetName = "Jupiter";
		break;
	case SATURN:
		prv_PlanetName = "Saturn";
		break;
	case URANUS:
		prv_PlanetName = "Uranus";
		break;
	case NEPTUNE:
		prv_PlanetName = "Neptune";
		break;
	case PLUTO:
		prv_PlanetName = "Pluto";
		break;
	}
}

void Camera::update(const SDL_Event &e)
{
	Update_FunctionButtons(e);
	Update_CommandButtons(e);
	Update_CameraMovement(e);
}

void Camera::Update_FunctionButtons(const SDL_Event &e)
{
	SDL_Keycode key = e.key.keysym.sym;

	if (e.type == SDL_KEYUP)
	{
		if (key == SDLK_F1 && !f1_released)
		{
			prv_InfinitePerspective = !prv_InfinitePerspective;
			prv_Nearplane = 0.1f;

			if (prv_InfinitePerspective)
			{
				prv_Farplane = HUGE_VALF;
				prv_Perspective = glm::infinitePerspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane);
			}
			else
			{
				prv_Farplane = 50.0f;
				prv_Perspective = glm::perspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
			}

			f1_released = true;
		}

		if (key == SDLK_F2 && !f2_released)
		{
			set_planet_lookup(planet_lookup + 1);
			f2_released = true;
		}

		if (key == SDLK_t && !t_released)
		{
			Object* obj = pObjectList->at(prv_PlanetLookup);
			glm::mat4 planet_model_matrix = obj->model_matrix;

			LookAtPlanet();
			float spacing = SIZE_SPHERE / obj->scale;
			prv_View[3] = planet_model_matrix[3];
			prv_View = glm::translate(prv_View, glm::vec3(0.0f, 0.0f, 10.0f * spacing));
			prv_ViewInv = glm::inverse(prv_View);

			t_released = true;
		}
	}

	if (e.type == SDL_KEYDOWN)
	{
		if (key == SDLK_F1 && f1_released)
			f1_released = false;
		if (key == SDLK_F2 && f2_released)
			f2_released = false;
		if (key == SDLK_t && t_released)
			t_released = false;

		if (e.key.keysym.sym == SDLK_EQUALS)
			prv_Viewspd += 0.25f;

		if (e.key.keysym.sym == SDLK_MINUS)
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
			LookAtPlanet();
	}
	if (InputController::Lalt)
	{
		if (InputController::zCommand)
		{
			prv_Fov -= glm::radians(5.0f * (float)myTime.Delta());
			if (prv_Fov < prv_FovMinMax.x)
				prv_Fov = prv_FovMinMax.x;

			if (prv_InfinitePerspective)
				prv_Perspective = glm::infinitePerspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane);
			else
				prv_Perspective = glm::perspective(prv_Fov, myContext.swapchain.swapchain_aspect, prv_Nearplane, prv_Farplane);
		}

		if (InputController::xCommand)
		{
			prv_Fov += glm::radians(5.0f * (float)myTime.Delta());
			if (prv_Fov > prv_FovMinMax.y)
				prv_Fov = prv_FovMinMax.y;

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

void Camera::LookAtPlanet()
{
	Object* obj = pObjectList->at(prv_PlanetLookup);
	glm::mat4 planet_model_matrix = obj->model_matrix;
	glm::vec3 eye = { prv_View[3].x, prv_View[3].y, prv_View[3].z };
	glm::vec3 center = { planet_model_matrix[3].x, planet_model_matrix[3].y, planet_model_matrix[3].z };
	glm::vec3 up = { 0.0f, -1.0f, 0.0f };
	prv_ViewInv = glm::lookAt(eye, center, up);
	prv_View = inverse(prv_ViewInv);
}
