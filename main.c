#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

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

    uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    uint32_t layer_count = 1;
    const char* layers[1] = {"VK_LAYER_KHRONOS_validation"};
    VkInstance vulkan_instance;

    vk_result = vkCreateInstance(&(VkInstanceCreateInfo) {
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
            .apiVersion = VK_VERSION_1_3
        },
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL
    }, NULL, &vulkan_instance);
    //FIXME: Really bad error handling, maybe use something like a monad? or maybe engage in black magic one-line macros for filename and
    if (vk_result != VK_SUCCESS) {
        switch (vk_result) {
        case VK_ERROR_LAYER_NOT_PRESENT:
            perror("ERR: Layer not found");
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            perror("ERR: Extension not found");
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            perror("ERR: Incompatible or Missing Driver");
            break;
        default:
            perror("ERR: Function not up to spec or u ran out of memory????");
        }
        perror("ERR: vulkan_instance failed");
        error_code = EXIT_FAILURE;
        goto exit_GLFW;
    };
    uint32_t num_physical_devices = 1;
    VkPhysicalDevice physical_device = malloc(sizeof(VkPhysicalDevice)*num_physical_devices);
    vk_result = vkEnumeratePhysicalDevices(vulkan_instance, &num_physical_devices, &physical_device);
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
            goto exit_vulkan;
        }
    }
    VkDevice device;
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
    );

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    //FIXME: give this a fallback
    if (!monitor) {
        perror("ERR: The program cannot find your primary monitor");
        error_code = -1;
        goto exit_GLFW;
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
                        .imageFormat = VK_FORMAT_UNDEFINED,
                        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                        .imageExtent = image_extent,
                        .imageArrayLayers = 1,
                        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                        .imageSharingMode = VK_SHARING_MODE_CONCURRENT,
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
                        .format = VK_IMAGE_ASPECT_COLOR_BIT,
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
    VkFramebuffer framebuffer;
    vkCreateFrameBuffer( device,
                    &(VkFramebufferCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0x0,
                        .renderPass = render_pass,
                        .attachmentCount = 1,
                        .pAttachments = attachment_description_array,
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

    while (!glfwWindowShouldClose(window) && glfwGetMouseButton(window, 1) != GLFW_PRESS) {
        glfwPollEvents();
    }
    printf("Exiting normally!!\n\n");
exit_window:
    glfwDestroyWindow(window);
exit_vulkan:
    vkFreeCommandBuffers(device, command_pool, 1, command_buffer_array);
    vkDestroyInstance(vulkan_instance, NULL);
exit_GLFW:
    glfwTerminate();

    return error_code;
}
