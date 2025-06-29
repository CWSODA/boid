#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
   public:
    unsigned int get_id() { return _id; }
    Shader(const char* vert_path, const char* frag_path) {
        std::string vert_code_str, frag_code_str;
        std::ifstream vert_file, frag_file;

        try {
            vert_file.open(vert_path);
            frag_file.open(frag_path);
            std::stringstream vss, fss;
            vss << vert_file.rdbuf();
            fss << frag_file.rdbuf();
            vert_file.close();
            frag_file.close();

            vert_code_str = vss.str();
            frag_code_str = fss.str();
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FAILED_TO_READ_FILE " << std::endl;
        }

        const char* vert_code = vert_code_str.c_str();
        const char* frag_code = frag_code_str.c_str();

        unsigned int vert_shader = glCreateShader(GL_VERTEX_SHADER);
        unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
        int param;
        char info_log[512];

        glShaderSource(vert_shader, 1, &vert_code, NULL);
        glCompileShader(vert_shader);
        glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &param);
        if (!param) {
            glGetShaderInfoLog(vert_shader, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::VERTEX::COMPIlATION_FAILED\n"
                      << info_log << std::endl;
        }

        glShaderSource(frag_shader, 1, &frag_code, NULL);
        glCompileShader(frag_shader);
        glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &param);
        if (!param) {
            glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPIlATION_FAILED\n"
                      << info_log << std::endl;
        }

        _id = glCreateProgram();
        glAttachShader(_id, vert_shader);
        glAttachShader(_id, frag_shader);
        glLinkProgram(_id);
        glGetProgramiv(_id, GL_LINK_STATUS, &param);
        if (!param) {
            glGetProgramInfoLog(_id, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << info_log << std::endl;
        }

        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
    }
    void use() const { glUseProgram(_id); }
    unsigned int get_id() const { return _id; }

    void set_float(const char* name, float value) const {
        glUniform1f(glGetUniformLocation(_id, name), value);
    }
    void set_vec3(const char* name, const glm::vec3& vec) const {
        glUniform3fv(glGetUniformLocation(_id, name), 1, &vec[0]);
    }
    void set_mat4(const char* name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(_id, name), 1, GL_FALSE,
                           glm::value_ptr(mat));
    }

    ~Shader() { glDeleteProgram(_id); }

   private:
    unsigned int _id;
};