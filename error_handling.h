#pragma once
#include <vulkan/vulkan.h>
const char* handle_vulkan_error(int vk_result) {
    switch(vk_result) {
    case VK_ERROR_OUT_OF_HOST_MEMORY: return "A host memory allocation has failed.";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "A device memory allocation has failed.";
    case VK_ERROR_INITIALIZATION_FAILED: return "Initialization of an object could not be completed for implementation-specific reasons.";
    case VK_ERROR_DEVICE_LOST: return "The logical or physical device has been lost. See Lost Device";
    case VK_ERROR_MEMORY_MAP_FAILED: return "Mapping of a memory object has failed.";
    case VK_ERROR_LAYER_NOT_PRESENT: return "A requested layer is not present or could not be loaded.";
    case VK_ERROR_EXTENSION_NOT_PRESENT: return "A requested extension is not supported.";
    case VK_ERROR_FEATURE_NOT_PRESENT: return "A requested feature is not supported.";
    case VK_ERROR_INCOMPATIBLE_DRIVER: return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons.";
    case VK_ERROR_TOO_MANY_OBJECTS: return "Too many objects of the type have already been created.";
    case VK_ERROR_FORMAT_NOT_SUPPORTED: return "A requested format is not supported on this device.";
    case VK_ERROR_FRAGMENTED_POOL: return "A pool allocation has failed due to fragmentation of the pool’s memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation.";
    case VK_ERROR_SURFACE_LOST_KHR: return "A surface is no longer available.";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again.";
    case VK_ERROR_OUT_OF_DATE_KHR: return "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface.";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image.";
    case VK_ERROR_INVALID_SHADER_NV: return "One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled.";
    case VK_ERROR_OUT_OF_POOL_MEMORY: return "A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead.";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "An external handle is not a valid handle of the specified type.";
    case VK_ERROR_FRAGMENTATION: return "A descriptor pool creation has failed due to fragmentation.";
    case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: return "A buffer creation failed because the requested address is not available, A buffer creation or memory allocation failed because the requested address is not available, or A shader group handle assignment failed because the requested shader group handle information is no longer valid.";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exclusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application’s control.";
    case VK_ERROR_VALIDATION_FAILED_EXT: return "A command failed because invalid usage was detected by the implementation or a validation-layer.";
    case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: return "An image creation failed because internal resources required for compression are exhausted. This must only be returned when fixed-rate compression is requested.";
    case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: return "The requested VkImageUsageFlags are not supported.";
    case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: return "The requested video picture layout is not supported.";
    case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: return "A video profile operation specified via VkVideoProfileInfoKHR::videoCodecOperation is not supported.";
    case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: return "Format parameters in a requested VkVideoProfileInfoKHR chain are not supported.";
    case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: return "Codec-specific parameters in a requested VkVideoProfileInfoKHR chain are not supported.";
    case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: return "The specified video Std header version is not supported.";
    case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR: return "The specified Video Std parameters do not adhere to the syntactic or semantic requirements of the used video compression standard, or values derived from parameters according to the rules defined by the used video compression standard do not adhere to the capabilities of the video compression standard or the implementation.";
    case VK_ERROR_NOT_PERMITTED: return "The driver implementation has denied a request to acquire a priority above the default priority (VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_EXT) because the application does not have sufficient privileges.";
    case VK_ERROR_NOT_ENOUGH_SPACE_KHR: return "The application did not provide enough space to return all the required data.";
    case VK_ERROR_UNKNOWN: return "An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred.";
    default: return "UNKNOWN UNKNOWN :/";
    }
}
#define handle_error(coolio, ahh) {vk_result = coolio; do{if(vk_result < VK_SUCCESS) {\
        fprintf(stderr,\
                 "ERR at %s, line %d:  %s",\
         __FILE__, __LINE__, handle_vulkan_error(vk_result)); error_code = EXIT_FAILURE; goto ahh;}} while(0);}

