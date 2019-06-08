#ifndef PCH_H
#define PCH_H

//SDL
#include "SDL.h"
#include "SDL_vulkan.h"

//Vulkan
#include "vulkan/vulkan.h"

//FBX SDK
#include "fbxsdk.h"

//STL
#include <optional>
#include <set>
#include <algorithm>
#include <cstring>
#include <vector>
#include <fstream>
#include <array>
#include <chrono>
#include <random>
#include <conio.h>
#include <iostream>

#include "vec4.hpp"
#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"

//Lari's XTime
#include "XTime.h"

#ifdef _DEBUG
//Logging Information
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

#endif