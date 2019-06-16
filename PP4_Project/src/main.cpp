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
	Object *square = create_square(".\\assets\\misc\\white_pixel.png", -GRID_LENGTH, GRID_LENGTH, -GRID_LENGTH, GRID_LENGTH);
	Object *flag = create_flag();
	flag->model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -7.0f, -10.0f)) ;
	flag->uniform_function = flag_uniform;
	Object *circle = create_ball();
	Object *Skybox = new Object(".\\assets\\solarsystem\\SpaceSkybox.fmd", ".\\assets\\solarsystem\\texture\\SpaceCubeMap.png");
	Skybox->uniform_function = skybox_uniform;

	std::vector<Object*> *Object_List = new std::vector<Object*>(0);
	Object_List->push_back(Skybox);
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Sun.fmd",		".\\assets\\solarsystem\\texture\\sunmap.png"));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Mercury.fmd",	".\\assets\\solarsystem\\texture\\mercurymap.png"));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Venus.fmd",	".\\assets\\solarsystem\\texture\\venusmap.png"));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Earth.fmd",	".\\assets\\solarsystem\\texture\\earthmap1k.png"));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Mars.fmd",	".\\assets\\solarsystem\\texture\\marsmap1k.png"));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Jupiter.fmd",	".\\assets\\solarsystem\\texture\\jupitermap.png"));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Saturn.fmd",	".\\assets\\solarsystem\\texture\\saturnmap.png"));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Uranus.fmd",	".\\assets\\solarsystem\\texture\\uranusmap.png"));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Neptune.fmd",	".\\assets\\solarsystem\\texture\\neptunemap.png"));
	Object_List->push_back(create_sphere(".\\assets\\solarsystem\\Pluto.fmd",	".\\assets\\solarsystem\\texture\\plutomap1k.png"));

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
	Object_List->push_back(flag);
	Object_List->push_back(circle);

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

