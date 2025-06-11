#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

const char* handle_error_type(int vk_result) {
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
#define handle_error(coolio, ahh) {vk_result = coolio; do{if(vk_result != VK_SUCCESS) {\
        fprintf(stderr,\
                 "ERR at %s, line %d:  %s",\
         __FILE__, __LINE__, handle_error_type(vk_result)); error_code = EXIT_FAILURE; goto ahh;}} while(0);}

void error_handle_glfw(int e, const char* msg) {
    fprintf(stderr, "GLFW ERR: %d, MSG: %s", e, msg);
}

int main() {
    int error_code = EXIT_SUCCESS;
    VkResult vk_result;
    glfwInit();
    glfwSetErrorCallback(&error_handle_glfw);
    if (!glfwVulkanSupported()) {
        perror("ERR: GLFW+Vulkan not supported");
        error_code = EXIT_FAILURE;
        goto exit_GLFW;
    }
    uint32_t version = 0;
    vkEnumerateInstanceVersion(&version);
    if (version < VK_VERSION_1_3) {
        perror("ERR: Vulkan 1.3 or above required");
        error_code = EXIT_FAILURE;
        goto exit_GLFW;
    }

    uint32_t instance_extension_count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&instance_extension_count);
    uint32_t layer_count = 1;
    const char* layers[1] = {"VK_LAYER_KHRONOS_validation"};
    VkInstance vulkan_instance;
    handle_error(vkCreateInstance(&(VkInstanceCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0x0,
        .pApplicationInfo = &(VkApplicationInfo) {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = NULL,
            .pApplicationName = "UntitledFactoryGame",
            .applicationVersion = 0,
            .pEngineName = "Tuntomite",
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_3
        },
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = instance_extension_count,
        .ppEnabledExtensionNames = extensions
    }, NULL, &vulkan_instance), exit_GLFW);
    //FIXME: Really bad error handling, maybe use something like a monad? or maybe engage in black magic one-line macros for filename and
    VkPhysicalDevice physical_device;
    {
    int type_to_prio[5] = { 4, 1, 0, 2, 3 };
    uint32_t num_physical_devices = 0;
    handle_error(vkEnumeratePhysicalDevices(vulkan_instance, &num_physical_devices, NULL), destroy_instance);
    VkPhysicalDevice* physical_devices = malloc(sizeof(VkPhysicalDevice)*num_physical_devices);
    handle_error(vkEnumeratePhysicalDevices(vulkan_instance, &num_physical_devices, physical_devices), destroy_instance);
    int best_i = 0;
    int best_prio = 5;
    for( int i = 0; i < num_physical_devices; i++) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physical_devices[i], &properties);
            if(type_to_prio[properties.deviceType] < best_prio) {
                best_i = i;
                best_prio = type_to_prio[properties.deviceType];
            }
    }
    physical_device = physical_devices[best_i];
    free(physical_devices);
    }
    if(vk_result != VK_SUCCESS) {
        switch (vk_result) {
        case VK_ERROR_INITIALIZATION_FAILED:
            perror("ERR: Failed initialization");
            break;
        default:
            perror("ERR: Out of memory??");
            break;
        }
        perror("ERR: failed to enumerate devices");
        goto destroy_instance;
    }
    uint32_t queue_family_num;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_num, NULL);
    VkQueueFamilyProperties* queue_family_properties = malloc(sizeof(VkQueueFamilyProperties)*queue_family_num);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_num, queue_family_properties);
    int queue_family_index;
    for (queue_family_index = 0;
        !(queue_family_properties[queue_family_index].queueFlags | 0x1); queue_family_index++ ) {
        if (queue_family_index == queue_family_num) {
            perror("ERR: Can't find graphics queue!!");
            error_code = EXIT_FAILURE;
            goto destroy_instance;
        }
    }
    VkDevice device;
    handle_error(
        vkCreateDevice( physical_device,
                    &(VkDeviceCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .queueCreateInfoCount = 1,
                        .pQueueCreateInfos = &(VkDeviceQueueCreateInfo) {
                            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0x0,
                            .queueFamilyIndex = queue_family_index,
                            .queueCount = 1,
                            .pQueuePriorities = ((const float[1]){1.0})
                        },
                        .enabledLayerCount = 0,
                        .ppEnabledLayerNames = NULL,
                        .enabledExtensionCount = 0,
                        .ppEnabledExtensionNames = NULL,
                        .pEnabledFeatures = NULL
                    },
                   NULL,
                   &device
    ), destroy_instance);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    //FIXME: give this a fallback
    if (!monitor) {
        perror("ERR: The program cannot find your primary monitor");
        error_code = -1;
        goto destory_device;
    }
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Hello Window", NULL, NULL);
    if (window == NULL) {
        perror("ERR: no window...");
        error_code = -1;
        goto exit_window;
    }

    VkSurfaceKHR surface;
    glfwCreateWindowSurface( vulkan_instance,
                    window,
                    NULL,
                    &surface
    );
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    VkExtent2D image_extent = (VkExtent2D) {
                    .width = width,
                    .height = height
    };
    int queue_family_indices[1] = {queue_family_index};
    VkSwapchainKHR swapchain;
    vkCreateSwapchainKHR( device,
                    &(VkSwapchainCreateInfoKHR) {
                        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                        .pNext = NULL,
                        .flags = 0x0,
                        .surface = surface,
                        .minImageCount = 1,
                        .imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
                        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                        .imageExtent = image_extent,
                        .imageArrayLayers = 1,
                        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                        .queueFamilyIndexCount = queue_family_index,
                        .pQueueFamilyIndices = queue_family_indices,
                        .preTransform = 0x0,
                        .compositeAlpha = 0x0,
                        .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
                        .clipped = VK_TRUE,
                        .oldSwapchain = VK_NULL_HANDLE
                    },
                    NULL,
                    &swapchain
    );
    VkQueue queue;
    vkGetDeviceQueue(device, queue_family_index, 0, &queue);
    VkCommandPool command_pool;
    vkCreateCommandPool( device,
                    &(VkCommandPoolCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .queueFamilyIndex = queue_family_index
                    },
                   NULL,
                   &command_pool
    );
    VkCommandBuffer command_buffer;
    VkCommandBuffer command_buffer_array[1] = {command_buffer};
    vkAllocateCommandBuffers( device,
                    &(VkCommandBufferAllocateInfo) {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                        .pNext = NULL,
                        .commandPool = command_pool,
                        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                        .commandBufferCount = 1
                    },
                   command_buffer_array
    );
    VkAttachmentDescription attachment_description = (VkAttachmentDescription) {
                        .flags = 0x0,
                        .format = VK_FORMAT_B8G8R8A8_SRGB,
                        .samples = VK_SAMPLE_COUNT_1_BIT,
                        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };
    VkAttachmentDescription attachment_description_array[1] = {attachment_description};
    VkAttachmentReference attachment_reference = (VkAttachmentReference) {
                        .attachment = 0,
                        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    VkAttachmentReference attachment_reference_array[1] = {attachment_reference};
    VkSubpassDescription subpass_description = (VkSubpassDescription) {
                        .flags = 0x0,
                        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                        .inputAttachmentCount = 0,
                        .pInputAttachments = NULL,
                        .colorAttachmentCount = 1,
                        .pColorAttachments = attachment_reference_array,
                        .pResolveAttachments = NULL,
                        .pDepthStencilAttachment = NULL,
                        .preserveAttachmentCount = 0,
                        .pPreserveAttachments = NULL

    };
    VkSubpassDescription subpass_description_array[1] = {subpass_description};
    VkSubpassDependency subpass_dependency = (VkSubpassDependency) {
                        .srcSubpass = 0,
                        .dstSubpass = 0,
                        .srcStageMask = 0x0,
                        .dstStageMask = 0x0,
                        .srcAccessMask = 0x0,
                        .dstAccessMask = 0x0,
                        .dependencyFlags = 0x0
    };
    VkSubpassDependency subpass_dependency_array[1] = {subpass_dependency};
    VkRenderPass render_pass;
    vkCreateRenderPass( device,
                    &(VkRenderPassCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .attachmentCount = 1,
                        .pAttachments = attachment_description_array,
                        .subpassCount = 1,
                        .pSubpasses = subpass_description_array,
                        .dependencyCount = 0,
                        .pDependencies = NULL
                    },
                    NULL,
                    &render_pass
    );
    
    VkExtent3D extent = (VkExtent3D) {
        .width = width,
        .height = height,
        .depth = 1
    };
    VkImage image;
    vkCreateImage( device,
                    &(VkImageCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .imageType = VK_IMAGE_TYPE_2D,
                        .format = VK_FORMAT_B8G8R8A8_SRGB,
                        .extent = extent,
                        .mipLevels = 1,
                        .arrayLayers = 1,
                        .samples = VK_SAMPLE_COUNT_1_BIT,
                        .tiling = VK_IMAGE_TILING_OPTIMAL,
                        .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                        .queueFamilyIndexCount = 1,
                        .pQueueFamilyIndices = queue_family_indices,
                        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
                    },
                    NULL,
                    &image
    );
    int image_count;
    vkGetSwapchainImagesKHR( device,
                    swapchain,
                    &image_count,
                    NULL
    );
    VkImage *swapchain_image_array = malloc(image_count);
    vkGetSwapchainImagesKHR( device,
                    swapchain,
                    &image_count,
                    swapchain_image_array
    );
    VkImageView image_view;
    vkCreateImageView( device,
                    &(VkImageViewCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .image = swapchain_image_array[0],
                        .viewType = VK_IMAGE_VIEW_TYPE_2D,
                        .format = VK_FORMAT_B8G8R8A8_SRGB,
                        .components = (VkComponentMapping) {
                            .r = VK_COMPONENT_SWIZZLE_R,
                            .g = VK_COMPONENT_SWIZZLE_G,
                            .b = VK_COMPONENT_SWIZZLE_B,
                            .a = VK_COMPONENT_SWIZZLE_A,
                        },
                        .subresourceRange = (VkImageSubresourceRange) {
                            .aspectMask = VK_IMAGE_ASPECT_NONE_KHR,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1
                        }
                    },
                    NULL,
                    &image_view
    );
    VkImageView image_view_array[1] = {image_view};
    VkFramebuffer framebuffer;
    vkCreateFramebuffer( device,
                    &(VkFramebufferCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .renderPass = render_pass,
                        .attachmentCount = 1,
                        .pAttachments = image_view_array,
                        .width = 1,
                        .height = 1, 
                        .layers = 1
                    },
                    NULL,
                    &framebuffer
    );
    vkBeginCommandBuffer( command_buffer,
                    &(VkCommandBufferBeginInfo) {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .pInheritanceInfo = &(VkCommandBufferInheritanceInfo) {
                            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
                            .pNext = NULL,
                            .renderPass = render_pass,
                            .subpass = 0,
                            .framebuffer = framebuffer,
                            .occlusionQueryEnable = VK_TRUE,
                            .queryFlags = 0x0,
                            .pipelineStatistics = 0x0,
                        }
                    }
    );

    FILE *f = fopen("shaders/vert.spv", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *vert_shader_code = malloc(fsize);
    fread(vert_shader_code, sizeof(char), fsize, f);
    fclose(f);
    printf(vert_shader_code);

    while (!glfwWindowShouldClose(window) && glfwGetMouseButton(window, 1) != GLFW_PRESS) {
        glfwPollEvents();
    }
    printf("Exiting normally!!\n\n");
exit_window:
    glfwDestroyWindow(window);
exit_vulkan:
    vkFreeCommandBuffers(device, command_pool, 1, command_buffer_array);
destory_device:
    vkDestroyDevice(device, NULL);
destroy_instance:
    vkDestroyInstance(vulkan_instance, NULL);
exit_GLFW:
    glfwTerminate();

    return error_code;
}
