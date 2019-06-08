#include "pch.h"
#include "Window.h"

#include "planets.h"

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

