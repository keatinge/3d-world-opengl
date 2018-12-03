#pragma once

#include "numeric.h"
#include "gl_draw.h"
#include "camera.h"


struct game_player;
struct glw_game_object;
struct glw_raycast_result;

struct chunk {
	glw_game_object* cubes;
	glw_game_object** cube_loc_lookup;
	size_t max_cubes;
	size_t num_cubes;
	size_t n_x;
	size_t n_y;
	size_t n_z;
};


int block_position_to_cube_idx(chunk* cube_handler, int x, int y, int z);
glw_raycast_result get_selected_cube(chunk* world_cubes, game_player* player);

glm::vec3 block_coordinates_to_world_coordinates(int block_x, int block_y, int block_z);
glm::vec3 world_coordinates_to_block_coordinates(glm::vec3 world_coords);

void set_block(chunk* chunk, glw_game_object* block);

glm::ivec3 get_dim_amplitude(chunk* the_chunk);
bool coordinate_in_bounds(chunk* the_chunk, glm::ivec3 block_coordinates);
glw_game_object* get_block_at(chunk *the_chunk, glm::ivec3 block_coordinates);
bool there_is_block_at(chunk* the_chunk, glm::ivec3 block_coordinates);


glw_game_object* find_cube_at_xz(chunk* the_chunk, glm::ivec3 start_position);
void build_cube_at_block_coordinates(chunk* the_chunk, glm::ivec3 block_coordinates, glm::vec4 color);


chunk build_chunk();
void chunk_free(chunk* the_chunk);
//glw_game_object* find_cube_at_xz_fast(chunk* the_chunk, glm::ivec3 start_position);