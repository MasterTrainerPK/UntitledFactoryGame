#include <stdio.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

void error_handle_glfw(int e, const char* msg) {
    fprintf(stderr, "GLFW ERR: %d, MSG: %s", e, msg);
}

int main() {
    glfwInit();
    glfwSetErrorCallback(&error_handle_glfw);
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        perror("The program screams in panic as it cannot find your primary monitor");
        return -1;
    }
    GLFWwindow* window = NULL;
    glfwCreateWindow(1000, 1000, "Hello Window", NULL, window);
    if (!window) {
       perror("ERR: no window...");
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    printf("Program Exiting Normally\n");
    return 0;
}
