#include "pch.h"
#include "Window.h"
#include "planets.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main(int argc, char* args[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetBreakAlloc(209);

	Window* myWindow = new Window(1280, 720, "My New Window");

	Object *pyramid = create_pyramid();
	Object *fighter_ship = create_fighter_ship();
	Object *normal_ship = create_normal_ship();
	Object *grid = create_grid();
	Object *square = create_square();
	Object *Skybox = new Object(".\\assets\\solarsystem\\SpaceSkybox.fbx", ".\\assets\\solarsystem\\texture\\", 0.1f);
	Skybox->uniform_function = skybox_uniform;

	std::vector<Object*> *Object_List = new std::vector<Object*>(0);
	Object_List->push_back(Skybox);
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Sun.fbx",		".\\assets\\solarsystem\\texture\\", nullptr, SIZE_SUN));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Mercury.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_MERCURY));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Venus.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_VENUS));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Earth.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_EARTH));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Mars.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_MARS));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Jupiter.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_JUPITER));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Saturn.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_SATURN));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Uranus.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_URANUS));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Neptune.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_NEPTUNE));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Pluto.fbx",	".\\assets\\solarsystem\\texture\\", nullptr, SIZE_PLUTO));

	Object_List->at(SUN)->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_SUN, 0.0f, 0.0f));
	Object_List->at(SUN)->uniform_function = SunRotation;

	Object_List->at(MERCURY)->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_MERCURY, 0.0f, 0.0f));
	Object_List->at(MERCURY)->uniform_function = MercuryRotation;

	Object_List->at(VENUS)->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_VENUS, 0.0f, 0.0f));
	Object_List->at(VENUS)->uniform_function = VenusRotation;

	Object_List->at(EARTH)->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_EARTH, 0.0f, 0.0f));
	Object_List->at(EARTH)->uniform_function = EarthRotation;

	Object_List->at(MARS)->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_MARS, 0.0f, 0.0f));
	Object_List->at(MARS)->uniform_function = MarsRotation;

	Object_List->at(JUPITER)->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_JUPITER, 0.0f, 0.0f));
	Object_List->at(JUPITER)->uniform_function = JupiterRotation;

	Object_List->at(SATURN)->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_SATURN, 0.0f, 0.0f));
	Object_List->at(SATURN)->uniform_function = SaturnRotation;

	Object_List->at(URANUS)->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_URANUS, 0.0f, 0.0f));
	Object_List->at(URANUS)->uniform_function = UranusRotation;

	Object_List->at(NEPTUNE)->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_NEPTUNE, 0.0f, 0.0f));
	Object_List->at(NEPTUNE)->uniform_function = NeptuneRotation;

	Object_List->at(PLUTO)->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(DISTANCE_PLUTO, 0.0f, 0.0f));
	Object_List->at(PLUTO)->uniform_function = PlutoRotation;

	Object_List->push_back(fighter_ship);
	Object_List->push_back(normal_ship);
	Object_List->push_back(grid);
	Object_List->push_back(pyramid);
	Object_List->push_back(square);

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
	
	delete myWindow;

#if _DEBUG
	LOG("\n\nPress the enter key once or twice to exit.")
	std::cin.ignore().get();
#endif

	return 0;
}

