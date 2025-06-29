#include "win_manager.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    WinData::instance().width = width;
    WinData::instance().height = height;
    update_aspect();
}

void mouse_scroll_callback(GLFWwindow* window, double x_scroll,
                           double y_scroll) {
    WinData::instance().base_height -= y_scroll;

    // set zoom limit
    if (WinData::instance().base_height < 5.0f) {
        WinData::instance().base_height = 5.0f;
    }
}

void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
    WinData::instance().cursor_x = xpos;
    WinData::instance().cursor_y = ypos;
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// Only works for 2D scenes
glm::vec2 get_cursor_world_pos() {
    // cursor pos is 0,0 at top left of window
    // base height is bottom of screen in world space

    float base_height = WinData::instance().base_height;
    float base_width = base_height * WinData::instance().aspect_ratio;

    int win_width, win_height;
    glfwGetWindowSize(WinData::instance().window, &win_width, &win_height);

    // scale from 0.0 to 1.0
    float y = WinData::instance().cursor_y / win_height;
    // flip since y is reversed
    y = std::abs(y - 1.0f);

    float x = WinData::instance().cursor_x / win_width;

    x -= 0.5;
    y -= 0.5;
    x *= base_width;
    y *= base_height;

    return glm::vec2(x, y);
}