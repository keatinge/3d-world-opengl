
#include <GLFW\glfw3.h>
#include <cassert>
#include <stdio.h>
#include <math.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <iostream>
#include <sstream>

#include "input.h"
#include "globals.h"
#include "player.h"
#include "numeric.h"

glm::vec2 handle_mouse_input(GLFWwindow* window) {
	// do not call this several times per frame


	double cursor_dx, cursor_dy;
	glfwGetCursorPos(window, &cursor_x, &cursor_y);
	cursor_dx = cursor_x - prev_cursor_x;
	cursor_dy = cursor_y - prev_cursor_y;
	prev_cursor_x = cursor_x;
	prev_cursor_y = cursor_y;
	return glm::vec2(-cursor_dx, -cursor_dy);// These are negative cause screen coordinates are reverse
}


glm::vec2 get_change_in_pitch_yaw(GLFWwindow* window) {
	glm::vec2 d_cursor = handle_mouse_input(window);

	float mouse_sens = 0.000625;
	float d_pitch = d_cursor.y  * mouse_sens;
	float d_yaw = d_cursor.x * mouse_sens;


	//d_yaw = dt;
	//pitch is the angle up from your desk, yaw is the angle relative to the x axis, like on unit circle

	return glm::vec2(d_pitch, d_yaw);

}


void handle_player_pitch_yaw(game_player* player, GLFWwindow* window) {

	glm::vec2 pitch_yaw = get_change_in_pitch_yaw(window);

	player->pitch += pitch_yaw.x;

	clamp_float(&(player->pitch), glm::radians(-89.5f), glm::radians(90.0));

	player->yaw += pitch_yaw.y;
}

void handle_free_cam_fly(glw_camera* cam, GLFWwindow* window, float dt) {
	float speed = 20.0;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		glw_move_camera(cam, speed * dt, 0.0, 0.0);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		glw_move_camera(cam, -speed * dt, 0.0, 0.0);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		glw_move_camera(cam, 0.0, -speed * dt, 0.0);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		glw_move_camera(cam, 0.0, speed * dt, 0.0);
	}

}









void handle_player_move(game_player* player, GLFWwindow* window, float dt) {

	float speed = 6.0;
	glm::vec3 forward = glw_get_forward_vector_for_pitch_yaw(player->pitch, player->yaw);
	glm::vec3 forward_xz_only(forward);
	forward_xz_only.y = 0;
	forward_xz_only = glm::normalize(forward_xz_only);

	glm::vec3 right_vector = glm::normalize(glm::cross(forward_xz_only, glm::vec3(0.0, 1.0, 0.0)));

	glm::vec3 forward_move_vector = speed * forward_xz_only;
	glm::vec3 right_move_vector = speed * right_vector;


	// TODO PRESSING WA at same time
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		player->velocity.x = forward_move_vector.x;
		player->velocity.z = forward_move_vector.z;
	}

	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		player->velocity.x = -forward_move_vector.x;
		player->velocity.z = -forward_move_vector.z;

	}

	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

		player->velocity.x = -right_move_vector.x;
		player->velocity.z = -right_move_vector.z;
	}

	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		player->velocity.x = right_move_vector.x;
		player->velocity.z = right_move_vector.z;
	}
	else if (player->is_on_ground) {
		player->velocity.x = 0.0;
		player->velocity.z = 0.0;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player->is_on_ground) {
		player->velocity.y = 10;
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		player->yaw += 1 * dt;
	}




}


void handle_key_event(GLFWwindow* window, int key, int scan_codew, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		is_tabbed_out = true;
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		should_show_alt_camera = !should_show_alt_camera;
	}

}

void handle_mouse_button_event(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT && is_tabbed_out) {
		is_tabbed_out = false;
		glfwGetCursorPos(window, &prev_cursor_x, &prev_cursor_y);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
		free_cam_mode = !free_cam_mode;

	
	}

	else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT && !free_cam_mode && !is_tabbed_out) {
		should_place_block_this_frame = true;

	}


}
