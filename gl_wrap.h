#pragma once

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

struct glw_program {
	int vertex_shader_id;
	int fragment_shader_id;
	int program_id;
};

GLenum gl_check_error(const char *file, int line);
GLFWwindow* glw_make_window();

int read_to_buffer(char* filepath, char* buffer, int buff_size);
void error_check_gl(char* desc, unsigned int id, GLenum e_type);
glw_program glw_build_gl_program(char* vertex_shader_path, char* fragment_shader_path);

int glw_get_uniform_location(glw_program* prog, char* uniform_name);
void glw_set_matrix_4f(glw_program* prog, char* uniform_name, glm::mat4 mat);
void glw_set_vec3f(glw_program* prog, char* uniform_name, glm::vec3 v);
void glw_set_vec4f(glw_program* prog, char* uniform_name, glm::vec4 v);
void glw_set_float(glw_program* prog, char* uniform_name, float v);
void glw_set_bool(glw_program* prog, char* uniform_name, bool b);

void glw_framebuffer_size_callback(GLFWwindow* win, int w, int h);