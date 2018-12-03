#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <cstdio>


#include "gl_wrap.h"
#include "gl_draw.h"
#include "fps.h"
#include "text.h"
#include "globals.h"


void add_fps_measurement(fps_buffer* buff, float frame_time) {
	buff->measurements[buff->cur] = { (float)glfwGetTime(), frame_time };
	buff->cur = (buff->cur + 1) % buff->size;
}


float average_fps(fps_buffer* buff) {
	float total_time = 0.0f;
	for (size_t i = 0; i < buff->size; i++) {
		total_time += buff->measurements[i].time_for_frame;
	}

	float fps = (float)buff->size / total_time;
	return fps;
}

//draw_fps_graph(&shaderProgram, &main_fps_buffer, &fps_quad);
void draw_fps_graph(glw_program* prog, fps_buffer* buff, glw_screen_space_obj* fps_quad, glw_screen_space_obj* line) {
	glw_draw_screen_space_obj(prog, fps_quad);

	int stop = buff->cur;
	int start_i = buff->cur - 1;
	if (start_i == -1) { start_i = buff->size - 1; }

	int count = 0;
	const float FPS_QUAD_WIDTH = 500;
	const float FPS_QUAD_HEIGHT = 150;

	float max_seen = buff->measurements[0].time_for_frame;
	float min_seen = buff->measurements[0].time_for_frame;
	for (int i = start_i; count < buff->size; count++) {

		float x = (float)count / (buff->size - 1) * FPS_QUAD_WIDTH;


		// COOL DEMO!!!
		//float y = 60.0f * cos(2 * M_PI * x / 100.0f - 2 * M_PI * glfwGetTime());
		//float screen_y = FPS_QUAD_HEIGHT / 2.0 - y;


		float this_measurement = buff->measurements[i].time_for_frame;

		float TOP_OF_GRAPH_Y_VALUE = buff->recent_max;
		float BOTTOM_OF_GRAPH_Y_VALUE = buff->recent_min;

		float graph_span = TOP_OF_GRAPH_Y_VALUE - BOTTOM_OF_GRAPH_Y_VALUE;




		if (this_measurement > max_seen) {
			max_seen = this_measurement;
		}
		else if (this_measurement < min_seen) {
			min_seen = this_measurement;
		}


		float y = (this_measurement - BOTTOM_OF_GRAPH_Y_VALUE) * FPS_QUAD_HEIGHT / graph_span;

		//y = .2;




		/*assert(y >= 0);*/
		float screen_x = x;
		float screen_y = FPS_QUAD_HEIGHT - y;
		/*assert(screen_x >= 0 && screen_x <= WIDTH);
		assert(screen_y >= 0 && screen_y <= HEIGHT);*/

		buff->graph_line_data[count] = glm::vec3(screen_x, screen_y, 0.0f);

		i = i - 1;
		if (i == -1) {
			i = buff->size - 1;
		}
	}


	char text_buffer[100];


	float font_size = 20.0f;
	sprintf_s(text_buffer, "%.4f ms", (buff->recent_max * 1000.0f));
	text_draw(0, HEIGHT, text_buffer, font_size);
	sprintf_s(text_buffer, "%.4f ms", (buff->recent_min * 1000.0f));
	text_draw(0, HEIGHT - FPS_QUAD_HEIGHT + font_size, text_buffer, font_size);


	buff->recent_max = max_seen;
	buff->recent_min = min_seen;

	glUseProgram(prog->program_id);

	glBindBuffer(GL_ARRAY_BUFFER, line->mesh.vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, buff->size * sizeof(glm::vec3), buff->graph_line_data);
	glw_draw_screen_space_obj(prog, line);
}
