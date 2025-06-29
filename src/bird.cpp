#include "bird.hpp"

#include <glm/glm.hpp>
#include <iostream>

#include "lib.hpp"
#include "win_manager.hpp"

void Bird::update_pos() {
    float magnitude =
        GlobalData::instance().delta_time * BirdSimSettings::instance().speed;
    position += direction * magnitude;
};

void update_birds(std::vector<std::vector<Bird>>& flocks) {
    float delta_time = GlobalData::instance().delta_time;
    std::vector<std::vector<Bird>> new_flocks;

    // loop through all flocks
    for (int flock_i = 0; flock_i < flocks.size(); flock_i++) {
        std::vector<Bird> new_flock;
        const std::vector<Bird>& flock = flocks[flock_i];

        glm::vec3 average_mass;
        for (auto const& bird : flock) {
            average_mass += bird.position;
        }
        average_mass /= flock.size();

        // loop through all birds in current flock
        for (int i = 0; i < flock.size(); i++) {
            Bird current_bird = flock[i];

            // Alignment + Seperation, same flock birds
            glm::vec3 alignment_sum = glm::vec3(0.0f);
            glm::vec3 seperation_sum = glm::vec3(0.0f);
            for (int x = 0; x < flock.size(); x++) {
                if (x == i) continue;  // ignore same bird
                Bird other = flock[x];
                glm::vec3 direction_away =
                    current_bird.position - other.position;
                float dist = glm::length(direction_away);
                direction_away = glm::normalize(direction_away);

                alignment_sum += other.direction / (dist + 1.0f);
                seperation_sum += seperation_func(direction_away, dist);
            }

            // Seperation, all other flocks
            for (int other_flock_i = 0; other_flock_i < flocks.size();
                 other_flock_i++) {
                if (other_flock_i == flock_i) {
                    continue;
                }
                for (auto const& other_bird : flocks[other_flock_i]) {
                    glm::vec3 direction_away =
                        current_bird.position - other_bird.position;
                    float dist = direction_away.length();
                    direction_away = glm::normalize(direction_away);

                    seperation_sum += seperation_func(direction_away, dist);
                }
            }

            glm::vec3 alignment =
                alignment_sum * BirdSimSettings::instance().alignment_strength;
            glm::vec3 seperation =
                seperation_sum *
                BirdSimSettings::instance().seperation_strength;

            // Cohesion
            glm::vec3 cohesion = average_mass - current_bird.position;
            cohesion = glm::normalize(cohesion);
            cohesion *= BirdSimSettings::instance().cohesion_strength;

            // Noise
            glm::vec3 noise(0.0f);
            if (BirdSimSettings::instance().noise_strength != 0) {
                // set new target when limit is reached
                if (current_bird.noise_lvl >= 1.0f) {
                    current_bird.noise_tgt1 = current_bird.noise_tgt2;
                    current_bird.noise_tgt2.x = gen_random(-1.0f, 1.0f);
                    current_bird.noise_tgt2.y = gen_random(-1.0f, 1.0f);
                    current_bird.noise_lvl = 0.0f;
                }

                // linearly interpolate between targets
                noise =
                    current_bird.noise_tgt1 * (1.0f - current_bird.noise_lvl);
                noise += current_bird.noise_tgt2 * current_bird.noise_lvl;
                noise *= BirdSimSettings::instance().noise_strength;
                current_bird.noise_lvl +=
                    delta_time /
                    BirdSimSettings::instance().noise_tgt_switching;
            }

            // Target
            glm::vec3 target(0.0f);
            if (BirdSimSettings::instance().is_follow_cursor) {
                glm::vec2 cursor_pos = get_cursor_world_pos();
                target = glm::vec3(cursor_pos.x, cursor_pos.y, 0.0f) -
                         current_bird.position;
                target = glm::normalize(target);
                target *= BirdSimSettings::instance().follow_cursor_strength;
            }

            // Obstacle Avoidance
            glm::vec3 obstacles(0.0f);
            for (const glm::vec3& point :
                 BirdSimSettings::instance().obstacles) {
                glm::vec3 dir = current_bird.position - point;
                float dist = glm::length(dir);
                dir = glm::normalize(dir);

                obstacles += dir / (dist * dist + 0.01f);
            }
            obstacles *= BirdSimSettings::instance().obstacle_repulsion;

            current_bird.direction += (cohesion + alignment + seperation +
                                       noise + target + obstacles) *
                                      delta_time;
            current_bird.direction = glm::normalize(current_bird.direction);

            new_flock.push_back(current_bird);
        }
        new_flocks.push_back(new_flock);
    }

    flocks = new_flocks;
}

glm::vec3 seperation_func(glm::vec3 dir, float dist) {
    if (dist > 30.0f) return glm::vec3(0);

    return dir / (4 * dist * dist * dist + 0.05f);
}

std::vector<std::vector<Bird>> spawn_birds(unsigned int n_birds,
                                           unsigned int n_flocks) {
    std::vector<std::vector<Bird>> flocks;

    // generate flocks
    for (int f = 0; f < n_flocks; f++) {
        flocks.push_back(std::vector<Bird>());
    }

    // randomly assign flocks to flocks
    for (int x = 0; x < n_birds; x++) {
        int flock = floor(gen_random(0.0f, n_flocks - 0.01f));
        flocks[flock].push_back(Bird());
    }

    return flocks;
}