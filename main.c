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
#include "graphics_handling.h"

float *transform_vector(float *matrix, float *vector, int dimension) {
    float *new_vector = malloc(sizeof(float) * dimension);
    for (int a = 0; a < dimension; a++) {
        new_vector[a] = 0.0f;
        for (int b = 0; b < dimension; b++) {
            new_vector[a] += matrix[a * dimension + b] * vector[b];
        }
    }
    return new_vector;
}

int main() {
    int error_code = EXIT_SUCCESS;
    VkResult vk_result;

    struct graphics_state graphics;
    create_graphics_state(&graphics);

    int vertex_count = 36;
    int vertex_dim = 3;
    int vertex_color = 3;
    int vertex_size = vertex_dim + vertex_color;

    struct graphics_buffer vertex_buffer;
    create_graphics_buffer(&graphics, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, sizeof(float) * vertex_size * vertex_count, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertex_buffer);

    void *data;
    vkMapMemory(graphics.device, vertex_buffer.memory, 0, vertex_buffer.size, 0x0, &data);

    float vertices[216] = {
        //x, y, z, r, g, b

        // Front face
        0.5f,  0.5f,  -0.5f, 1.0f,  1.0f,  0.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f,

        // Back face
        0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  1.0f,

        // Right face
        0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  -0.5f, 1.0f,  1.0f,  0.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  1.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  -0.5f, 1.0f,  1.0f,  0.0f,

        // Left face
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

        // Top face
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  0.0f,

        // Bottom face
        0.5f,  0.5f,  -0.5f, 1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,   0.5f, 1.0f,  1.0f,  1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,
    };

    uint32_t current_frame = 0;

    struct timespec curr_time;
    const int dt = 10000000; // 1/100 of a sec
    long int accumulator = 0;
    long int t = 0;
    if(clock_gettime(CLOCK_MONOTONIC, &curr_time) != 0) {
            perror("failed to get time");
            goto cleanup_graphics;
    }
    while(!glfwWindowShouldClose(graphics.window) && glfwGetMouseButton(graphics.window, 1) != GLFW_PRESS) {
        struct timespec new_time;
        if(clock_gettime(CLOCK_MONOTONIC, &new_time) != 0) {
            perror("failed to get time during loop");
            goto cleanup_graphics;
        }
        int frame_time = new_time.tv_sec - curr_time.tv_sec;
        frame_time = new_time.tv_nsec - curr_time.tv_nsec + frame_time * 1000000000 ; // sec to ns
        //ass(frame_time >= 0);
        if(frame_time > 250000000) { // 1/4 a sec
            frame_time = 250000000;
        }
        curr_time = new_time;
        accumulator += frame_time;
        while(accumulator > dt) {
            // logic tick
            t += dt;
            accumulator -= dt;
        }
        const double alpha = (double)accumulator / dt;
        //lerp state and render state;
        //printf("%s", "Beginning new frame\n");

        vkWaitForFences(graphics.device, 1, &graphics.swapchain_in_flight_fence_array[current_frame], VK_TRUE, UINT64_MAX);
        vkResetFences(graphics.device, 1, &graphics.swapchain_in_flight_fence_array[current_frame]);
        vkResetCommandBuffer(graphics.command_buffer, 0x0);

        float camera_position[3] = {0.0f, 0.0f, 1.5f};

        double theta = ((double)curr_time.tv_nsec / 1000000000.0 + (double)curr_time.tv_sec);

        float rotation_matrix_x[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, (float)cos(theta), (float)cos(theta + 3.1415 * 0.5), 0.0f,
            0.0f, (float)sin(theta), (float)sin(theta + 3.1415 * 0.5), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        float rotation_matrix_y[16] = {
            (float)cos(theta), 0.0f, (float)cos(theta + 3.1415 * 0.5), 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            (float)sin(theta), 0.0f, (float)sin(theta + 3.1415 * 0.5), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        float rotation_matrix_z[16] = {
            (float)cos(theta), (float)cos(theta + 3.1415 * 0.5), 0.0f, 0.0f,
            (float)sin(theta), (float)sin(theta + 3.1415 * 0.5), 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        float transformed_vertices[252];

        // Carry over the color
        for (int i = 0; i < vertex_count; i++) {
            transformed_vertices[i * vertex_size + 3] = vertices[i * vertex_size + 3];
            transformed_vertices[i * vertex_size + 4] = vertices[i * vertex_size + 4];
            transformed_vertices[i * vertex_size + 5] = vertices[i * vertex_size + 5];
        }
        
        for (int a = 0; a < vertex_count; a++) {
            float *vector = malloc(sizeof(float) * vertex_dim);
            for (int b = 0; b < vertex_dim; b++) {
                vector[b] = vertices[a * vertex_size + b];
            }
            vector[vertex_dim] = 1.0f;

            float *new_vector = transform_vector(rotation_matrix_x, vector, vertex_dim + 1);
            new_vector = transform_vector(rotation_matrix_y, new_vector, vertex_dim + 1);
            new_vector = transform_vector(rotation_matrix_z, new_vector, vertex_dim + 1);
            for (int b = 0; b < vertex_dim; b++) {
                transformed_vertices[a * vertex_size + b] = new_vector[b] + camera_position[b];
            }
        }

        memcpy(data, transformed_vertices, vertex_buffer.size);

        handle_error(vkBeginCommandBuffer(
            graphics.command_buffer,
            &(VkCommandBufferBeginInfo) {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = NULL,
                .flags = 0x0,
                .pInheritanceInfo = &(VkCommandBufferInheritanceInfo) {
                    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
                    .pNext = NULL,
                    .renderPass = graphics.render_pass,
                    .subpass = 0,
                    .framebuffer = graphics.framebuffer_array[current_frame],
                    .occlusionQueryEnable = VK_TRUE,
                    .queryFlags = 0x0,
                    .pipelineStatistics = 0x0,
                }
            }
        ), cleanup_graphics);

        VkClearValue clear_color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        VkClearValue *clear_color_array = malloc(sizeof(VkClearValue) * graphics.swapchain_image_len);
        for(int i = 0; i < graphics.swapchain_image_len; i++) {
            clear_color_array[i] = clear_color;
        }

        vkCmdBeginRenderPass(
            graphics.command_buffer,
            &(VkRenderPassBeginInfo) {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = NULL,
                .renderPass = graphics.render_pass,
                .framebuffer = graphics.framebuffer_array[current_frame],
                .renderArea = (VkRect2D) {
                    .offset = {0, 0},
                    .extent = graphics.image_extent
                },
                .clearValueCount = graphics.swapchain_image_len,
                .pClearValues = clear_color_array
            },
            VK_SUBPASS_CONTENTS_INLINE
        );
        //printf("%s", "Command buffer and render pass have begun\n");

        uint32_t image_index;
        vkAcquireNextImageKHR(graphics.device, graphics.swapchain, UINT64_MAX, graphics.swapchain_image_available_semaphore_array[current_frame], VK_NULL_HANDLE, &image_index);
        //printf("%s", "Ready to submit commands\n");

        vkCmdBindPipeline(graphics.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics.pipeline);
        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(graphics.command_buffer, 0, 1, &vertex_buffer.buffer, offsets);
        vkCmdDraw(graphics.command_buffer, vertex_count, 1, 0, 0);
        vkCmdEndRenderPass(graphics.command_buffer);
        vkEndCommandBuffer(graphics.command_buffer);

        VkPipelineStageFlags wait_stage_mask_array[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        vkQueueSubmit(
           graphics.queue,
            1,
                &(VkSubmitInfo) {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext = NULL,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &graphics.swapchain_image_available_semaphore_array[current_frame],
                .pWaitDstStageMask = wait_stage_mask_array,
                .commandBufferCount = 1,
                .pCommandBuffers = &graphics.command_buffer,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = &graphics.swapchain_render_finished_semaphore_array[current_frame]
            },
            graphics.swapchain_in_flight_fence_array[current_frame]
        );
        //printf("%s", "Commands submitted\n");
        vkWaitForFences(graphics.device, 1, &graphics.swapchain_in_flight_fence_array[current_frame], VK_TRUE, UINT64_MAX);

        vkQueuePresentKHR(
            graphics.queue,
            &(VkPresentInfoKHR) {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .pNext = NULL,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &graphics.swapchain_render_finished_semaphore_array[current_frame],
                .swapchainCount = 1,
                .pSwapchains = &graphics.swapchain,
                .pImageIndices = &image_index,
                .pResults = NULL
            }
        );
        //printf("%s", "Image presented\n");

        current_frame = (current_frame + 1) % graphics.swapchain_image_len;

        glfwPollEvents();
    }

    printf("Exiting normally!!\n\n");
cleanup_graphics:
    cleanup(&graphics);
}
