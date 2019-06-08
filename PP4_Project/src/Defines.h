#ifndef DEFINES_H
#define DEFINES_H



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

#define MAX_FRAMES 2



#include "texture_h/celestial.h"
#include "texture_h/axeTexture.h"
#include "texture_h/eyeball.h"

#include "InputController.h"


class VkObj_Context;
class Object;
class Camera;

typedef void(*UniformFctn)(const VkObj_Context &context, Object &obj, Camera &camera);

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

#define SHADER_BASIC_COLOR 0
#define SHADER_BASIC_TEXTURE 1
#define SHADER_EARTH 2

#define SKYBOX 0
#define SUN 1
#define MERCURY 2
#define VENUS 3
#define EARTH 4
#define MARS 5
#define JUPITER 6
#define SATURN 7
#define URANUS 8
#define NEPTUNE 9
#define PLUTO 10

#define SIZE_SPHERE			79.1f
#define SIZE_SUN			SIZE_SPHERE / 139.2f
#define SIZE_MERCURY		SIZE_SPHERE /  0.50f
#define SIZE_VENUS			SIZE_SPHERE /  1.20f
#define SIZE_EARTH			SIZE_SPHERE /  1.30f
#define SIZE_EARTHMOON		SIZE_SPHERE /  0.35f
#define SIZE_MARS			SIZE_SPHERE /  0.70f
#define SIZE_JUPITER		SIZE_SPHERE / 14.30f
#define SIZE_SATURN			SIZE_SPHERE / 12.50f
#define SIZE_URANUS			SIZE_SPHERE /  5.10f
#define SIZE_NEPTUNE		SIZE_SPHERE /  5.00f
#define SIZE_PLUTO			SIZE_SPHERE /  0.23f

#define DISTANCE_SUN						 0.00f
#define DISTANCE_MERCURY	SIZE_SPHERE +   35.98f
#define DISTANCE_VENUS		SIZE_SPHERE +   67.24f
#define DISTANCE_EARTH		SIZE_SPHERE +   92.96f
#define DISTANCE_MARS		SIZE_SPHERE +  141.60f
#define DISTANCE_JUPITER	SIZE_SPHERE +  483.80f
#define DISTANCE_SATURN		SIZE_SPHERE +  890.80f
#define DISTANCE_URANUS		SIZE_SPHERE + 1784.00f
#define DISTANCE_NEPTUNE	SIZE_SPHERE + 2793.00f
#define DISTANCE_PLUTO		SIZE_SPHERE + 3670.00f

#define GRID_LENGTH 10.0f
#define GRID_SUBLENGTH 1.0f

#endif // #ifndef DEFINES_H
