#ifndef DEFINES_H
#define DEFINES_H

#pragma region VULKAN_INCLUDE_AND_DEFINES

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#ifdef _DEBUG
#include <iostream>
#define LOG(x) std::cout << x << std::endl;
#else
#define LOG(x) 
#endif

#pragma endregion This contains defines and includes with GLFW and Vulkan

#define MAX_FRAMES_FLIGHT 2

#include <optional>
#include <set>
#include <algorithm>
#include <cstring>
#include "vulkan/vulkan.h"
#include <vector>
#include <fstream>
#include <array>
#include <chrono>
#include <random>

#include "texture_h/celestial.h"
#include "texture_h/axeTexture.h"
#include "../vendor/fbxsdk/include/fbxsdk.h"

#define RAND_NORMAL glm::vec3(rand()/float(RAND_MAX),rand()/float(RAND_MAX),rand()/float(RAND_MAX))

#define V_COLOR_BLACK	{0.0f, 0.0f, 0.0f, 1.0f}
#define V_COLOR_RED		{1.0f, 0.0f, 0.0f, 1.0f}
#define V_COLOR_GREEN	{0.0f, 1.0f, 0.0f, 1.0f}
#define V_COLOR_BLUE	{0.0f, 0.0f, 1.0f, 1.0f}
#define V_COLOR_MAGENTA	{1.0f, 0.0f, 1.0f, 1.0f}
#define V_COLOR_CYAN	{0.0f, 1.0f, 1.0f, 1.0f}
#define V_COLOR_YELLOW	{1.0f, 1.0f, 0.0f, 1.0f}
#define V_COLOR_WHITE	{1.0f, 1.0f, 1.0f, 1.0f}

#endif // #ifndef DEFINES_H
