
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <cassert>
#include <stdio.h>
#include <math.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <stb_image.h>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "gl_wrap.h"
#include "globals.h"
#include "text.h"


float quad_verts[] = {
	-0.5f, -0.5f,
	0.5f, -0.5f,
	0.5f, 0.5f,
	-0.5f, 0.5f
};


struct notification {
	char text[50];
	float time;
	float time_delete;
};

struct notifications {

	unsigned int count;
	unsigned int max_count;
	notification* notifs_q;
	unsigned int start_index;


};

unsigned int notif_quad_vao;
unsigned int notif_model_loc;
unsigned int notif_color_loc;
notifications all_notifs;
glw_program flat_prog;
glm::vec4 notif_color;

void notification_init() {

	unsigned int vao;
	unsigned int vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);


	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	notif_quad_vao = vao;


	flat_prog = glw_build_gl_program("flat_vshader.txt", "flat_fshader.txt");
	notif_model_loc = glGetUniformLocation(flat_prog.program_id, "model");
	notif_color_loc = glGetUniformLocation(flat_prog.program_id, "color");

	notif_color = glm::vec4(0.0, 0.0, 0.0, 1.0);

	const int max_notifications = 10;



	all_notifs = {
		0,
		max_notifications,
		(notification*)malloc(10 * sizeof(notification)),
		0
	};

}


void notification_quad_screen_coordinates(glm::vec2 tl, glm::vec2 br, float opacity) {
	// Refer to text.cpp for the coordinate system used here

	glUseProgram(flat_prog.program_id);

	notif_color.a = opacity;
	glUniform4fv(notif_color_loc, 1, glm::value_ptr(notif_color));

	float width_px = br.x - tl.x;
	float height_px = tl.y - br.y;


	glm::vec2 center = 0.5f * (tl + br);

	float ndc_center_x = (center.x / WIDTH) * 2.0f - 1.0f;
	float ndc_center_y = (center.y / HEIGHT) * 2.0f - 1.0f;

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(ndc_center_x, ndc_center_y, 0)) * 
		glm::scale(glm::mat4(1.0f), glm::vec3(width_px * 2.0f / WIDTH, height_px * 2.0f/ HEIGHT, 0.0f));

	//glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(width_px * 2.0f / WIDTH, height_px * 2.0f / HEIGHT, 0.0f));

	glUniformMatrix4fv(notif_model_loc, 1, GL_FALSE, glm::value_ptr(model));


	glBindVertexArray(notif_quad_vao);

	unsigned int verts[] = { 0, 1, 2, 0, 2, 3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)verts);
}

void notification_show() {
	float text_height = 25.0f;

	float width = 400;
	float height = text_height + 10.0f;


	float time_now = glfwGetTime();
	unsigned int to_delete = 0;
	for (int i = 0; i < all_notifs.count; i++) {
		unsigned int actual_index = (all_notifs.start_index + i) % all_notifs.max_count;
		assert(all_notifs.notifs_q[actual_index].time_delete != 0.0);
		if (time_now > all_notifs.notifs_q[actual_index].time_delete) {
			to_delete++;
			//assert(0);
		}
		else {
			break;
		}
	}

	all_notifs.start_index += to_delete;
	all_notifs.count -= to_delete;



	float dy = -(height+10.0f);

	for (int i = 0; i < all_notifs.count; i++) {


		unsigned int last_index = all_notifs.start_index + all_notifs.count - 1;

		unsigned int actual_index = (last_index -  i) % all_notifs.max_count;


		float speed = 5.0f;

		// Linear interpolate the movement up of boxes
		dy += (height + 10.f) * std::min(1.0f, speed * (time_now - all_notifs.notifs_q[actual_index].time));

		glm::vec2 top_left = glm::vec2((float)WIDTH - width, height + dy);
		glm::vec2 bottom_right = glm::vec2(top_left.x + width, top_left.y - height);


	


		float time_delete = all_notifs.notifs_q[actual_index].time_delete;
		float opacity = 1 - lerp(time_delete - 1.0f, time_delete, time_now);


	

		notification_quad_screen_coordinates(top_left, bottom_right, .6 * opacity);
		text_draw(top_left.x + 2.5f, top_left.y - text_height / 4.0f, all_notifs.notifs_q[actual_index].text, text_height, glm::vec4(1.0, 1.0, 1.0, opacity));
		//text_draw(top_left.x + 2.5f, top_left.y - text_height / 4.0f, "B", text_height, glm::vec4(1.0, 1.0, 1.0, opacity));

	}
}

void notification_add(char* text) {



	float duration = 10.0f;
	notification this_notif = {
		{'\0'},
		(float)glfwGetTime(),
		(float)glfwGetTime() + duration,
	};

	//this_notif.time_delete = glfwGetTime() + duration;

	// todo adjust this if you adjust notificaion!!!!


	strcpy_s(this_notif.text, sizeof(notification) - 2 * sizeof(float), text);


	unsigned int next_index = (all_notifs.start_index + all_notifs.count) % all_notifs.max_count;
	all_notifs.notifs_q[next_index] = this_notif;

	if (all_notifs.count < all_notifs.max_count) {

		all_notifs.count++;
	}
	else {
		all_notifs.start_index++;
	}
	//all_notifs.count = glw_min_int(all_notifs.count + 1, all_notifs.max_count);



	assert(all_notifs.count <= all_notifs.max_count);


}