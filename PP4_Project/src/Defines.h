#ifndef DEFINES_H
#define DEFINES_H

#pragma region VULKAN_INCLUDE_AND_DEFINES

#define GLFW_INCLUDE_VULKAN
#include "SDL.h"
#include "SDL_vulkan.h"

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

#define MAX_FRAMES 2

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
#include <conio.h>

#include "texture_h/celestial.h"
#include "texture_h/axeTexture.h"
#include "../vendor/fbxsdk/include/fbxsdk.h"
#include "InputController.h"
#include "../vendor/fullsail/XTime.h"

#define MAX_UINT32	0xFFFFFFFF
#define MAX_UINT64	0xFFFFFFFFFFFFFFFF

#define MASK_BIT_1 0x1
#define MASK_BIT_2 0x2
#define MASK_BIT_3 0x4
#define MASK_BIT_4 0x8
#define MASK_BIT_5 0x10
#define MASK_BIT_6 0x20
#define MASK_BIT_7 0x40
#define MASK_BIT_8 0x80
#define MASK_BIT_32 0x80000000

#define MAPPED_FLAG 
#define OWNS_BUFFER_FLAG 0x80000000

#define CAST(type,variable) static_cast<type>(variable)

#define RAND_NORMAL glm::vec3(rand()/float(RAND_MAX),rand()/float(RAND_MAX),rand()/float(RAND_MAX))

#define V_COLOR_BLACK	{0.0f, 0.0f, 0.0f, 1.0f}
#define V_COLOR_RED		{1.0f, 0.0f, 0.0f, 1.0f}
#define V_COLOR_GREEN	{0.0f, 1.0f, 0.0f, 1.0f}
#define V_COLOR_BLUE	{0.0f, 0.0f, 1.0f, 1.0f}
#define V_COLOR_MAGENTA	{1.0f, 0.0f, 1.0f, 1.0f}
#define V_COLOR_CYAN	{0.0f, 1.0f, 1.0f, 1.0f}
#define V_COLOR_YELLOW	{1.0f, 1.0f, 0.0f, 1.0f}
#define V_COLOR_WHITE	{1.0f, 1.0f, 1.0f, 1.0f}

#define VKDEFINE_HOST_VISIBLE_MEMORY 64
#define VKDEFINE_DEVICE_LOCAL_MEMORY 128
#define VKDEFINE_GPU_UPLOAD_BUFFER_SIZE 64

#define VKDEFINE_MEMORY_USAGE_GPU_ONLY 0
#define VKDEFINE_MEMORY_USAGE_GPU_TO_CPU 1
#define VKDEFINE_MEMORY_USAGE_CPU_ONLY 2
#define VKDEFINE_MEMORY_USAGE_CPU_TO_GPU 3

#define VKDEFINE_ALLOCATION_TYPE_FREE 0
#define VKDEFINE_ALLOCATION_TYPE_BUFFER 1
#define VKDEFINE_ALLOCATION_TYPE_IMAGE 2
#define VKDEFINE_ALLOCATION_TYPE_IMAGE_LINEAR 3
#define VKDEFINE_ALLOCATION_TYPE_IMAGE_OPTIMAL 4

#define VKDEFINE_OPERATION_TYPE_LESS -1
#define VKDEFINE_OPERATION_TYPE_EQUAL 0
#define VKDEFINE_OPERATION_TYPE_GREATER 1

#define VKDEFINE_RENDERCOMMAND_NOP 0
#define VKDEFINE_RENDERCOMMAND_DRAW_VIEW_3D 1
#define VKDEFINE_RENDERCOMMAND_DRAW_VIEW_GUI 2
#define VKDEFINE_RENDERCOMMAND_COPY_RENDER 3
#define VKDEFINE_RENDERCOMMAND_POST_PROCESS 4

#define VKDEFINE_BUFFER_MAP_READ 1
#define VKDEFINE_BUFFER_MAP_WRITE 2
#define VKDEFINE_BUFFER_USAGE_STATIC 1
#define VKDEFINE_BUFFER_USAGE_DYNAMIC 2

//This is sourced in the Linux-Kernel for byte alignment.
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))
//Modified to work with 32-bit unsigned integers, as "Typeof(x)" doesn't work [and 64-bit unsigned integers]
#define ALIGN_UINT32(x,a)       __ALIGN_MASK(x,(unsigned int)(a)-1)
#define ALIGN_UINT64(x,a)		__ALIGN_MASK(x, (unsigned long long)(a)-1)

#define MB_TO_BYTE(x) x * 1048576

#define TUTORIAL_TRANSITION 0

#define OBJECT_TYPE_FBX 0
#define OBJECT_TYPE_OBJ 1
#define OBJECT_TYPE_DOTH 2

#define TEXTURE_TYPE_STBI 0
#define TEXTURE_TYPE_HEAP 1
#define TEXTURE_TYPE_DOTH 2

#endif // #ifndef DEFINES_H
