
#include "gl_draw.h"
#include "numeric.h"
#include "gl_wrap.h"
#include "player.h"
#include "camera.h"
#include "chunk.h"
#include "globals.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include "globals.h"
#include <glm\gtc\type_ptr.hpp>



int block_position_to_cube_idx(chunk* cube_handler, int x, int y, int z) {


	int x_dim = (cube_handler->n_x - 1) / 2;
	int y_dim = (cube_handler->n_y - 1) / 2;
	int z_dim = (cube_handler->n_z - 1) / 2;
	int adjusted_x = x + x_dim;
	int adjusted_y = y + y_dim;
	int adjusted_z = z + z_dim;



	int i = adjusted_z * (cube_handler->n_x * cube_handler->n_y) + adjusted_x * (cube_handler->n_y) + adjusted_y;

	if (i >= cube_handler->max_cubes || i < 0) {
		return -1;
	}

	return i;
}



glw_raycast_result get_selected_cube(chunk* world_cubes, game_player* player) {

	glw_box_ray_result best_result = { false, glm::vec3(0.0), -1.0f };
	glw_game_object* best_result_cube = NULL;
	best_result.did_hit = 0;


	glm::vec3 player_look_direction = glw_get_forward_vector_for_pitch_yaw(player->pitch, player->yaw);


	float distance_cutoff = 50;
	float distance_cutoff_squared = distance_cutoff * distance_cutoff;


	int d_cutoff = 20;



	glm::vec3 player_block_coordinates = world_coordinates_to_block_coordinates(player->head_position);


	const int player_x = round(player_block_coordinates.x);
	const int player_y = round(player_block_coordinates.y);
	const int player_z = round(player_block_coordinates.z);

	for (int dx = -d_cutoff; dx <= d_cutoff; dx++) {
		for (int dy = -d_cutoff; dy <= d_cutoff; dy++) {
			for (int dz = -d_cutoff; dz <= d_cutoff; dz++) {

				int i = block_position_to_cube_idx(world_cubes, player_x + dx, player_y + dy, player_z + dz);



				if (i == -1) continue;
				glw_game_object* this_cube = world_cubes->cube_loc_lookup[i];

				if (this_cube == nullptr) { continue; }



				glm::vec3 cube_pos = this_cube->position;
				glm::vec3 ray_start_to_cube_center = glm::normalize(cube_pos - player->head_position);
				float cos_theta = glm::dot(ray_start_to_cube_center, player_look_direction);
				this_cube->cos_theta = cos_theta;


				if (cos_theta <= 0.97) {
					continue;
				}

				//glm::vec3 p = this_cube->position;
				glm::vec3 cube_to_player = ((this_cube->position) - (player->head_position));
				float distance_cube_to_player_sqd = glm::dot(cube_to_player, cube_to_player);

				if (distance_cube_to_player_sqd > distance_cutoff_squared) {
					continue;
				}



				glw_box_ray_result result = get_closest_box_ray_intersection(player->head_position, player_look_direction, cube_pos, glm::vec3(1.0, 1.0, 1.0));
				if (result.did_hit && (!best_result.did_hit || result.t < best_result.t)) {
					best_result = result;
					best_result_cube = this_cube;

				}

			}

		}
	}

	return glw_raycast_result{ best_result.did_hit, best_result.t, best_result_cube, best_result.normal };
}


glm::vec3 block_coordinates_to_world_coordinates(int block_x, int block_y, int block_z) {
	float w = BLOCK_WIDTH; // width of the block
	float s = BLOCK_SPACING; //spacing between blocks
	float x = 0 + block_x * (w + s);
	float y = 0 + block_y * (w + s);
	float z = 0 + block_z * (w + s);

	return glm::vec3(x, y, z);
}

glm::vec3 world_coordinates_to_block_coordinates(glm::vec3 world_coords) {

	float w = BLOCK_WIDTH;
	float s = BLOCK_SPACING;

	return glm::vec3(world_coords.x / (w + s), world_coords.y / (w + s), world_coords.z / (w + s));
}




