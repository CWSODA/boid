#include <glm/glm.hpp>
#include <vector>
#include <unordered_set>

constexpr float min_spacing = 0.5f;
constexpr float epsilon = 0.01f;

namespace ivec3_hasher {
struct ivec3_hash {
    size_t operator()(const glm::ivec3 &v) const noexcept {
        size_t h1 = std::hash<int>()(v.x);
        size_t h2 = std::hash<int>()(v.x);
        size_t h3 = std::hash<int>()(v.x);

        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};
struct ivec3_equal {
    bool operator()(const glm::ivec3 &a, const glm::ivec3 &b) const noexcept {
        return a == b;
    }
};
}  // namespace ivec3_hasher

using ivec3_hashset = std::unordered_set<glm::ivec3, ivec3_hasher::ivec3_hash,
                                         ivec3_hasher::ivec3_equal>;

glm::ivec3 quantize(const glm::vec3 &input) {
    return glm::floor(input / epsilon);
}

std::vector<glm::vec3> gen_points(glm::vec3 from, glm::vec3 to,
                                  ivec3_hashset &seen) {
    std::vector<glm::vec3> new_points;
    float length = glm::length(to - from);
    float dist = 0.0f;
    glm::vec3 dir = glm::normalize(to - from);
    glm::vec3 pos = from;
    while (dist < length) {
        if (seen.insert(quantize(pos)).second) {
            new_points.push_back(pos);
        }

        pos += dir * min_spacing;
        dist += min_spacing;
    }
    if (seen.insert(quantize(to)).second) {
        new_points.push_back(to);
    }

    return new_points;
}