#pragma once
#include "player.h"
#include "globals.h"
#include "chunk.h"
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm\gtc\type_ptr.hpp>


glm::ivec3 get_feet_block_coordinates(game_player* player) {
	glm::vec3 feet_position = player->head_position - glm::vec3(0, PLAYER_HEAD_HEIGHT, 0);

	glm::vec3 bc = world_coordinates_to_block_coordinates(feet_position);

	return glm::ivec3(round_to_int(bc.x), round_to_int(bc.y), round_to_int(bc.z));

}


void update_player_position(game_player* player, chunk* the_chunk, float dt) {



	//printf("%d, %d, %d \n",world_coords.x , world_coords.y, world_coords.z);
	player->head_position += dt * player->velocity;

	glm::ivec3 block_coords = get_feet_block_coordinates(player);
	glm::vec3 feet_position = player->head_position - glm::vec3(0, PLAYER_HEAD_HEIGHT, 0);

	glw_game_object* block = get_block_at(the_chunk, block_coords);





	bool prev = player->is_on_ground;

	if (block != nullptr) {

		assert(block->block_position.x == block_coords.x && block->block_position.y == block_coords.y && block->block_position.z == block_coords.z);
		block->color = glm::ivec4(1.0, 0.0, 0.0, 1.0);


		float EPS = 0.01;
		player->is_on_ground = feet_position.y - EPS <= block->position.y + 0.5;
	}
	else {
		player->is_on_ground = false;
	}


	if (player->is_on_ground != prev) {  
		printf("SWITCH\n");
	}








	if (player->is_on_ground && player->velocity.y <= 0) {
		player->head_position.y = block->position.y + BLOCK_WIDTH/2.0 + PLAYER_HEAD_HEIGHT;
		player->velocity.y = 0;
	}



	if (!player->is_on_ground) {
		player->velocity.y -= 1 * 9.8 * dt;
	}
	if (player->is_on_ground && free_cam_mode) {
		player->velocity.x = 0;
		player->velocity.y = 0;
		player->velocity.z = 0;
	}


	if (player->head_position.y < -100.0) { // TODO HARDCODES

		player->head_position = glm::vec3(0, 50, 0);
	}

	/*if (player->velocity.y <= 0 && player->is_on_ground) {
		player->velocity.y = 0;
		if (free_cam_mode) {
			player->velocity.x = 0;
			player->velocity.z = 0;
		}
	}*/
}