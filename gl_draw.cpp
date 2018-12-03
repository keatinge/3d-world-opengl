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


#include "globals.h"
#include "gl_wrap.h"
#include "gl_draw.h"
#include "camera.h"
#include "chunk.h"
#include "numeric.h"
#include "player.h"


glw_texture glw_build_texture(char* file_path) {
	//assert(0);
	static unsigned int current_texture = 0;


	printf("Current texture is %d\n", current_texture);

	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0 + current_texture);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(file_path, &width, &height, &nrChannels, 0);
	assert(data);



	GLenum format = strstr(file_path, ".png") ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	current_texture++;


	return glw_texture{
		file_path,
		texture_id,
		GL_TEXTURE0 + current_texture
	};


}

glw_screen_space_mesh glw_build_screen_space_mesh(float* ptr, size_t raw_size, GLenum draw_mode) {
	unsigned int vbo, vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, raw_size, ptr, GL_DYNAMIC_DRAW);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);



	assert(raw_size % sizeof(float) == 0);
	assert(raw_size % (3 * sizeof(float)) == 0);

	return { vao, vbo, raw_size / (3 * sizeof(float)), draw_mode };

}


void glw_draw_screen_space_obj(glw_program* prog, glw_screen_space_obj* obj) {



	float width_f = (float)WIDTH;
	float height_f = (float)HEIGHT;


	glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(-width_f / 2.0, -height_f / 2.0, 0.0));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / width_f, -2.0f / height_f, 0.0));


	glm::mat4 id = glm::mat4(1.0f);

	glm::mat4 model = scale * trans;


	glDisable(GL_DEPTH_TEST);

	glw_set_matrix_4f(prog, "model", model);
	glw_set_matrix_4f(prog, "view", id);
	glw_set_matrix_4f(prog, "perspective", id);
	glw_set_bool(prog, "respect_normals", false);

	glw_set_vec4f(prog, "color", obj->color);

	glBindVertexArray(obj->mesh.vao);
	glDrawArrays(obj->mesh.draw_mode, 0, obj->mesh.count);




	glEnable(GL_DEPTH_TEST);

}


glw_screen_space_obj glw_build_screen_space_triangle_obj(float* ptr, size_t raw_size, glm::vec4 color) {
	glw_screen_space_mesh m = glw_build_screen_space_mesh(ptr, raw_size, GL_TRIANGLES);
	return {
		m,
		color
	};
}
glw_screen_space_obj glw_build_screen_space_lines_obj(float* ptr, size_t raw_size, glm::vec4 color) {
	glw_screen_space_mesh m = glw_build_screen_space_mesh(ptr, raw_size, GL_LINES);
	return {
		m,
		color
	};
}

glw_screen_space_obj glw_build_screen_space_lines_strip_obj(float* ptr, size_t raw_size, glm::vec4 color) {
	glw_screen_space_mesh m = glw_build_screen_space_mesh(ptr, raw_size, GL_LINE_STRIP);
	return {
		m,
		color
	};
}


glw_mesh glw_build_mesh_with_normals(float* ptr, size_t raw_size) {
	unsigned int vbo;
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);


	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, raw_size, ptr, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	return glw_mesh{ vao, raw_size / (6 * sizeof(float)), true };
}


glw_mesh glw_build_mesh_without_normals(float* ptr, size_t raw_size) {
	unsigned int vbo;
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);


	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, raw_size, ptr, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	return glw_mesh{ vao, raw_size / (3 * sizeof(float)), false };
}


void glw_draw_mesh_as_triangles(glw_mesh* mesh) {
	glBindVertexArray(mesh->vao);

	glDrawArrays(GL_TRIANGLES, 0, mesh->num_elements);
}


void glw_draw_mesh_as_lines(glw_mesh* mesh) {
	glBindVertexArray(mesh->vao);
	glDrawArrays(GL_LINES, 0, mesh->num_elements);
}



void glw_draw_game_object(glw_program* prog, glw_game_object* obj) {


	//static int model_loc = glw_get_uniform_location(prog, "model");
	//static int respect_normals_loc = glw_get_uniform_location(prog, "respect_normals");
	//static int color_loc = glw_get_uniform_location(prog, "color");



	//glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(obj->translation_mat));
	//glUniform1i(respect_normals_loc, 1);
	//glUniform3f(color_loc, obj->color.x, obj->color.y, obj->color.z);


	glw_set_matrix_4f(prog, "model", obj->translation_mat);
	glw_set_bool(prog, "respect_normals", true);
	glw_set_vec4f(prog, "color", obj->color);
	glw_draw_mesh_as_triangles(&(obj->mesh));


}






