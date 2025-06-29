#include "lib.hpp"

std::string display_vec3(const glm::vec3 vector) {
    std::string output;
    output = std::to_string(vector.x) + ", " + std::to_string(vector.y) + ", " +
             std::to_string(vector.z);

    return output;
}

glm::mat4 dir2rotation_matrix(glm::vec3 dir) {
    glm::vec3 x_basis =
        glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), dir));

    // top y-axis is the head
    glm::mat4 out = {
        x_basis.x, dir.x, 0, 0,  //
        x_basis.y, dir.y, 0, 0,  //
        x_basis.z, dir.z, 1, 0,  //
        0,         0,     0, 1,  //
    };

    return out;
}

float gen_random(float min, float max) {
    static std::default_random_engine generator;

    if (max < min) {
        std::cout << "Random engine invalid range!" << std::endl;
        return 0.0f;
    }

    float output = generator();
    output = (output - generator.min()) / (generator.max() - generator.min());
    output = output * (max - min) + min;
    return output;
}

// default is from 0.0 to 1.0
float gen_random() {
    static std::default_random_engine generator;

    float output = generator();
    output = (output - generator.min()) / (generator.max() - generator.min());
    return output;
}