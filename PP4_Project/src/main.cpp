#include "Window.h"

int main()
{
	Window* myWindow = new Window(800,600,"My New Window");
	myWindow->run();

#if _DEBUG
	std::cout << "\n\nPress the enter key once or twice to exit." << std::endl;
	std::cin.ignore().get();
#endif

	return 0;
}