void glw_draw_parameterized_line(glw_program* prog, glm::vec3 start, glm::vec3 direction, float t) {
	//asume direction is normalized

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), direction);
	glm::mat4 scale_2 = glm::scale(glm::mat4(1.0f), glm::vec3(t)); // todo optoimzie
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), start);
	glm::mat4 model = translate * scale_2 * scale;

	glLineWidth(2.0f);
	glw_set_matrix_4f(prog, "model", model);
	glw_set_bool(prog, "respect_normals", false);
	glw_set_vec4f(prog, "color", glm::vec4(1.0, 0.0, 0.0, 0.5));

	glw_draw_mesh_as_lines(&line_mesh);


}

void glw_draw_line_screen_coords(glw_program* prog, glm::vec3 dir_vec, glm::vec3 trans) {
	glw_set_matrix_4f(prog, "perspective", glm::mat4(1.0f));
	glw_set_matrix_4f(prog, "view", glm::mat4(1.0f));
	glw_set_bool(prog, "respect_normals", false);
	glw_set_vec4f(prog, "color", glm::vec4(1.0, 1.0, 1.0, 1.0));


	glm::mat4 model_matrix = glm::scale(glm::mat4(1.0f), dir_vec) * glm::translate(glm::mat4(1.0f), trans);

	glw_set_matrix_4f(prog, "model", model_matrix);

	glLineWidth(1.0f);

	glw_draw_mesh_as_lines(&line_mesh);

}



void draw_cubes(glw_program* prog, chunk* world, game_player* player, glw_raycast_result* raycast_result) {

	if (world->num_cubes == 0) return;




	unsigned int color_location = glGetUniformLocation(prog->program_id, "color");
	unsigned int model_location = glGetUniformLocation(prog->program_id, "model");



	glw_game_object* first_cube = &(world->cubes[0]);
	unsigned int mesh_size = first_cube->mesh.num_elements;


	glBindVertexArray(first_cube->mesh.vao);

	glw_set_bool(prog, "respect_normals", true);


	glm::vec3 player_position = player->head_position;
	glm::vec3 player_view_direction = glw_get_forward_vector_for_pitch_yaw(player->pitch, player->yaw);

	for (size_t i = 0; i < world->num_cubes; i++) {


		glw_game_object this_cube = world->cubes[i];


		glm::vec3 player_to_cube = glm::normalize(this_cube.position - player_position);





		//if (sleep > 0 && this_cube != raycast_result->cube) continue;



		// TODO TURN THIS OPTIMIZATION BACK ON!!!
		bool optimize = (int)glfwGetTime() % 10 == 0;
		if (optimize && glm::dot(player_to_cube, player_view_direction) < 0.7) {
			continue;
			// This cube is outisde of the field of view
		}

		//glw_set_matrix_4f(prog, "model", obj->translation_mat);
		//glw_set_vec4f(prog, "color", obj->color);


		//if (cubes[i].color != current_color) {
		glUniform4fv(color_location, 1, glm::value_ptr(this_cube.color));
		//}
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(this_cube.translation_mat));
		glDrawArrays(GL_TRIANGLES, 0, mesh_size);




	}



	if (true && raycast_result->cube != NULL) {

		glw_set_matrix_4f(prog, "model", raycast_result->cube->translation_mat * glm::scale(glm::mat4(1.0f), glm::vec3(1.01f)));
		glw_set_bool(prog, "respect_normals", false);
		glw_set_vec4f(prog, "color", glw_hex_to_rgb(0xf442bc));
		glLineWidth(1.0f);
		glw_draw_mesh_as_lines(&cube_lines_mesh);

	}




}
glw_textured_mesh glw_build_textured_mesh(float* data, size_t raw_size) {
	unsigned int vao;
	unsigned int vbo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);


	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, raw_size, data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	return {
		vao,
		raw_size / (5 * sizeof(float))
	};




}

void draw_textured_quad(glw_program* p, glw_textured_mesh m, glw_texture tex, unsigned int texture_unit) {

	glUseProgram(p->program_id);
	unsigned int model_loc = glGetUniformLocation(p->program_id, "model");
	unsigned int color_loc = glGetUniformLocation(p->program_id, "color");
	unsigned int sampler_loc = glGetUniformLocation(p->program_id, "samp");

	assert(model_loc != -1);
	//assert(color_loc != -1);



	float ar = (float)WIDTH / (float)HEIGHT;


	float k = 1.0f / 4.0f;
	float initial_dx = 0.5;
	float initial_dy = 0.5;


	float new_dx = initial_dx * k * ar;
	float new_dy = initial_dy * k;


	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(1-new_dx - 0.05, 1-new_dy - 0.05, 0)) * glm::scale(glm::mat4(1.0f), k * glm::vec3(ar, 1, 0));
	//glm::mat4 model;
	//model = glm::mat4(1.0f);
	glm::vec4 color(0.0, 0.5, 0.5, 1.0);


	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform4fv(color_loc, 1, glm::value_ptr(color));


	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture_unit);

	glUniform1i(sampler_loc, 0);


	glBindVertexArray(m.vao);
	

	glDrawArrays(GL_TRIANGLES, 0, m.num_elements);



}


