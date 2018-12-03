#pragma once

#include <glm/glm.hpp>

struct chunk;

struct game_player {
	glm::vec3 head_position; // world coordinates
	glm::vec3 velocity; // world coordinates
	float yaw;
	float pitch;
	bool is_on_ground;


};



void update_player_position(game_player* player, chunk* main_chunk, float dt);


