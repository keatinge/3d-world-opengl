#pragma once

#include <glm\glm.hpp>
#include <GLFW\glfw3.h>

#include "gl_wrap.h"
#include "chunk.h"
#include "player.h"
#include "numeric.h"

struct glw_texture {
	char* file_path; // not very safe
	unsigned int texture_id;
	GLenum active_texture_value;
};

struct glw_mesh {
	unsigned int vao;
	size_t num_elements;
	bool respect_normals;
};

struct glw_textured_mesh {
	unsigned int vao;
	size_t num_elements;
};



struct glw_screen_space_mesh {
	unsigned int vao;
	unsigned int vbo;
	size_t count;
	GLenum draw_mode;

};

struct glw_screen_space_obj {
	glw_screen_space_mesh mesh;
	glm::vec4 color;
};


struct glw_game_object {
	glw_mesh mesh;
	glm::vec4 color;
	glm::vec3 position;
	glm::ivec3 block_position;
	glm::mat4 translation_mat;
	float cos_theta;
};



glw_texture glw_build_texture(char* file_path);
glw_screen_space_mesh glw_build_screen_space_mesh(float* ptr, size_t raw_size, GLenum draw_mode);



glw_screen_space_obj glw_build_screen_space_triangle_obj(float* ptr, size_t raw_size, glm::vec4 color);
glw_screen_space_obj glw_build_screen_space_lines_obj(float* ptr, size_t raw_size, glm::vec4 color);
glw_screen_space_obj glw_build_screen_space_lines_strip_obj(float* ptr, size_t raw_size, glm::vec4 color);

glw_mesh glw_build_mesh_with_normals(float* ptr, size_t raw_size);
glw_mesh glw_build_mesh_without_normals(float* ptr, size_t raw_size);

void glw_draw_mesh_as_triangles(glw_mesh* mesh);
void glw_draw_mesh_as_lines(glw_mesh* mesh);

void glw_draw_game_object(glw_program* prog, glw_game_object* obj);
void glw_draw_parameterized_line(glw_program* prog, glm::vec3 start, glm::vec3 direction, float t);
void glw_draw_line_screen_coords(glw_program* prog, glm::vec3 dir_vec, glm::vec3 trans);
void glw_draw_screen_space_obj(glw_program* prog, glw_screen_space_obj* obj);
void draw_cubes(glw_program* prog, chunk* world, game_player* player, glw_raycast_result* raycast_result);



glw_textured_mesh glw_build_textured_mesh(float* data, size_t raw_size);
void draw_textured_quad(glw_program* p, glw_textured_mesh m, glw_texture tex, unsigned int test);