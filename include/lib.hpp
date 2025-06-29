#pragma once

#include <iostream>
#include <random>
#include <string>
#include <glm/glm.hpp>

// generates random float within range
float gen_random(float min, float max);

// default is from 0.0 to 1.0
float gen_random();

glm::mat4 dir2rotation_matrix(glm::vec3 dir);

std::string display_vec3(const glm::vec3 vector);

struct GlobalData {
   public:
    // time
    float delta_time;
    bool is_paused = false;

    static GlobalData& instance() {
        static GlobalData instance;
        return instance;
    }

   private:
    GlobalData() = default;
};