#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
   public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float roll = 0.0f;

    Camera(glm::vec3 cam_pos) : position(cam_pos) {}
    Camera(void) {}

    glm::mat4 get_view_mat() {
        return glm::translate(glm::mat4(1.0f),
                              glm::vec3(-position.x, -position.y, -position.z));
    }

   private:
    void update_vectors() {}
};

glm::mat4 get_transform_mat(glm::vec3 pos) {
    return glm::translate(glm::mat4(1.0f), pos);
}
glm::mat4 get_transform_mat(float x, float y, float z) {
    return glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}