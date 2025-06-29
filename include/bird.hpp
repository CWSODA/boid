#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "lib.hpp"

constexpr float max_height = 15.0f;
constexpr float min_height = -max_height;
constexpr float max_width = 10.0f;
constexpr float min_width = -max_width;

class Bird {
   public:
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 noise_tgt1 = glm::vec3(0.0f);
    glm::vec3 noise_tgt2 = glm::vec3(0.0f);
    float noise_lvl = 1.0f;

    // calcs direction given an angle in degrees
    // bird points up by default so trig functions are swapped
    Bird(glm::vec3 pos, float angle) : position(pos) {
        direction.x = sin(angle);
        direction.y = cos(angle);
    }
    Bird(glm::vec3 pos, glm::vec3 dir) : position(pos), direction(dir) {}
    Bird(glm::vec3 pos) : position(pos) { gen_dir(); }
    Bird(float x, float y, float z) {
        position = glm::vec3(x, y, z);
        gen_dir();
    }
    Bird() {
        gen_pos();
        gen_dir();
    }

    void update_pos();

   private:
    void gen_pos() {
        float x = gen_random(min_width, max_width);
        float y = gen_random(min_height, max_height);
        position = glm::vec3(x, y, 0.0f);  // 2D so no z coord
    }
    void gen_dir() {
        direction =
            glm::vec3(gen_random(-1.0, 1.0), gen_random(-1.0, 1.0), 0.0f);
        direction = glm::normalize(direction);
    }
};

struct BirdSimSettings {
   public:
    // bird settings
    float seperation_strength = 40.0f;
    float alignment_strength = 0.6f;
    float cohesion_strength = 2.5f;
    float noise_strength = 0.0f;
    float noise_tgt_switching = 1.0f;
    float obstacle_repulsion = 8.0f;
    float speed = 4.0f;

    // fun
    bool is_follow_cursor = false;
    float follow_cursor_strength = 3.0f;

    std::vector<glm::vec3> obstacles;

    static BirdSimSettings& instance() {
        static BirdSimSettings instance;
        return instance;
    }

   private:
    BirdSimSettings() = default;
};

void update_birds(std::vector<std::vector<Bird>>& flocks);
std::vector<std::vector<Bird>> spawn_birds(unsigned int n_birds,
                                           unsigned int n_flocks);

glm::vec3 seperation_func(glm::vec3 dir, float dist);