void set_block(chunk* chunk, glw_game_object* block) {
	assert(chunk->num_cubes < chunk->max_cubes);


	int i = block_position_to_cube_idx(chunk, block->block_position.x, block->block_position.y, block->block_position.z);
	glw_game_object* ptr_to_existing_block_at_loc = chunk->cube_loc_lookup[i];

	if (ptr_to_existing_block_at_loc != nullptr) {
		*ptr_to_existing_block_at_loc = *block;
	}
	else {

		chunk->cubes[chunk->num_cubes] = *block;
		chunk->cube_loc_lookup[i] = &(chunk->cubes[chunk->num_cubes]);
		chunk->num_cubes++;

	}
	assert(i != -1);

}


float rand_between(float f1, float f2) {
	float r_zero_to_one = (float)rand() / RAND_MAX;

	return r_zero_to_one * (f2 - f1) + f1;
}


glm::ivec3 get_dim_amplitude(chunk* the_chunk) {

	int x_dim = (the_chunk->n_x - 1) / 2;
	int y_dim = (the_chunk->n_y - 1) / 2;
	int z_dim = (the_chunk->n_z - 1) / 2;


	return glm::ivec3(x_dim, y_dim, z_dim);



}

bool coordinate_in_bounds(chunk* the_chunk, glm::ivec3 block_coordinates) {

	int x_dim = (the_chunk->n_x - 1) / 2;
	int y_dim = (the_chunk->n_y - 1) / 2;
	int z_dim = (the_chunk->n_z - 1) / 2;

	bool x_good = block_coordinates.x >= -x_dim && block_coordinates.x <= x_dim;
	bool y_good = block_coordinates.y >= -y_dim && block_coordinates.y <= y_dim;
	bool z_good = block_coordinates.z >= -z_dim && block_coordinates.z <= z_dim;

	return (x_good && y_good && z_good);

}



glw_game_object* get_block_at(chunk *the_chunk, glm::ivec3 block_coordinates) {


	if (!coordinate_in_bounds(the_chunk, block_coordinates)) {
		return nullptr;
	}
	int i = block_position_to_cube_idx(the_chunk, block_coordinates.x, block_coordinates.y, block_coordinates.z);
	glw_game_object* cube_ptr = the_chunk->cube_loc_lookup[i];


	return cube_ptr;

}
bool there_is_block_at(chunk* the_chunk, glm::ivec3 block_coordinates) {
	//assert(coordinate_in_bounds())

	return get_block_at(the_chunk, block_coordinates) != nullptr;


}


struct bad_step_vectors_result {
	char num_bad_step_directions;
	glm::ivec3 bad_step_directions[4];
};

bad_step_vectors_result get_step_vectors_for_discontinuities(glw_game_object* the_cube, chunk* the_chunk) {
	glm::ivec3 step_direction_vectors[] = {
		glm::ivec3(1, 0, 0),
		glm::ivec3(-1, 0, 0),

		glm::ivec3(0, 0, 1),
		glm::ivec3(0, 0, -1),
	};

	glm::ivec3 y_step_vector[] = {
		glm::ivec3(0, 1, 0),
		glm::ivec3(0, 0, 0),
		glm::ivec3(0, -1, 0),

	};


	bad_step_vectors_result this_result;
	this_result.num_bad_step_directions = 0;

	if (there_is_block_at(the_chunk, the_cube->block_position + glm::ivec3(0.0, 1.0, 0.0))) {
		return this_result;
	}



	for (int j = 0; j < 4; j++) {

		bool found_one = false;
		int oob_coordinates = 0;
		for (int k = 0; k < 3; k++) {
			glm::ivec3 this_step = step_direction_vectors[j] + y_step_vector[k];
			glm::ivec3 block_coordinates = the_cube->block_position + this_step;

			if (!coordinate_in_bounds(the_chunk, block_coordinates)) {// todo not sure wtf to do here ????
				oob_coordinates++;
				continue;

				this_result.num_bad_step_directions = 0;
				return this_result;
			}

			if (there_is_block_at(the_chunk, block_coordinates)) {
				found_one = true;
				break;
			}
		}

		if (!found_one && oob_coordinates < 3 ) {
			this_result.bad_step_directions[this_result.num_bad_step_directions] = step_direction_vectors[j];
			this_result.num_bad_step_directions++;

		}
	}

	return this_result;
}

