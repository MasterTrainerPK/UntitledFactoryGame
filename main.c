#include <stdio.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

void error_handle_glfw(int e, const char* msg) {
    fprintf(stderr, "GLFW ERR: %d, MSG: %s", e, msg);
}

int main() {
    int error_code = 0;
    glfwInit();
    glfwSetErrorCallback(&error_handle_glfw);
    if (!glfwVulkanSupported()) {
        perror("ERR: GLFW+Vulkan not supported");
        goto exit_GLFW;
    }
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        perror("The program screams in panic as it cannot find your primary monitor");
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
    printf("Exiting normally!!");
exit_window:
    glfwDestroyWindow(window);
exit_GLFW:
    glfwTerminate();
    return error_code;
}
