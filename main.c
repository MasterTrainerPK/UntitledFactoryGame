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
#define handle_error(coolio, ahh) {vk_result = coolio; do{if(vk_result < VK_SUCCESS) {\
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
    const char** extensions;
    {
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&instance_extension_count);
    extensions = malloc(sizeof(char*) * instance_extension_count + 0);
    for(int i = 0; i < instance_extension_count; i++) {
        extensions[i] = glfw_extensions[i];
    }
    //extensions[instance_extension_count] = "VK_KHR_swapchain";
    printf("%s", "Loading Extensions\n");
    for(int i = 0; i < instance_extension_count; i++) {
        printf("%s\n", extensions[i]);
    }
    }
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
    // order of priority is descrete, integrated, virtual, cpu, other
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
                if(best_prio == 0) {
                    break;
                }
            }
    }
    physical_device = physical_devices[best_i];
    free(physical_devices);
    }
    uint32_t queue_family_num;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_num, NULL);
    VkQueueFamilyProperties* queue_family_properties = malloc(sizeof(VkQueueFamilyProperties)*queue_family_num);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_num, queue_family_properties);
    int queue_family_index;
    for (queue_family_index = 0;
        !(queue_family_properties[queue_family_index].queueFlags | VK_QUEUE_GRAPHICS_BIT); queue_family_index++ ) {
        if (queue_family_index == queue_family_num) {
            perror("ERR: Can't find graphics queue!!");
            error_code = EXIT_FAILURE;
            goto destroy_instance;
        }
    }
    VkDevice device;
    const char* const device_extension[1] = {"VK_KHR_swapchain"};
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
                        .enabledExtensionCount = 1,
                        .ppEnabledExtensionNames = device_extension,
                        .pEnabledFeatures = NULL
                    },
                   NULL,
                   &device
    ), destroy_instance);
    printf("%s", "Device created\n");
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    //FIXME: give this a fallback
    if (!monitor) {
        perror("ERR: The program cannot find your primary monitor");
        error_code = -1;
        goto destory_device;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Hello Window", NULL, NULL);
    if (window == NULL) {
        perror("ERR: no window...");
        error_code = -1;
        goto destroy_window;
    }

    VkSurfaceKHR surface;
    handle_error(glfwCreateWindowSurface( vulkan_instance,
                    window,
                    NULL,
                    &surface
    ), destroy_window);
    printf("%s", "Surface created\n");
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    VkExtent2D image_extent = (VkExtent2D) {
                    .width = width,
                    .height = height
    };
    int queue_family_indices[1] = {queue_family_index};
    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);
    int surface_format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, NULL);
    VkSurfaceFormatKHR *surface_format_array = malloc(sizeof(VkSurfaceFormatKHR) * surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, surface_format_array);
    VkSurfaceFormatKHR surface_format = surface_format_array[0];
    for (int i = 0; i < surface_format_count; i++) {
        VkSurfaceFormatKHR available_surface_format = surface_format_array[i];
        if (available_surface_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surface_format = available_surface_format;
            break;
        }
    }
    int present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, NULL);
    VkPresentModeKHR *present_mode_array = malloc(sizeof(VkPresentModeKHR) * present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_mode_array);
    VkPresentModeKHR present_mode = present_mode_array[0];
    for (int i = 0; i < present_mode_count; i++) {
        VkPresentModeKHR available_present_mode = present_mode_array[i];
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = available_present_mode;
            break;
        }
    }
    VkSwapchainKHR swapchain;
    handle_error(vkCreateSwapchainKHR(device,
                    &(VkSwapchainCreateInfoKHR) {
                        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                        .pNext = NULL,
                        .flags = 0x0,
                        .surface = surface,
                        .minImageCount = surface_capabilities.minImageCount,
                        .imageFormat = surface_format.format,
                        .imageColorSpace = surface_format.colorSpace,
                        .imageExtent = image_extent,
                        .imageArrayLayers = 1,
                        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                        .queueFamilyIndexCount = 0,
                        .pQueueFamilyIndices = NULL,
                        .preTransform = surface_capabilities.currentTransform,
                        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                        .presentMode = present_mode,
                        .clipped = VK_TRUE,
                        .oldSwapchain = VK_NULL_HANDLE
                    },
                    NULL,
                    &swapchain
    ), destroy_surface);
    printf("%s", "Swapchain created\n");
    int image_count = surface_capabilities.minImageCount;
    VkQueue queue;
    vkGetDeviceQueue(device, queue_family_indices[0], 0, &queue);
    printf("%s", "Queue created\n");
    VkCommandPool command_pool;
    handle_error(vkCreateCommandPool(device,
                    &(VkCommandPoolCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                        .pNext = NULL,
                        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                        .queueFamilyIndex = queue_family_indices[0]
                    },
                   NULL,
                   &command_pool
    ), destory_swapchain);
    printf("%s", "Command pool created\n");
    VkCommandBuffer command_buffer;
    VkCommandBuffer command_buffer_array[1] = {command_buffer};
    handle_error(vkAllocateCommandBuffers(device,
                    &(VkCommandBufferAllocateInfo) {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                        .pNext = NULL,
                        .commandPool = command_pool,
                        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                        .commandBufferCount = 1
                    },
                   command_buffer_array
    ), destroy_command_pool);
    printf("%s", "Command buffer allocated\n");
    VkAttachmentDescription attachment_description = (VkAttachmentDescription) {
                        .flags = 0x0,
                        .format = surface_format.format,
                        .samples = VK_SAMPLE_COUNT_1_BIT,
                        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };
    VkAttachmentDescription *attachment_description_array = malloc(sizeof(VkAttachmentDescription) * image_count);
    for (int i = 0; i < image_count; i++) {
        attachment_description_array[i] = attachment_description;
    }
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
                        .srcSubpass = VK_SUBPASS_EXTERNAL,
                        .dstSubpass = 0,
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .srcAccessMask = 0x0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        .dependencyFlags = 0x0
    };
    VkSubpassDependency subpass_dependency_array[1] = {subpass_dependency};
    VkRenderPass render_pass;
    handle_error(vkCreateRenderPass(device,
                    &(VkRenderPassCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .attachmentCount = image_count,
                        .pAttachments = attachment_description_array,
                        .subpassCount = 1,
                        .pSubpasses = subpass_description_array,
                        .dependencyCount = 1,
                        .pDependencies = subpass_dependency_array
                    },
                    NULL,
                    &render_pass
    ), free_command_buffers);
    printf("%s", "Render pass created\n");
    VkExtent3D extent = (VkExtent3D) {
        .width = width,
        .height = height,
        .depth = 1
    };
    VkImage image;
    handle_error(vkCreateImage(device,
                    &(VkImageCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .imageType = VK_IMAGE_TYPE_2D,
                        .format = surface_format.format,
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
    ), destroy_render_pass);
    //int image_count;
    //vkGetSwapchainImagesKHR(device,
    //                swapchain,
    //                &image_count,
    //                NULL
    //);
    VkImage *swapchain_image_array = malloc(sizeof(VkImage) * image_count);
    vkGetSwapchainImagesKHR(device,
                    swapchain,
                    &image_count,
                    swapchain_image_array
    );
    VkImageView *image_view_array = malloc(sizeof(VkImageView) * image_count);
    int created_image_views;
    for (created_image_views = 0; created_image_views < image_count; created_image_views++) {
        VkImageView image_view;
        handle_error(vkCreateImageView( device,
                    &(VkImageViewCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .image = swapchain_image_array[created_image_views],
                        .viewType = VK_IMAGE_VIEW_TYPE_2D,
                        .format = surface_format.format,
                        .components = (VkComponentMapping) {
                            .r = VK_COMPONENT_SWIZZLE_R,
                            .g = VK_COMPONENT_SWIZZLE_G,
                            .b = VK_COMPONENT_SWIZZLE_B,
                            .a = VK_COMPONENT_SWIZZLE_A,
                        },
                        .subresourceRange = (VkImageSubresourceRange) {
                            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1
                        }
                    },
                    NULL,
                    &image_view
        ), destroy_image_views);
        image_view_array[created_image_views] = image_view;
    }
    VkFramebuffer framebuffer;
    handle_error(vkCreateFramebuffer( device,
                    &(VkFramebufferCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .renderPass = render_pass,
                        .attachmentCount = image_count,
                        .pAttachments = image_view_array,
                        .width = width,
                        .height = height, 
                        .layers = 1
                    },
                    NULL,
                    &framebuffer
    ), destroy_image_views);
    printf("%s", "Frame buffer created\n");
    FILE *f_vertex = fopen("shaders/vert.spv", "rb");
    if(f_vertex == NULL) {
        perror("failed to open file");
        goto destroy_frame_buffer;
    }
    fseek(f_vertex, 0, SEEK_END);
    long fsize_vertex = ftell(f_vertex);
    fseek(f_vertex, 0, SEEK_SET);
    uint32_t *vertex_shader_code = malloc(sizeof(uint32_t) * fsize_vertex);
    fread(vertex_shader_code, sizeof(uint32_t), fsize_vertex, f_vertex);
    fclose(f_vertex);
    VkShaderModule vertex_shader_module;
    handle_error(vkCreateShaderModule( device,
                    &(VkShaderModuleCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .codeSize = fsize_vertex,
                        .pCode = vertex_shader_code
                    },
                    NULL,
                    &vertex_shader_module
    ), destory_frame_buffer);
    FILE *f_fragment = fopen("shaders/frag.spv", "rb");
    if(f_fragment == NULL) {
        perror("failed to open file");
        goto destroy_vertex_shader_module;
    }
    fseek(f_fragment, 0, SEEK_END);
    long fsize_fragment = ftell(f_fragment);
    fseek(f_fragment, 0, SEEK_SET);
    uint32_t *fragment_shader_code = malloc(sizeof(uint32_t) * fsize_fragment);
    fread(fragment_shader_code, sizeof(uint32_t), fsize_fragment, f_fragment);
    fclose(f_fragment);
    VkShaderModule fragment_shader_module;
    handle_error(vkCreateShaderModule( device,
                    &(VkShaderModuleCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .codeSize = fsize_fragment,
                        .pCode = fragment_shader_code
                    },
                    NULL,
                    &fragment_shader_module
    ), destroy_vertex_shader_module);
    VkPipelineShaderStageCreateInfo vertex_shader_stage = (VkPipelineShaderStageCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0x0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertex_shader_module,
        .pName = "main",
        .pSpecializationInfo = NULL
    };
    VkPipelineShaderStageCreateInfo fragment_shader_stage = (VkPipelineShaderStageCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0x0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragment_shader_module,
        .pName = "main",
        .pSpecializationInfo = NULL
    };
    VkPipelineShaderStageCreateInfo shader_stage_array[2] = {vertex_shader_stage, fragment_shader_stage};
    printf("%s", "Shaders created\n");
    VkViewport viewport = (VkViewport) {
        .x = 0.0f,
        .y = 0.0f,
        .width = width,
        .height = height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    VkRect2D scissor = (VkRect2D) {
        .offset = {0, 0},
        .extent = image_extent
    };
    VkPipelineColorBlendAttachmentState color_blend_attachment = (VkPipelineColorBlendAttachmentState) {
                    .blendEnable = VK_FALSE,
                    .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
                    .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                    .colorBlendOp = VK_BLEND_OP_ADD,
                    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                    .alphaBlendOp = VK_BLEND_OP_ADD,
                    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };
    VkPipelineColorBlendAttachmentState color_blend_attachment_array[1] = {color_blend_attachment};
    VkPipelineLayout pipeline_layout;
    handle_error(vkCreatePipelineLayout( device,
                    &(VkPipelineLayoutCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .setLayoutCount = 0,
                        .pSetLayouts = NULL,
                        .pushConstantRangeCount = 0,
                        .pPushConstantRanges = NULL,
                    },
                    NULL,
                    &pipeline_layout
    ), destroy_fragment_shader_module);
    VkPipeline pipeline; // The big one
    handle_error(vkCreateGraphicsPipelines( device,
                    VK_NULL_HANDLE,
                    1,
                    &(VkGraphicsPipelineCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .stageCount = 2,
                        .pStages = shader_stage_array,
                        .pVertexInputState = &(VkPipelineVertexInputStateCreateInfo) {
                            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0x0,
                            .vertexBindingDescriptionCount = 0,
                            .pVertexBindingDescriptions = NULL,
                            .vertexAttributeDescriptionCount = 0,
                            .pVertexAttributeDescriptions = NULL
                        },
                        .pInputAssemblyState = &(VkPipelineInputAssemblyStateCreateInfo) {
                            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0x0,
                            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                            .primitiveRestartEnable = VK_FALSE
                        },
                        .pTessellationState = VK_NULL_HANDLE,
                        .pViewportState = &(VkPipelineViewportStateCreateInfo) {
                            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0x0,
                            .viewportCount = 1,
                            .pViewports = &viewport,
                            .scissorCount = 1,
                            .pScissors = &scissor
                        },
                        .pRasterizationState = &(VkPipelineRasterizationStateCreateInfo) {
                            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0x0,
                            .depthClampEnable = VK_FALSE,
                            .rasterizerDiscardEnable = VK_FALSE,
                            .polygonMode = VK_POLYGON_MODE_FILL,
                            .cullMode = VK_CULL_MODE_BACK_BIT,
                            .frontFace = VK_FRONT_FACE_CLOCKWISE,
                            .depthBiasEnable = VK_FALSE,
                            .depthBiasConstantFactor = 0.0f,
                            .depthBiasClamp = 0.0f,
                            .depthBiasSlopeFactor = 0.0f,
                            .lineWidth = 1.0f
                        },
                        .pMultisampleState = &(VkPipelineMultisampleStateCreateInfo) {
                            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0x0,
                            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                            .sampleShadingEnable = VK_FALSE,
                            .minSampleShading = 1.0f,
                            .pSampleMask = NULL,
                            .alphaToCoverageEnable = VK_FALSE,
                            .alphaToOneEnable = VK_FALSE
                        },
                        .pDepthStencilState = NULL,
                        .pColorBlendState = &(VkPipelineColorBlendStateCreateInfo) {
                            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0x0,
                            .logicOpEnable = VK_FALSE,
                            .logicOp = VK_LOGIC_OP_COPY,
                            .attachmentCount = 1,
                            .pAttachments = color_blend_attachment_array,
                            .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
                        },
                        .pDynamicState = &(VkPipelineDynamicStateCreateInfo) {
                            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0x0,
                            .dynamicStateCount = 0,
                            .pDynamicStates = NULL
                        },
                        .layout = pipeline_layout,
                        .renderPass = render_pass,
                        .subpass = 0,
                        .basePipelineHandle = VK_NULL_HANDLE,
                        .basePipelineIndex = -1
                    },
                    NULL,
                    &pipeline
    ), destroy_pipeline_layout);
    printf("%s", "Pipeline created\n");
    handle_error(vkBeginCommandBuffer( command_buffer_array[0],
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
    ), destroy_pipeline);
    VkClearValue clear_color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    VkClearValue *clear_color_array = malloc(sizeof(VkClearValue) * image_count);
    for (int i = 1; i < image_count; i++) {
        clear_color_array[i] = clear_color;
    }
    vkCmdBeginRenderPass( command_buffer_array[0],
                    &(VkRenderPassBeginInfo) {
                        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                        .pNext = NULL,
                        .renderPass = render_pass,
                        .framebuffer = framebuffer,
                        .renderArea = (VkRect2D) {
                            .offset = {0, 0},
                            .extent = image_extent
                        },
                        .clearValueCount = image_count,
                        .pClearValues = clear_color_array
                    },
                    VK_SUBPASS_CONTENTS_INLINE
    );
    printf("%s", "Command buffer and render pass have begun\n");
    VkSemaphore image_available_semaphore;
    handle_error(vkCreateSemaphore(device,
                    &(VkSemaphoreCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0
                    },
                    NULL,
                    &image_available_semaphore
    ), destroy_pipeline);
    VkSemaphore render_finished_semaphore;
    handle_error(vkCreateSemaphore(device,
                    &(VkSemaphoreCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0
                    },
                    NULL,
                    &render_finished_semaphore
    ), destroy_image_semaphore);
    VkFence in_flight_fence;
    handle_error(vkCreateFence( device,
                    &(VkFenceCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0
                    },
                    NULL,
                    &in_flight_fence
    ), destroy_render_semaphore);
    int image_index;
    handle_error(vkAcquireNextImageKHR( device,
                    swapchain,
                    UINT64_MAX,
                    image_available_semaphore,
                    VK_NULL_HANDLE,
                    &image_index
    ), destroy_flight_fence);
    printf("%s", "Ready to submit commands\n");
    vkCmdBindPipeline(command_buffer_array[0], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdDraw(command_buffer_array[0], 3, 1, 0, 0);
    vkCmdEndRenderPass(command_buffer_array[0]);
    vkEndCommandBuffer(command_buffer_array[0]);
    VkPipelineStageFlags wait_stage_mask_array[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    vkQueueSubmit( queue,
                    1,
                    &(VkSubmitInfo) {
                        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                        .pNext = NULL,
                        .waitSemaphoreCount = 1,
                        .pWaitSemaphores = &image_available_semaphore,
                        .pWaitDstStageMask = wait_stage_mask_array,
                        .commandBufferCount = 1,
                        .pCommandBuffers = command_buffer_array,
                        .signalSemaphoreCount = 1,
                        .pSignalSemaphores = &render_finished_semaphore
                    },
                    in_flight_fence
    );
    printf("%s", "Commands submitted\n");
    vkWaitForFences(device, 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
    vkQueuePresentKHR( queue,
                    &(VkPresentInfoKHR) {
                        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                        .pNext = NULL,
                        .waitSemaphoreCount = 1,
                        .pWaitSemaphores = &render_finished_semaphore,
                        .swapchainCount = 1,
                        .pSwapchains = &swapchain,
                        .pImageIndices = &image_index,
                        .pResults = NULL
                    }
    );

    while (!glfwWindowShouldClose(window) && glfwGetMouseButton(window, 1) != GLFW_PRESS) {
        glfwPollEvents();
    }
    printf("Exiting normally!!\n\n");
destroy_flight_fence:
    vkDestroyFence(device, in_flight_fence, NULL);
destroy_render_semaphore:
    vkDestroySemaphore(device, render_finished_semaphore, NULL);
destroy_image_semaphore:
    vkDestroySemaphore(device, image_available_semaphore, NULL);
destroy_pipeline:
    vkDestroyPipeline(device, pipeline, NULL);
destroy_pipeline_layout:
    vkDestroyPipelineLayout(device, pipeline_layout, NULL);
destroy_fragment_shader_module:
    vkDestroyShaderModule(device, fragment_shader_module, NULL);
destroy_vertex_shader_module:
    vkDestroyShaderModule(device, vertex_shader_module, NULL);
destroy_frame_buffer:
    vkDestroyFramebuffer(device, framebuffer, NULL);
destroy_image_views:
    for (int i = 0; i < created_image_views; i++) {
        VkImageView image_view = image_view_array[i];
        vkDestroyImageView(device, image_view, NULL);
    }
destroy_image:
    vkDestroyImage(device, image, NULL);
destroy_render_pass:
    vkDestroyRenderPass(device, render_pass, NULL);
free_command_buffers:
    vkFreeCommandBuffers(device, command_pool, 1, command_buffer_array);
destroy_command_pool:
    vkDestroyCommandPool(device, command_pool, NULL);
destory_swapchain:
    vkDestroySwapchainKHR(device, swapchain, NULL);
destroy_surface:
    vkDestroySurfaceKHR(vulkan_instance, surface, NULL);
destroy_window:
    glfwDestroyWindow(window);
destory_device:
    vkDestroyDevice(device, NULL);
destroy_instance:
    vkDestroyInstance(vulkan_instance, NULL);
exit_GLFW:
    glfwTerminate();

    return error_code;
}
