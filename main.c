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
    VkQueue queue;
    vkGetDeviceQueue(device, queue_family_index, 0, &queue);

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
    while (!glfwWindowShouldClose(window) && glfwGetMouseButton(window, 1) != GLFW_PRESS) {
        glfwPollEvents();
    }
    printf("Exiting normally!!\n\n");
exit_window:
    glfwDestroyWindow(window);
exit_vulkan:
    vkDestroyInstance(vulkan_instance, NULL);
exit_GLFW:
    glfwTerminate();

    return error_code;
}
