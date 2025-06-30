#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "error_handling.h"

void error_handle_glfw(int e, const char* msg) {
    fprintf(stderr, "GLFW ERR: %d, MSG: %s", e, msg);
}

struct graphics_buffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkBufferUsageFlagBits usage;
    VkMemoryPropertyFlagBits properties;
    unsigned long long size;
};

struct graphics_state {
    VkResult last_error;
    VkExtensionProperties* extension_array;
    uint32_t extension_num;
    VkInstance instance;
    VkPhysicalDevice* physical_device_array;
    uint32_t physical_device_len;
    VkPhysicalDevice physical_device;
    int queue_family_index;
    VkDevice device;
    GLFWmonitor* monitor;
    GLFWwindow* window;
    VkSurfaceKHR surface;
    VkExtent2D image_extent;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR present_mode;
    VkSwapchainKHR swapchain;
    VkQueue queue;
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkRenderPass render_pass;
    VkImage* swapchain_image_array;
    uint32_t swapchain_image_len;
    VkImageView* swapchain_image_view_array;
    uint32_t swapchain_image_view_len;
    VkSemaphore* swapchain_image_available_semaphore_array;
    uint32_t swapchain_image_available_semaphore_len;
    VkSemaphore* swapchain_render_finished_semaphore_array;
    uint32_t swapchain_render_finished_semaphore_len;
    VkFence* swapchain_in_flight_fence_array;
    uint32_t swapchain_in_flight_fence_len;
    VkFramebuffer* framebuffer_array;
    uint32_t framebuffer_len;
    struct graphics_buffer* buffer_array;
    uint32_t buffer_len;
    VkShaderModule vertex_shader_module;
    VkShaderModule fragment_shader_module;
    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
};

int create_graphics_buffer(struct graphics_state *graphics_state, VkBufferUsageFlagBits usage, unsigned long long size, VkMemoryPropertyFlagBits memory_property_flags, struct graphics_buffer *graphics_buffer) {
    printf("%s", "Creating graphics buffer\n");
    int error_code = EXIT_SUCCESS;
    VkResult vk_result;

    graphics_buffer->size = size;
    graphics_buffer->usage = usage;

    handle_error(vkCreateBuffer(
        graphics_state->device,
        &(VkBufferCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0x0,
            .size = graphics_buffer->size,
            .usage = graphics_buffer->usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = NULL
        },
        NULL,
        &graphics_buffer->buffer
    ), exit_function);
    printf("%s", "Graphics buffer created\n");

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(graphics_state->device, graphics_buffer->buffer, &memory_requirements);

    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(graphics_state->physical_device, &memory_properties);

    int memory_index;
    for (memory_index = 0; memory_index < memory_properties.memoryTypeCount; memory_index++) {
        if ((memory_requirements.memoryTypeBits & (1 << memory_index)) && (memory_properties.memoryTypes[memory_index].propertyFlags & memory_property_flags) == memory_property_flags) {
            break;
        }
    }

    handle_error(vkAllocateMemory(
        graphics_state->device,
        &(VkMemoryAllocateInfo) {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = NULL,
            .allocationSize = memory_requirements.size,
            .memoryTypeIndex = memory_index
        },
        NULL,
        &graphics_buffer->memory
    ), destroy_buffer);
    printf("%s", "Graphics buffer allocated\n");

    handle_error(vkBindBufferMemory(graphics_state->device, graphics_buffer->buffer, graphics_buffer->memory, 0), free_buffer_memory);
    printf("%s", "Graphics buffer bound\n");

    graphics_state->buffer_array[graphics_state->buffer_len] = *graphics_buffer;
    graphics_state->buffer_len += 1;

    return error_code;
free_buffer_memory:
    vkFreeMemory(graphics_state->device, graphics_buffer->memory, NULL);
destroy_buffer:
    vkDestroyBuffer(graphics_state->device, graphics_buffer->buffer, NULL);
exit_function:
    return error_code;
}

