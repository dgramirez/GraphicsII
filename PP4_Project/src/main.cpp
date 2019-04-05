#include "Window.h"

int main()
{
	Window* myWindow = new Window(800,600,"My New Window");
	try
	{
		myWindow->run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
#if _DEBUG
	std::cout << "\n\nPress the enter key once or twice to exit." << std::endl;
	std::cin.ignore().get();
#endif

	return 0;
}