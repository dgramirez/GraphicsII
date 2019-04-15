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
//Logging Information
#include <iostream>
#define LOG(log_message)													std::cout << log_message << std::endl;
#define CHECK_VKRESULT(result_variable, vkFunction, log_message)			VkResult result_variable = vkFunction; if (result_variable != VK_SUCCESS) {LOG(log_message << " Error Code: " << result_variable) return false;}
#define CHECK_VKRESULT_NO_RETURN(result_variable, vkFunction, log_message)	VkResult result_variable = vkFunction; if (result_variable != VK_SUCCESS) {LOG(log_message << " Error Code: " << result_variable)}
#define CHECK_RESULT(bool_result, log_message)								if (!bool_result)	{LOG(log_message) return false;}
#define CHECK_RESULT_NO_RETURN(bool_result, log_message)					if (!bool_result)	{LOG(log_message)}

#else
#define LOG(log_message)
#define CHECK_VKRESULT(result_variable, vkFunction, log_message) vkFunction
#define CHECK_VKRESULT_NO_RETURN(result_variable, vkFunction, log_message) vkFunction
#define CHECK_RESULT(bool_result, log_message) bool_result
#define CHECK_RESULT_NO_RETURN(bool_result, log_message) bool_result
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

#define MAX_UINT32	0xFFFFFFFF
#define MAX_UINT64	0xFFFFFFFFFFFFFFFF

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
