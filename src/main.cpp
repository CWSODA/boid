#include <iostream>
#include <vector>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "win_manager.hpp"
#include "lib.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "bird.hpp"
#include "circle.hpp"
#include "obstacle.hpp"

constexpr int START_WIDTH = 800;
constexpr int START_HEIGHT = 600;

constexpr unsigned int BIRD_COUNT = 50;
constexpr unsigned int FLOCK_COUNT = 4;
constexpr float fps_cooldown = 0.25f;

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW." << std::endl;
        return -1;
    };

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(START_WIDTH, START_HEIGHT, "Boids", NULL, NULL);
    if (!window) {
        const char* description;
        int code = glfwGetError(&description);
        printf("GLFW Error: %s (Code %d)\n", description, code);
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    WinData::instance().window = window;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    gladLoadGL();

    /* END OF WINDOW CREATION */

    /* SETUP IMGUI */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // Bird Model Setup
    unsigned int bird_VAO, bird_VBO, bird_EBO;
    glGenVertexArrays(1, &bird_VAO);
    glGenBuffers(1, &bird_VBO);
    glGenBuffers(1, &bird_EBO);

    float vertices[] = {
        0.0f,  1.0f,   // top
        -0.7f, -1.0f,  // bottom left
        0.0f,  -0.3f,  // bottom mid
        0.7f,  -1.0f,  // bottom right
    };
    unsigned int indices[] = {
        0, 1, 2,  // left triangle
        0, 2, 3,  // right triangle
    };

    glBindVertexArray(bird_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, bird_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bird_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    // Shader Setup
    Shader bird_shader("../src/shaders/vs/2d_simple.vert",
                       "../src/shaders/fs/single_color.frag");

    // Initiate Birds
    std::vector<std::vector<Bird>> flocks =
        spawn_birds(BIRD_COUNT, FLOCK_COUNT);
    std::vector<glm::vec3> bird_colors = {
        glm::vec3(0.1f, 1.0f, 0.1f), glm::vec3(1.0f, 0.1f, 0.1f),
        glm::vec3(1.0f, 1.0f, 0.1f), glm::vec3(0.1f, 1.0f, 1.0f),
        glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.1f, 1.0f, 0.1f),
    };

    // Circle Cursor
    CircleMesh circle = Circle::get_circle(32);
    // Border Stand-In Circle
    CircleMesh square = Circle::get_circle(4);

    // Obstacles
    {
        ivec3_hashset seen_points;
        std::vector<glm::vec3> obstacles;
        std::vector<std::array<glm::vec3, 2>> point_pairs = {
            {glm::vec3(-60.0f, 60.0f, 0.0f),
             glm::vec3(60.0f, 60.0f, 0.0f)},  // T
            {glm::vec3(-60.0f, -60.0f, 0.0f),
             glm::vec3(60.0f, -60.0f, 0.0f)},  // B
            {glm::vec3(-60.0f, 60.0f, 0.0f),
             glm::vec3(-60.0f, -60.0f, 0.0f)},  // L
            {glm::vec3(60.0f, 60.0f, 0.0f),
             glm::vec3(60.0f, -60.0f, 0.0f)}};  // R
        for (const auto& pairs : point_pairs) {
            std::vector<glm::vec3> new_points =
                gen_points(pairs[0], pairs[1], seen_points);
            obstacles.insert(obstacles.end(), new_points.begin(),
                             new_points.end());
        }
        BirdSimSettings::instance().obstacles = std::move(obstacles);
    }

    // make sure viewport is ok
    glfwGetFramebufferSize(window, &WinData::instance().width,
                           &WinData::instance().height);
    update_aspect();

    // glEnable(GL_DEPTH_TEST);
    Camera camera;
    float last_time = glfwGetTime();
    float fps_clock = 0.0f;
    float fps = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float current_time = glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;
        if (current_time > fps_clock) {
            fps = 1.0f / delta_time;
            fps_clock += fps_cooldown;
        }
        GlobalData::instance().delta_time = delta_time;

        glfwPollEvents();
        process_input(window);

        // IMGUI STUFF
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // GUI Setup
        ImGui::Begin("Settings");
        ImGui::Text("FPS: %.0f", fps);
        if (ImGui::Button("Reset")) {
            flocks = spawn_birds(BIRD_COUNT, FLOCK_COUNT);
        }
        glm::vec2 text_pos = get_cursor_world_pos();
        ImGui::Text("Cursor Position (%.2f, %.2f)",
                    WinData::instance().cursor_x, WinData::instance().cursor_y);
        ImGui::Text("Cursor World Position (%.2f, %.2f)", text_pos.x,
                    text_pos.y);
        ImGui::Text("Window Size (%d, %d)", WinData::instance().width,
                    WinData::instance().height);
        constexpr float slider_speed = 0.1f;
        std::string slider_format = "%.1f";
        ImGui::PushItemWidth(50.0f);
        ImGui::DragFloat("Bird Speed", &BirdSimSettings::instance().speed,
                         slider_speed, 0.0f, FLT_MAX, slider_format.c_str());
        ImGui::DragFloat("Cohesion Strength",
                         &BirdSimSettings::instance().cohesion_strength,
                         slider_speed, 0.0f, FLT_MAX, slider_format.c_str());
        ImGui::DragFloat("Alignment Strength",
                         &BirdSimSettings::instance().alignment_strength,
                         slider_speed, 0.0f, FLT_MAX, slider_format.c_str());
        ImGui::DragFloat("Seperation Strength",
                         &BirdSimSettings::instance().seperation_strength,
                         slider_speed, 0.0f, FLT_MAX, slider_format.c_str());
        ImGui::DragFloat("Noise", &BirdSimSettings::instance().noise_strength,
                         slider_speed, 0.0f, FLT_MAX, slider_format.c_str());
        ImGui::DragFloat("Noise Switching",
                         &BirdSimSettings::instance().noise_tgt_switching,
                         slider_speed, 0.0f, FLT_MAX, slider_format.c_str());
        ImGui::DragFloat("Obstacle Repulsion",
                         &BirdSimSettings::instance().obstacle_repulsion,
                         slider_speed, 0.0f, FLT_MAX, slider_format.c_str());
        ImGui::Checkbox("Follow Cursor",
                        &BirdSimSettings::instance().is_follow_cursor);
        ImGui::DragFloat("Follow Cursor Strength",
                         &BirdSimSettings::instance().follow_cursor_strength,
                         slider_speed, 0.0f, FLT_MAX, slider_format.c_str());
        ImGui::Checkbox("Pause", &GlobalData::instance().is_paused);
        ImGui::End();
        bool is_paused = GlobalData::instance().is_paused;

        float grey_shade = 0.7f;
        glClearColor(grey_shade, grey_shade, grey_shade, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // CALCULATE VIEW & PROJECTION
        float height = WinData::instance().base_height;
        float width = height * WinData::instance().aspect_ratio;
        glm::mat4 projection =
            glm::ortho<float>(-width / 2.0f, width / 2.0f,    // left and right
                              -height / 2.0f, height / 2.0f,  // bottom and top
                              -1.0f, 1.0f);  // back and front clipping
        glm::mat4 view = camera.get_view_mat();

        // UPDATE BIRDS
        if (!is_paused) {
            update_birds(flocks);
        }

        // DRAW BIRDS
        glBindVertexArray(bird_VAO);
        bird_shader.use();
        bird_shader.set_mat4("view", view);
        bird_shader.set_mat4("projection", projection);

        for (int i = 0; i < flocks.size(); i++) {
            bird_shader.set_vec3("color", bird_colors[i]);
            for (auto& bird : flocks[i]) {
                if (!is_paused) {
                    bird.update_pos();
                }
                glm::mat4 bird_model = get_transform_mat(bird.position);
                bird_model = bird_model * dir2rotation_matrix(bird.direction);
                bird_shader.set_mat4("model", bird_model);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        // Draw cursor circle
        glm::vec2 pos = get_cursor_world_pos();
        bird_shader.set_mat4("model",
                             get_transform_mat(glm::vec3(pos.x, pos.y, 0.0f)));
        circle.draw();

        // Draw obstacles
        bird_shader.set_vec3("color", glm::vec3(0.1f));
        for (const auto& pos : BirdSimSettings::instance().obstacles) {
            bird_shader.set_mat4("model", get_transform_mat(pos));
            square.draw();
        }

        // IMGUI RENDER
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // IMGUI Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}