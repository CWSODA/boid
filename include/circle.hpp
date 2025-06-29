#pragma once

#include <glad/glad.h>
#include <vector>
#include <unordered_map>
#include <iostream>

struct CircleMesh {
    unsigned int VAO, VBO;
    unsigned int n_vertices;

    CircleMesh(unsigned int in_vao, unsigned int in_vbo, unsigned int n_vert)
        : VAO(in_vao), VBO(in_vbo), n_vertices(n_vert) {}

    // Remember to set shader beforehand
    void draw(void) {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, n_vertices);
    }
};

class Circle {
   public:
    static const CircleMesh& get_circle(unsigned int resolution) {
        auto it = _dict.find(resolution);
        if (it != _dict.end()) return it->second;

        _dict.emplace(resolution, gen_circle(resolution));
        return _dict.at(resolution);
    }

   private:
    static std::unordered_map<unsigned int, CircleMesh> _dict;

    static CircleMesh gen_circle(unsigned int resolution) {
        // gen vertices, start with center and right
        // radius of 1
        std::vector<float> circle_verts = {0.0f, 0.0f, 1.0f, 0.0f};
        for (int i = 1; i < resolution + 1; i++) {
            // get angle in radians
            float angle = 2.0 * 3.1415f / resolution * i;
            float x = cos(angle);
            float y = sin(angle);
            circle_verts.push_back(x);
            circle_verts.push_back(y);
        }

        unsigned int circle_VAO, circle_VBO;
        glGenVertexArrays(1, &circle_VAO);
        glBindVertexArray(circle_VAO);

        glGenBuffers(1, &circle_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, circle_VBO);
        glBufferData(GL_ARRAY_BUFFER, circle_verts.size() * sizeof(float),
                     circle_verts.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                              (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        return CircleMesh(circle_VAO, circle_VBO, circle_verts.size() / 2);
    }
};

std::unordered_map<unsigned int, CircleMesh> Circle::_dict;