int recreate_swapchain(struct graphics_state *graphics_state) {
    printf("%s", "Recreating swapchain\n");
    int error_code = EXIT_SUCCESS;
    VkResult vk_result;

    handle_error(vkDeviceWaitIdle(graphics_state->device), exit_function);

    for (int i = 0; i < graphics_state->framebuffer_len; i++) {
        vkDestroyFramebuffer(graphics_state->device, graphics_state->framebuffer_array[i], NULL);
    }
    graphics_state->framebuffer_len = 0;

    for(int i = 0; i < graphics_state->swapchain_image_view_len; i++) {
        vkDestroyImageView(graphics_state->device,  graphics_state->swapchain_image_view_array[i], NULL);
    }
    graphics_state->swapchain_image_view_len = 0;

    int width, height;
    glfwGetFramebufferSize(graphics_state->window, &width, &height);
    graphics_state->image_extent = (VkExtent2D) {
        .width = width,
        .height = height
    };
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(graphics_state->physical_device, graphics_state->surface, &graphics_state->surface_capabilities);

    VkSwapchainKHR old_swapchain = graphics_state->swapchain;
    handle_error(vkCreateSwapchainKHR(
        graphics_state->device,
        &(VkSwapchainCreateInfoKHR) {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = NULL,
            .flags = 0x0,
            .surface = graphics_state->surface,
            .minImageCount = graphics_state->swapchain_image_len,
            .imageFormat = graphics_state->surface_format.format,
            .imageColorSpace = graphics_state->surface_format.colorSpace,
            .imageExtent = graphics_state->image_extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = NULL,
            .preTransform = graphics_state->surface_capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = graphics_state->present_mode,
            .clipped = VK_TRUE,
            .oldSwapchain = old_swapchain
        },
        NULL,
        &graphics_state->swapchain
    ), exit_function);
    vkDestroySwapchainKHR(graphics_state->device, old_swapchain, NULL);
    printf("%s", "Swapchain created\n");

    graphics_state->swapchain_image_array = malloc(sizeof(VkImage) * graphics_state->swapchain_image_len);
    vkGetSwapchainImagesKHR(graphics_state->device, graphics_state->swapchain, &graphics_state->swapchain_image_len, graphics_state->swapchain_image_array);

    graphics_state->swapchain_image_view_array = malloc(sizeof(VkImageView) * graphics_state->swapchain_image_len);
    for(graphics_state->swapchain_image_view_len = 0; graphics_state->swapchain_image_view_len < graphics_state->swapchain_image_len; graphics_state->swapchain_image_view_len++) {
        VkImageView image_view;
        handle_error(vkCreateImageView(
            graphics_state->device,
            &(VkImageViewCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = NULL,
                .flags = 0x0,
                .image = graphics_state->swapchain_image_array[graphics_state->swapchain_image_view_len],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = graphics_state->surface_format.format,
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
        ), destroy_swapchain);
        graphics_state->swapchain_image_view_array[graphics_state->swapchain_image_view_len] = image_view;
    }
    printf("%s", "Image views created\n");

    graphics_state->framebuffer_array = malloc(sizeof(VkFramebuffer) * graphics_state->swapchain_image_len);
    for (graphics_state->framebuffer_len = 0; graphics_state->framebuffer_len < graphics_state->swapchain_image_len; graphics_state->framebuffer_len++) {
        VkFramebuffer framebuffer;
        handle_error(vkCreateFramebuffer(
            graphics_state->device,
            &(VkFramebufferCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = NULL,
                .flags = 0x0,
                .renderPass = graphics_state->render_pass,
                .attachmentCount = 1,
                .pAttachments = &graphics_state->swapchain_image_view_array[graphics_state->framebuffer_len],
                .width = graphics_state->image_extent.width,
                .height = graphics_state->image_extent.height, 
                .layers = 1
            },
            NULL,
            &framebuffer
        ), destroy_image_views);
        graphics_state->framebuffer_array[graphics_state->framebuffer_len] = framebuffer;
    }
    printf("%s", "Frame buffers created\n");

    return error_code;
destroy_image_views:
    for(int i = 0; i < graphics_state->swapchain_image_view_len; i++) {
        VkImageView image_view = graphics_state->swapchain_image_view_array[i];
        vkDestroyImageView(graphics_state->device, image_view, NULL);
    }
destroy_swapchain:
    vkDestroySwapchainKHR(graphics_state->device, graphics_state->swapchain, NULL);
exit_function:
    return error_code;
}

int create_graphics_state(struct graphics_state *graphics_state) {
    int error_code = EXIT_SUCCESS;
    VkResult vk_result;

    glfwInit();
    glfwSetErrorCallback(&error_handle_glfw);
    if(!glfwVulkanSupported()) {
        perror("ERR: GLFW+Vulkan not supported");
        error_code = EXIT_FAILURE;
        goto exit_GLFW;
    }

    uint32_t version = 0;
    vkEnumerateInstanceVersion(&version);
    if(version < VK_VERSION_1_3) {
        perror("ERR: Vulkan 1.3 or above required");
        error_code = EXIT_FAILURE;
        goto exit_GLFW;
    }

    uint32_t glfw_extension_len;
    glfwGetRequiredInstanceExtensions(&glfw_extension_len);
    const char** glfw_extension_array = malloc(sizeof(char*) * glfw_extension_len);
    glfw_extension_array = glfwGetRequiredInstanceExtensions(&glfw_extension_len);
    //extensions[instance_extension_count] = "VK_KHR_swapchain";
    printf("%s", "GLFW initialized\n");
    
    uint32_t layer_count = 1;
    const char* layers[1] = {"VK_LAYER_KHRONOS_validation"};
    handle_error(vkCreateInstance(
        &(VkInstanceCreateInfo) {
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
            .enabledLayerCount = layer_count,
            .ppEnabledLayerNames = layers,
            .enabledExtensionCount = glfw_extension_len,
            .ppEnabledExtensionNames = glfw_extension_array
        }, 
        NULL, 
        &graphics_state->instance
    ), exit_GLFW);
    printf("%s", "Instance created\n");

    {
        // order of priority is descrete, integrated, virtual, cpu, other
        int type_to_prio[5] = { 4, 1, 0, 2, 3 };
        handle_error(vkEnumeratePhysicalDevices(graphics_state->instance, &graphics_state->physical_device_len, NULL), destroy_instance);
        graphics_state->physical_device_array = malloc(sizeof(VkPhysicalDevice)*graphics_state->physical_device_len);
        handle_error(vkEnumeratePhysicalDevices(graphics_state->instance, &graphics_state->physical_device_len, graphics_state->physical_device_array), destroy_instance);
        int best_i = 0;
        int best_prio = 5;
        for(int i = 0; i < graphics_state->physical_device_len; i++) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(graphics_state->physical_device_array[i], &properties);
            if(type_to_prio[properties.deviceType] < best_prio) {
                best_i = i;
                best_prio = type_to_prio[properties.deviceType];
                if(best_prio == 0) {
                    break;
                }
            }
        }
        graphics_state->physical_device = graphics_state->physical_device_array[best_i];
        free(graphics_state->physical_device_array);
    }

    uint32_t queue_family_num;
    vkGetPhysicalDeviceQueueFamilyProperties(graphics_state->physical_device, &queue_family_num, NULL);
    VkQueueFamilyProperties* queue_family_properties = malloc(sizeof(VkQueueFamilyProperties)*queue_family_num);
    vkGetPhysicalDeviceQueueFamilyProperties(graphics_state->physical_device, &queue_family_num, queue_family_properties);
    for(graphics_state->queue_family_index = 0;
        !(queue_family_properties[graphics_state->queue_family_index].queueFlags | VK_QUEUE_GRAPHICS_BIT); graphics_state->queue_family_index++) {
        if(graphics_state->queue_family_index == queue_family_num) {
            perror("ERR: Can't find graphics queue!!");
            error_code = EXIT_FAILURE;
            goto destroy_instance;
        }
    }

    const char* const device_extension[1] = {"VK_KHR_swapchain"};
    handle_error(vkCreateDevice(
        graphics_state->physical_device,
        &(VkDeviceCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0x0,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &(VkDeviceQueueCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0x0,
                .queueFamilyIndex = graphics_state->queue_family_index,
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
        &graphics_state->device
    ), destroy_instance);
    printf("%s", "Device created\n");

    printf("%s", "Loading extensions\n");
    vkEnumerateDeviceExtensionProperties(graphics_state->physical_device, NULL, &graphics_state->extension_num, NULL);
    graphics_state->extension_array = malloc(sizeof(VkExtensionProperties) * graphics_state->extension_num);
    vkEnumerateDeviceExtensionProperties(graphics_state->physical_device, NULL, &graphics_state->extension_num, graphics_state->extension_array);
    for(int i = 0; i < graphics_state->extension_num; i++) {
        printf("%s\n", graphics_state->extension_array[i].extensionName);
    }

    graphics_state->monitor = glfwGetPrimaryMonitor();
    //FIXME: give this a fallback
    if(!graphics_state->monitor) {
        perror("ERR: The program cannot find your primary monitor");
        error_code = -1;
        goto destory_device;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    graphics_state->window = glfwCreateWindow(1000, 1000, "Hello Window", NULL, NULL);
    if(graphics_state->window == NULL) {
        perror("ERR: no window...");
        error_code = -1;
        goto destroy_window;
    }

    graphics_state->surface;
    handle_error(glfwCreateWindowSurface(
        graphics_state->instance,
        graphics_state->window,
        NULL,
        &graphics_state->surface
    ), destroy_window);
    printf("%s", "Surface created\n");

    int width, height;
    glfwGetFramebufferSize(graphics_state->window, &width, &height);
    graphics_state->image_extent = (VkExtent2D) {
        .width = width,
        .height = height
    };

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(graphics_state->physical_device, graphics_state->surface, &graphics_state->surface_capabilities);

    uint32_t surface_format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(graphics_state->physical_device, graphics_state->surface, &surface_format_count, NULL);
    VkSurfaceFormatKHR *surface_format_array = malloc(sizeof(VkSurfaceFormatKHR) * surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(graphics_state->physical_device, graphics_state->surface, &surface_format_count, surface_format_array);
    graphics_state->surface_format = surface_format_array[0];

    for(int i = 0; i < surface_format_count; i++) {
        VkSurfaceFormatKHR available_surface_format = surface_format_array[i];
        if(available_surface_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            graphics_state->surface_format = available_surface_format;
            break;
        }
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(graphics_state->physical_device, graphics_state->surface, &present_mode_count, NULL);
    VkPresentModeKHR *present_mode_array = malloc(sizeof(VkPresentModeKHR) * present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(graphics_state->physical_device, graphics_state->surface, &present_mode_count, present_mode_array);
    graphics_state->present_mode = present_mode_array[0];

    for(int i = 0; i < present_mode_count; i++) {
        VkPresentModeKHR available_present_mode = present_mode_array[i];
        if(available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            graphics_state->present_mode = available_present_mode;
            break;
        }
    }

    graphics_state->swapchain_image_len = graphics_state->surface_capabilities.minImageCount;
    handle_error(vkCreateSwapchainKHR(
        graphics_state->device,
        &(VkSwapchainCreateInfoKHR) {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = NULL,
            .flags = 0x0,
            .surface = graphics_state->surface,
            .minImageCount = graphics_state->swapchain_image_len,
            .imageFormat = graphics_state->surface_format.format,
            .imageColorSpace = graphics_state->surface_format.colorSpace,
            .imageExtent = graphics_state->image_extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = NULL,
            .preTransform = graphics_state->surface_capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = graphics_state->present_mode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE
        },
        NULL,
        &graphics_state->swapchain
    ), destroy_surface);
    printf("%s", "Swapchain created\n");

    graphics_state->swapchain_image_array = malloc(sizeof(VkImage) * graphics_state->swapchain_image_len);
    vkGetSwapchainImagesKHR(graphics_state->device, graphics_state->swapchain, &graphics_state->swapchain_image_len, graphics_state->swapchain_image_array);

    graphics_state->swapchain_image_view_array = malloc(sizeof(VkImageView) * graphics_state->swapchain_image_len);
    for(graphics_state->swapchain_image_view_len = 0; graphics_state->swapchain_image_view_len < graphics_state->swapchain_image_len; graphics_state->swapchain_image_view_len++) {
        VkImageView image_view;
        handle_error(vkCreateImageView(
            graphics_state->device,
            &(VkImageViewCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = NULL,
                .flags = 0x0,
                .image = graphics_state->swapchain_image_array[graphics_state->swapchain_image_view_len],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = graphics_state->surface_format.format,
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
        graphics_state->swapchain_image_view_array[graphics_state->swapchain_image_view_len] = image_view;
    }
    printf("%s", "Image views created\n");

    vkGetDeviceQueue(graphics_state->device, graphics_state->queue_family_index, 0, &graphics_state->queue);
    printf("%s", "Queue created\n");

    handle_error(vkCreateCommandPool(
        graphics_state->device,
        &(VkCommandPoolCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = NULL,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = graphics_state->queue_family_index
        },
        NULL,
        &graphics_state->command_pool
    ), destory_swapchain);
    printf("%s", "Command pool created\n");

    handle_error(vkAllocateCommandBuffers(
        graphics_state->device,
        &(VkCommandBufferAllocateInfo) {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = NULL,
            .commandPool = graphics_state->command_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        },
        &graphics_state->command_buffer
    ), destroy_command_pool);
    printf("%s", "Command buffer allocated\n");

    VkAttachmentDescription attachment_description = (VkAttachmentDescription) {
        .flags = 0x0,
        .format = graphics_state->surface_format.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };
    VkAttachmentDescription *attachment_description_array = malloc(sizeof(VkAttachmentDescription) * graphics_state->swapchain_image_len);
    for(int i = 0; i < graphics_state->swapchain_image_len; i++) {
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

    handle_error(vkCreateRenderPass(
        graphics_state->device,
        &(VkRenderPassCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = NULL,
            .flags = 0x0,
            .attachmentCount = 1,
            .pAttachments = attachment_description_array,
            .subpassCount = 1,
            .pSubpasses = subpass_description_array,
            .dependencyCount = 1,
            .pDependencies = subpass_dependency_array
        },
        NULL,
        &graphics_state->render_pass
    ), free_command_buffers);
    printf("%s", "Render pass created\n");

    graphics_state->framebuffer_array = malloc(sizeof(VkFramebuffer) * graphics_state->swapchain_image_len);
    for (graphics_state->framebuffer_len = 0; graphics_state->framebuffer_len < graphics_state->swapchain_image_len; graphics_state->framebuffer_len++) {
        VkFramebuffer framebuffer;
        handle_error(vkCreateFramebuffer(
            graphics_state->device,
            &(VkFramebufferCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = NULL,
                .flags = 0x0,
                .renderPass = graphics_state->render_pass,
                .attachmentCount = 1,
                .pAttachments = &graphics_state->swapchain_image_view_array[graphics_state->framebuffer_len],
                .width = graphics_state->image_extent.width,
                .height = graphics_state->image_extent.height, 
                .layers = 1
            },
            NULL,
            &framebuffer
        ), destroy_framebuffers);
        graphics_state->framebuffer_array[graphics_state->framebuffer_len] = framebuffer;
    }
    printf("%s", "Frame buffers created\n");

    FILE *f_model = fopen("cube.obj", "r");

    int vertex_count = 36;
    int vertex_dim = 3;
    int vertex_color = 3;
    int vertex_size = vertex_dim + vertex_color;
    //float vertex_1[4] = {0.0f, -0.5f, 0.0f, 1.0f};
    //float vertex_2[4] = {0.5f, 0.5f, 0.0f, 1.0f};
    //float vertex_3[4] = {-0.5f, 0.5f, 0.0f, 1.0f};
    //float *model_data[3] = {vertex_1, vertex_2, vertex_3};
    //float vertices[12] = {
    //    0.0f, -0.5f, 0.0f, 1.0f,
    //    0.5f, 0.5f, 0.0f, 1.0f,
    //    -0.5f, 0.5f, 0.0f, 1.0f };

    VkVertexInputBindingDescription vertex_binding_description = (VkVertexInputBindingDescription) {
        .binding = 0,
        .stride = sizeof(float) * vertex_size,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkVertexInputAttributeDescription vertex_attribute_description_position = (VkVertexInputAttributeDescription) {
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .binding = 0,
        .offset = 0
    };

    VkVertexInputAttributeDescription vertex_attribute_description_color = (VkVertexInputAttributeDescription) {
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .binding = 0,
        .offset = sizeof(float) * vertex_dim
    };

    VkVertexInputAttributeDescription vertex_attribute_description_array[2] = {vertex_attribute_description_position, vertex_attribute_description_color};

    FILE *f_vertex = fopen("shaders/vert.spv", "rb");
    if(f_vertex == NULL) {
        perror("failed to open file");
        goto destroy_framebuffers;
    }
    fseek(f_vertex, 0, SEEK_END);
    long fsize_vertex = ftell(f_vertex);
    fseek(f_vertex, 0, SEEK_SET);
    uint32_t *vertex_shader_code = malloc(sizeof(uint32_t) * fsize_vertex);
    fread(vertex_shader_code, sizeof(uint32_t), fsize_vertex, f_vertex);
    fclose(f_vertex);

    handle_error(vkCreateShaderModule(
        graphics_state->device,
        &(VkShaderModuleCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0x0,
            .codeSize = fsize_vertex,
            .pCode = vertex_shader_code
        },
        NULL,
        &graphics_state->vertex_shader_module
    ), destroy_framebuffers);
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

    handle_error(vkCreateShaderModule(
        graphics_state->device,
        &(VkShaderModuleCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0x0,
            .codeSize = fsize_fragment,
            .pCode = fragment_shader_code
        },
        NULL,
        &graphics_state->fragment_shader_module
    ), destroy_vertex_shader_module);

    VkPipelineShaderStageCreateInfo vertex_shader_stage = (VkPipelineShaderStageCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0x0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = graphics_state->vertex_shader_module,
        .pName = "main",
        .pSpecializationInfo = NULL
    };

    VkPipelineShaderStageCreateInfo fragment_shader_stage = (VkPipelineShaderStageCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0x0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = graphics_state->fragment_shader_module,
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
        .extent = graphics_state->image_extent
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

    handle_error(vkCreatePipelineLayout(
        graphics_state->device,
        &(VkPipelineLayoutCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = NULL,
            .flags = 0x0,
            .setLayoutCount = 0,
            .pSetLayouts = NULL,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &(VkPushConstantRange) {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(float) * 16
            },
        },
        NULL,
        &graphics_state->pipeline_layout
    ), destroy_fragment_shader_module);

    handle_error(vkCreateGraphicsPipelines( // The big one
        graphics_state->device,
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
                .vertexBindingDescriptionCount = 1,
                .pVertexBindingDescriptions = &vertex_binding_description,
                .vertexAttributeDescriptionCount = 2,
                .pVertexAttributeDescriptions = vertex_attribute_description_array
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
            .layout = graphics_state->pipeline_layout,
            .renderPass = graphics_state->render_pass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        },
        NULL,
        &graphics_state->pipeline
    ), destroy_pipeline_layout);

    printf("%s", "Pipeline created\n");

    graphics_state->swapchain_image_available_semaphore_array = malloc(sizeof(VkSemaphore) * graphics_state->swapchain_image_len);
    for (graphics_state->swapchain_image_available_semaphore_len = 0; graphics_state->swapchain_image_available_semaphore_len < graphics_state->swapchain_image_len; graphics_state->swapchain_image_available_semaphore_len++) {
        VkSemaphore image_available_semaphore;
        handle_error(vkCreateSemaphore(
            graphics_state->device,
            &(VkSemaphoreCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0x0
            },
            NULL,
            &image_available_semaphore
        ), destroy_pipeline);
        graphics_state->swapchain_image_available_semaphore_array[graphics_state->swapchain_image_available_semaphore_len] = image_available_semaphore;
    }

    graphics_state->swapchain_render_finished_semaphore_array = malloc(sizeof(VkSemaphore) * graphics_state->swapchain_image_len);
    for (graphics_state->swapchain_render_finished_semaphore_len = 0; graphics_state->swapchain_render_finished_semaphore_len < graphics_state->swapchain_image_len; graphics_state->swapchain_render_finished_semaphore_len++) {
        VkSemaphore render_finished_semaphore;
        handle_error(vkCreateSemaphore(
            graphics_state->device,
            &(VkSemaphoreCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0x0
            },
            NULL,
            &render_finished_semaphore
        ), destroy_image_semaphores);
        graphics_state->swapchain_render_finished_semaphore_array[graphics_state->swapchain_render_finished_semaphore_len] = render_finished_semaphore;
    }

    graphics_state->swapchain_in_flight_fence_array = malloc(sizeof(VkFence) * graphics_state->swapchain_image_len);
    for (graphics_state->swapchain_in_flight_fence_len = 0; graphics_state->swapchain_in_flight_fence_len < graphics_state->swapchain_image_len; graphics_state->swapchain_in_flight_fence_len++) {
        VkFence in_flight_fence;
        handle_error(vkCreateFence(
            graphics_state->device,
            &(VkFenceCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .pNext = NULL,
                .flags = VK_FENCE_CREATE_SIGNALED_BIT
            },
            NULL,
            &in_flight_fence
        ), destroy_render_semaphores);
        graphics_state->swapchain_in_flight_fence_array[graphics_state->swapchain_in_flight_fence_len] = in_flight_fence;
    }
    printf("%s", "Sync objects created\n");

    graphics_state->buffer_array = malloc(sizeof(struct graphics_buffer) * 64);
    graphics_state->buffer_len = 0;

    return error_code;

destroy_flight_fences:
    for(int i = 0; i < graphics_state->swapchain_in_flight_fence_len; i++) {
        vkDestroyFence(graphics_state->device, graphics_state->swapchain_in_flight_fence_array[i], NULL);
    }
    graphics_state->swapchain_in_flight_fence_len = 0;
destroy_render_semaphores:
    for(int i = 0; i < graphics_state->swapchain_render_finished_semaphore_len; i++) {
        vkDestroySemaphore(graphics_state->device, graphics_state->swapchain_render_finished_semaphore_array[i], NULL);
    }
    graphics_state->swapchain_render_finished_semaphore_len = 0;
destroy_image_semaphores:
    for(int i = 0; i < graphics_state->swapchain_image_available_semaphore_len; i++) {
        vkDestroySemaphore(graphics_state->device, graphics_state->swapchain_image_available_semaphore_array[i], NULL);
    }
    graphics_state->swapchain_image_available_semaphore_len = 0;
destroy_pipeline:
    vkDestroyPipeline(graphics_state->device, graphics_state->pipeline, NULL);
destroy_pipeline_layout:
    vkDestroyPipelineLayout(graphics_state->device, graphics_state->pipeline_layout, NULL);
destroy_fragment_shader_module:
    vkDestroyShaderModule(graphics_state->device, graphics_state->fragment_shader_module, NULL);
destroy_vertex_shader_module:
    vkDestroyShaderModule(graphics_state->device, graphics_state->vertex_shader_module, NULL);
destroy_framebuffers:
    for (int i = 0; i < graphics_state->framebuffer_len; i++) {
        vkDestroyFramebuffer(graphics_state->device, graphics_state->framebuffer_array[i], NULL);
    }
    graphics_state->framebuffer_len = 0;
destroy_render_pass:
    vkDestroyRenderPass(graphics_state->device, graphics_state->render_pass, NULL);
free_command_buffers:
    vkFreeCommandBuffers(graphics_state->device, graphics_state->command_pool, 1, &graphics_state->command_buffer);
destroy_command_pool:
    vkDestroyCommandPool(graphics_state->device, graphics_state->command_pool, NULL);
destroy_image_views:
    for(int i = 0; i < graphics_state->swapchain_image_view_len; i++) {
        vkDestroyImageView(graphics_state->device, graphics_state->swapchain_image_view_array[i], NULL);
    }
    graphics_state->swapchain_image_view_len = 0;
destory_swapchain:
    vkDestroySwapchainKHR(graphics_state->device, graphics_state->swapchain, NULL);
destroy_surface:
    vkDestroySurfaceKHR(graphics_state->instance, graphics_state->surface, NULL);
destroy_window:
    glfwDestroyWindow(graphics_state->window);
destory_device:
    vkDestroyDevice(graphics_state->device, NULL);
destroy_instance:
    vkDestroyInstance(graphics_state->instance, NULL);
exit_GLFW:
    glfwTerminate();
    return error_code;
}

void cleanup(struct graphics_state *graphics_state) {
    vkDeviceWaitIdle(graphics_state->device);
    for(int i = 0; i < graphics_state->swapchain_in_flight_fence_len; i++) {
        vkDestroyFence(graphics_state->device, graphics_state->swapchain_in_flight_fence_array[i], NULL);
    }
    graphics_state->swapchain_in_flight_fence_len = 0;
    for(int i = 0; i < graphics_state->swapchain_render_finished_semaphore_len; i++) {
        vkDestroySemaphore(graphics_state->device, graphics_state->swapchain_render_finished_semaphore_array[i], NULL);
    }
    graphics_state->swapchain_render_finished_semaphore_len = 0;
    for(int i = 0; i < graphics_state->swapchain_image_available_semaphore_len; i++) {
        vkDestroySemaphore(graphics_state->device, graphics_state->swapchain_image_available_semaphore_array[i], NULL);
    }
    graphics_state->swapchain_image_available_semaphore_len = 0;
    vkDestroyPipeline(graphics_state->device, graphics_state->pipeline, NULL);
    vkDestroyPipelineLayout(graphics_state->device, graphics_state->pipeline_layout, NULL);
    vkDestroyShaderModule(graphics_state->device, graphics_state->fragment_shader_module, NULL);
    vkDestroyShaderModule(graphics_state->device, graphics_state->vertex_shader_module, NULL);
    for (int i = 0; i < graphics_state->buffer_len; i++) {
        vkFreeMemory(graphics_state->device, graphics_state->buffer_array[i].memory, NULL);
        vkDestroyBuffer(graphics_state->device, graphics_state->buffer_array[i].buffer, NULL);
    }
    graphics_state->buffer_len = 0;
    for (int i = 0; i < graphics_state->framebuffer_len; i++) {
        vkDestroyFramebuffer(graphics_state->device, graphics_state->framebuffer_array[i], NULL);
    }
    graphics_state->framebuffer_len = 0;
    vkDestroyRenderPass(graphics_state->device, graphics_state->render_pass, NULL);
    vkFreeCommandBuffers(graphics_state->device, graphics_state->command_pool, 1, &graphics_state->command_buffer);
    vkDestroyCommandPool(graphics_state->device, graphics_state->command_pool, NULL);
    for(int i = 0; i < graphics_state->swapchain_image_view_len; i++) {
        vkDestroyImageView(graphics_state->device, graphics_state->swapchain_image_view_array[i], NULL);
    }
    graphics_state->swapchain_image_view_len = 0;
    vkDestroySwapchainKHR(graphics_state->device, graphics_state->swapchain, NULL);
    vkDestroySurfaceKHR(graphics_state->instance, graphics_state->surface, NULL);
    glfwDestroyWindow(graphics_state->window);
    vkDestroyDevice(graphics_state->device, NULL);
    vkDestroyInstance(graphics_state->instance, NULL);
    glfwTerminate();
}