glw_game_object* find_cube_at_xz(chunk* the_chunk, glm::ivec3 start_position) {
	glm::ivec3 dims = get_dim_amplitude(the_chunk);

	for (int depth = 0; depth < dims.y; depth++) {
		for (int sign = -1; sign < 2; sign += 2) {
			glm::ivec3 search_vector = glm::ivec3(0, sign * depth, 0);
			glw_game_object* maybe_block = get_block_at(the_chunk, search_vector + start_position);


			if (maybe_block) {
				return maybe_block;
			}
		}
	}
	return nullptr;
}


//glw_game_object* find_cube_at_xz_fast(chunk* the_chunk, glm::ivec3 start_position) {
//	int i_begin = block_position_to_cube_idx(the_chunk, start_position.x, 0, start_position.z);
//
//	glw_game_object* arr_start = the_chunk->cube_loc_lookup[i_begin];
//
//
//	for (int i = i_begin; i < i_begin + the_chunk->n_z; i++) {
//
//		//glw_game_object* 
//		assert(the_chunk->cubes[i].block_position.x == start_position.x);
//		assert(the_chunk->cubes[i].block_position.z == start_position.z);
//
//	}
//
//	return nullptr;
//}


void build_cube_at_block_coordinates(chunk* the_chunk, glm::ivec3 block_coordinates, glm::vec4 color) {

	glm::vec3 world_pos = block_coordinates_to_world_coordinates(block_coordinates.x, block_coordinates.y, block_coordinates.z);
	glw_game_object cube_to_place = { cube_mesh, color, world_pos,block_coordinates, glm::translate(glm::mat4(1.0f), world_pos) * glm::scale(glm::mat4(1.0f), BLOCK_WIDTH * glm::vec3(1.0f)), 1 };
	set_block(the_chunk, &cube_to_place);
}

