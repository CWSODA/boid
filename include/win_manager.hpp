#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_scroll_callback(GLFWwindow* window, double x_scroll,
                           double y_scroll);
void cursor_callback(GLFWwindow* window, double xpos, double ypos);

// Helper functions
glm::vec2 get_cursor_world_pos();

void process_input(GLFWwindow* window);

struct WinData {
   public:
    // window
    GLFWwindow* window;

    // size
    int width, height;
    float base_height = 160.0f;
    float aspect_ratio;

    // cursor
    float cursor_x, cursor_y;

    // singleton
    static WinData& instance() {
        static WinData instance;
        return instance;
    }

   private:
    WinData() = default;
};

inline void update_aspect(void) {
    WinData::instance().aspect_ratio =
        (float)WinData::instance().width / (float)WinData::instance().height;
}