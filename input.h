#pragma once
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include "camera.h"
#include "player.h"

glm::vec2 handle_mouse_input(GLFWwindow* window);

glm::vec2 get_change_in_pitch_yaw(GLFWwindow* window);
void handle_player_pitch_yaw(game_player* player, GLFWwindow* window);
void handle_free_cam_fly(glw_camera* cam, GLFWwindow* window, float dt);;
void handle_player_move(game_player* player, GLFWwindow* window, float dt);
void handle_key_event(GLFWwindow* window, int key, int scan_codew, int action, int mods);
void handle_mouse_button_event(GLFWwindow* window, int button, int action, int mods);
