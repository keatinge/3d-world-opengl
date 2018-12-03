

#include <GLFW\glfw3.h>
#include <cassert>
#include <stdio.h>
#include <math.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <iostream>
#include <sstream>

#include "gl_wrap.h"
#include "globals.h"
#include "input.h"
#include "numeric.h"
#include "camera.h"


glm::vec3 glw_get_forward_vector_for_pitch_yaw(float pitch, float yaw) {
	glm::vec3 for_vec(cos(yaw) * cos(pitch), sin(pitch), -1.0 * sin(yaw) * cos(pitch));
	return glm::normalize(for_vec); //todo don't think this is needed
}

void glw_update_camera(glw_program* prog, glw_camera* cam) {


	glm::vec3 forward = glw_get_forward_vector_for_pitch_yaw(cam->pitch, cam->yaw);

	glm::mat4 view_matrix = glm::lookAt(cam->pos, cam->pos + forward, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 pers_matrix = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);


	//pers_matrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);


	glw_set_matrix_4f(prog, "view", view_matrix);
	glw_set_matrix_4f(prog, "perspective", pers_matrix);
	glw_set_vec3f(prog, "camera_pos", cam->pos);


}

void glw_move_camera(glw_camera* cam, float dforward, float dright, float dup) {


	glm::vec3 forward = glw_get_forward_vector_for_pitch_yaw(cam->pitch, cam->yaw);

	glm::vec3 right = glm::cross(forward, glm::vec3(0.0, 1.0, 0.0));
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);

	glm::vec3 d_camera = forward * dforward + right * dright + up * dup;
	cam->pos += d_camera;

}


void handle_camera_pitch_yaw(glw_camera* cam, GLFWwindow* window) {
	glm::vec2 pitch_yaw = get_change_in_pitch_yaw(window);

	cam->pitch += pitch_yaw.x;
	cam->yaw += pitch_yaw.y;

	clamp_float(&(cam->pitch), glm::radians(-89.5f), glm::radians(90.0));
}
