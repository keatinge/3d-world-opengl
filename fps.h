#pragma once
#include <glm/glm.hpp>


struct fps_measurement {
	float time_measured;
	float time_for_frame;
};


struct fps_buffer {
	size_t size;
	size_t cur;
	fps_measurement* measurements;
	glm::vec3* graph_line_data;
	float recent_max;
	float recent_min;
};

void add_fps_measurement(fps_buffer* buff, float frame_time);

float average_fps(fps_buffer* buff);
void draw_fps_graph(glw_program* prog, fps_buffer* buff, glw_screen_space_obj* fps_quad, glw_screen_space_obj* line);
