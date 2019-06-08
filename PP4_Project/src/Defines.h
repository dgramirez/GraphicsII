#ifndef DEFINES_H
#define DEFINES_H

#define MAX_UINT32	0xFFFFFFFF
#define MAX_UINT64	0xFFFFFFFFFFFFFFFF

#define MASK_BIT_1  0x1
#define MASK_BIT_2  0x2
#define MASK_BIT_3  0x4
#define MASK_BIT_4  0x8
#define MASK_BIT_5  0x10
#define MASK_BIT_6  0x20
#define MASK_BIT_7  0x40
#define MASK_BIT_8  0x80
#define MASK_BIT_9  0x100
#define MASK_BIT_10 0x200
#define MASK_BIT_11	0x400
#define MASK_BIT_12 0x800
#define MASK_BIT_13 0x1000
#define MASK_BIT_14 0x2000
#define MASK_BIT_15 0x4000
#define MASK_BIT_16 0x8000
#define MASK_BIT_32 0x80000000

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

#endif // #ifndef DEFINES_H
