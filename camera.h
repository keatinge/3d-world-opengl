#pragma once

#include <glm\glm.hpp>

#include "gl_wrap.h"


struct glw_camera {
	glm::vec3 pos;

	float yaw;
	float pitch;
};


glm::vec3 glw_get_forward_vector_for_pitch_yaw(float pitch, float yaw);
void glw_update_camera(glw_program* prog, glw_camera* cam);
void glw_move_camera(glw_camera* cam, float dforward, float dright, float dup);
void handle_camera_pitch_yaw(glw_camera* cam, GLFWwindow* window);