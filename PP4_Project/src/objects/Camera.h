#ifndef CAMERA_H
#define CAMERA_H

#include "../Defines.h"

class Camera
{
public:
	Camera();

	void update();
private:
	glm::mat4 view;
	glm::mat4 view_inv;
	glm::mat4 perspective;
};

#endif