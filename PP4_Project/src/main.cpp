#include "Window.h"
#include "Assets.h"
#include "Object.h"

int main()
{
	Window* myWindow = new Window(800, 600, "My New Window");

	std::vector<Vertex> pyramid_vertex = {
	{ { 0.0f, -0.5f,  0.0f, 1.0f}, V_COLOR_RED	 , {0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Top point
	{ { 0.5f,  0.5f,  0.5f, 1.0f}, V_COLOR_GREEN , {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 1
	{ {-0.5f,  0.5f,  0.5f, 1.0f}, V_COLOR_BLUE  , {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 2
	{ { 0.5f,  0.5f, -0.5f, 1.0f}, V_COLOR_CYAN  , {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },	//Bottom Vertex 3
	{ {-0.5f,  0.5f, -0.5f, 1.0f}, V_COLOR_YELLOW, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }	//Bottom Vertex 4;
	};

	const std::vector<uint32_t> pyramid_index = {
	0, 2, 4,
	0, 1, 2,
	0, 3, 1,
	0, 4, 3
	};

	std::vector<Vertex> two_squares = {
	{{-0.5f, -0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 0.5f,  0.5f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{-0.5f,  0.5f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
																	   
	{{-0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{ 0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{-0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}
	};

	const std::vector<uint32_t> two_squares_index = {
	0, 1, 2,
	2, 3, 0,
	4, 5, 6,
	6, 7, 4
	};

	Texture pyramid_texture(celestial_width, celestial_height, celestial_pixels, false);
	Object Pyramid(pyramid_vertex, pyramid_index, pyramid_texture);
	Object TwoSquare(two_squares, pyramid_index, pyramid_texture);
	Object3D Axe("Axe.fbx");
	std::vector<Object> Object_List;
	Object_List.push_back(Pyramid);
	Object_List.push_back(TwoSquare);

	myWindow->setup_object_list(Object_List);

#ifdef _DEBUG
	try
	{
		myWindow->run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
#else
	myWindow->run();
#endif
#if _DEBUG
	std::cout << "\n\nPress the enter key once or twice to exit." << std::endl;
	std::cin.ignore().get();
#endif

	return 0;
}