chunk build_chunk() {
	int dim_sz = 100;
	int x_dim = dim_sz;
	int y_dim = 50;
	int z_dim = dim_sz;


	size_t n_x = 2 * x_dim + 1;
	size_t n_y = 2 * y_dim + 1;
	size_t n_z = 2 * z_dim + 1;

	size_t total_blocks = n_x * n_y * n_z;

	glw_game_object* cubes = (glw_game_object*)malloc(total_blocks * sizeof(glw_game_object));
	glw_game_object** cube_loc_lookup = (glw_game_object**)calloc(total_blocks, sizeof(glw_game_object*));


	assert(cubes != nullptr);
	assert(cube_loc_lookup != nullptr);




	chunk this_chunk = { cubes, cube_loc_lookup, total_blocks, 0, n_x, n_y, n_z };


	size_t current_index = 0;


	const int n_coeffs = 6;

	//8, 5
	float cos_coeffs[n_coeffs] = { 28, 10, 1.3, 1.0, 0.5, 0.1 };
	float sin_coeffs[n_coeffs] = { 18, 10, 1, 0.5, 0.3, 0.1 };
	float cos_diag_coeffs[n_coeffs] = { 1.0, .8, .3, .4, .2, 0.1 };

	float cos_freq[n_coeffs] = { 1.0f / 10.0f, 1.0f, 7.0f, 11.0f, 17.0f, 19.0f };
	float sin_freq[n_coeffs] = { 1.0f / 5.0f, 1.2f, 13.0f, 23.0f, 29.0f, 31.0f };
	float cos_diag_freq[n_coeffs] = { 1.0f, 2.0f, 7.0f, 13.0f, 19.0f };


	float cos_phase[n_coeffs];
	float sin_phase[n_coeffs];
	float cos_diag_phase[n_coeffs];



	float variation_pct = .2;
	float v_low = 1 - variation_pct;
	float v_high = 1 + variation_pct;
	for (int i = 0; i < n_coeffs; i++) {
		cos_coeffs[i] = rand_between(cos_coeffs[i] *v_low, cos_coeffs[i] * v_high);
		sin_coeffs[i] = rand_between(sin_coeffs[i] * v_low, sin_coeffs[i] * v_high);
		//cos_diag_coeffs[i] = rand_between(cos_diag_coeffs[i] * .6, cos_diag_coeffs[i] * 1.4);


		cos_freq[i] = rand_between(cos_freq[i] * v_low, cos_freq[i] * v_high);
		sin_freq[i] = rand_between(sin_freq[i] * v_low, sin_freq[i] * v_high);
		//cos_diag_freq[i] = rand_between(cos_diag_freq[i] * .6, cos_diag_freq[i] * 1.4);


		cos_phase[i] = rand_between(0, 2 * M_PI);
		sin_phase[i] = rand_between(0, 2 * M_PI);
		//cos_diag_phase[i] = rand_between(0, 2 * M_PI);
	}




	for (int x = -x_dim; x <= x_dim; x++) {
		for (int z = -z_dim; z <= z_dim; z++) {


			float x_normed = 2.0 * M_PI * (float)(x + x_dim) / (200);
			float z_normed = 2.0 * M_PI * (float)(z + z_dim) / (200);
			//float diag_normed = 2.0 * M_PI * (float)(x*x + z*z) / ((2 * x_dim + 1) *(2 * x_dim + 1) + (2 * z_dim + 1) * (2 * z_dim + 1));


			float y_float = 0;


			for (int i = 0; i < n_coeffs; i++) {
				y_float += cos_coeffs[i] * cos(cos_freq[i] * x_normed + cos_phase[i]);
				y_float += sin_coeffs[i] * sin(sin_freq[i] * z_normed + sin_phase[i]);
				//y_float += cos_diag_coeffs[i] * cos(cos_diag_freq[i] * diag_normed + cos_diag_phase[i]);
			}

			//y_float += 0.5 * sin(15 * (x_normed + z_normed));

			int y = (int)y_float;

			clamp_int(&y, -y_dim, y_dim);


			glm::vec4 color =glm::vec4((x + x_dim) / (2.0f * x_dim + 1) , (z + z_dim)/ (2.0f * z_dim + 1), 0.0, 1.0);


			build_cube_at_block_coordinates(&this_chunk, glm::ivec3(x, y, z), color);
			//glm::vec3 pos = block_coordinates_to_world_coordinates(x, y, z);
			//glw_game_object this_cube = { cube_mesh, color, pos, glm::ivec3(x,y,z), glm::translate(glm::mat4(1.0f), pos), 1 };


			//set_block(&this_chunk, &this_cube);
		}
	}


	for (size_t i = 0; i < this_chunk.num_cubes; i++) {
		glw_game_object* this_cube = &(this_chunk.cubes[i]);


		bad_step_vectors_result r = get_step_vectors_for_discontinuities(this_cube, &this_chunk);



		for (char step_i = 0; step_i < r.num_bad_step_directions; step_i++) {


			glm::ivec3 this_xz_step_vector = r.bad_step_directions[step_i];

			glm::ivec3 check_starting_position = this_cube->block_position + this_xz_step_vector;

			glw_game_object* cube_at_this_xz = find_cube_at_xz(&this_chunk, check_starting_position);




			int dy = cube_at_this_xz->block_position.y - this_cube->block_position.y;

			clamp_int(&dy, -1, 1);
			assert(dy == -1 || dy == 0 || dy == 1);

			glm::ivec3 this_cube_to_next_cube_in_step_dir_y_only = glm::ivec3(0.0, dy, 0.0);
			glm::ivec3 cube_position = this_cube->block_position + this_xz_step_vector + this_cube_to_next_cube_in_step_dir_y_only;

			glm::vec4 color = dy > 0 ? glm::vec4(0.0, 0.0, 1.0, 1.0) : glm::vec4(0.0, 1.0, 0.0, 1.0);

			//glm::vec4 color = glm::vec4(0.0, 0.0, 1.0, 1.0);

			if (dy >= 0) {
				build_cube_at_block_coordinates(&this_chunk, cube_position, color);

			}

		
			/*if (cube_at_this_xz->block_position.y > this_cube->block_position.y) {
				this_cube->color = glm::vec4(0.0, 0.0, 1.0, 1.0);
			}
			else if (cube_at_this_xz->block_position.y < this_cube->block_position.y) {
				this_cube->color = glm::vec4(0.0, 1.0, 0.0, 1.0);
			}
			else {
				this_cube->color = glm::vec4(1.0, 0.0, 0.0, 1.0);
			}*/
				
	
		}
		
		



	}


	return this_chunk;
}


void chunk_free(chunk* the_chunk) {


	free(the_chunk->cubes);
	free(the_chunk->cube_loc_lookup);
}