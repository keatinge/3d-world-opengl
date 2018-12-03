#pragma once

#include <glm/glm.hpp>
#include "gl_draw.h"


struct glw_game_object;

struct glw_box_ray_result {
	bool did_hit;
	glm::vec3 normal;
	float t;
};


struct glw_raycast_result {
	bool did_hit;
	float t;
	glw_game_object* cube;
	glm::vec3 plane_normal;

};


void clamp_float(float* val, float min, float max);
void clamp_int(int* val, int min, int max);


float lerp(float t0, float t1, float now);


int glw_min_int(int a, int b);
int glw_max_int(int a, int b);
int round_to_int(float val);

glm::vec4 glw_rgb_to_color_vec(int r, int g, int b);
glm::vec4 glw_hex_to_rgb(int hex);
glm::vec4 glw_hex_to_rgba(int hex, float a);


float glw_ray_plane_intersection(glm::vec3 ray_start, glm::vec3 ray_path, glm::vec3 normal, glm::vec3 point);
bool intersection_inside_rect(glm::vec3 less_pt, glm::vec3 more_pt, glm::vec3 normal, glm::vec3 point_to_test);

glw_box_ray_result get_closest_box_ray_intersection(glm::vec3 ray_start, glm::vec3 ray_path, glm::vec3 center, glm::vec3 widths);