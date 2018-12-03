#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <cassert>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <sstream>


#include "numeric.h"
#include "player.h"
#include "globals.h"
#include "gl_wrap.h"
#include "gl_draw.h"
#include "camera.h"
#include "input.h"
#include "chunk.h"
#include "fps.h"
#include "text.h"
#include "notification.h"
#include "anim.h"


int WIDTH = 1440;
int HEIGHT = 900;
glw_mesh line_mesh;
glw_mesh cube_mesh;
glw_mesh cube_lines_mesh;

double cursor_x;
double cursor_y;
double prev_cursor_x;
double prev_cursor_y;




bool is_tabbed_out = false;
bool free_cam_mode = false;
bool should_place_block_this_frame = false;
bool should_show_alt_camera = false;


unsigned int frame_count = 0;









int main() {
	GLFWwindow* window = glw_make_window();
	glw_program shaderProgram = glw_build_gl_program("v_shader.txt", "f_shader.txt");
	glw_program textureProgram = glw_build_gl_program("textured_vshader.txt", "textured_fshader.txt");

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, handle_key_event);
	glfwSetMouseButtonCallback(window, handle_mouse_button_event);

	glUseProgram(shaderProgram.program_id);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	float cube_verticies[] = {
		// back face
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -0.5f, // bottom-left
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -0.5f,  // top-right
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -0.5f,  // bottom-right         
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -0.5f,  // top-right
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -0.5f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -0.5f, // top-left
													// front face
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.5f, // bottom-left
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.5f,  // bottom-right
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.5f,  // top-right
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.5f,  // top-right
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  0.5f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.5f, // bottom-left
		// left face
		-0.5f,  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, // top-right
		-0.5f,  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, // top-left
		-0.5f, -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, // top-right
		// right face
		0.5f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  // top-left
		0.5f, -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  // bottom-right
		0.5f,  0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  // top-right         
		0.5f, -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  // bottom-right
		0.5f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  // top-left
		0.5f, -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  // bottom-left     
		// bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, -0.5f,  0.0f, // top-right
		0.5f, -0.5f, -0.5f,  0.0f, -0.5f,  0.0f,  // top-left
		0.5f, -0.5f,  0.5f,  0.0f, -0.5f,  0.0f,  // bottom-left
		0.5f, -0.5f,  0.5f,  0.0f, -0.5f,  0.0f,  // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, -0.5f,  0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, -0.5f,  0.0f, // top-right
			// top face
		-0.5f,  0.5f, -0.5f,  0.0f,  0.5f,  0.0f, // top-left
		0.5f,  0.5f , 0.5f,  0.0f,  0.5f,  0.0f,  // bottom-right
		0.5f,  0.5f, -0.5f,  0.0f,  0.5f,  0.0f,  // top-right     
		0.5f,  0.5f,  0.5f,  0.0f,  0.5f,  0.0f,  // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f,  0.5f,  0.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f,  0.5f,  0.0f,  // bottom-left      
	};
	float line_verticies[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
	};
	float cube_lines[] = {
		// LINES ALONG THE TOP
		-0.5, 0.5, 0.5,
		0.5, 0.5, 0.5,	 
		-0.5, -0.5, 0.5,	
		0.5, -0.5, 0.5, 
		- 0.5, -0.5, 0.5, 
		-0.5, 0.5, 0.5, 
		0.5, -0.5, 0.5, 
		0.5, 0.5, 0.5,

		- 0.5, 0.5, -0.5, 
		0.5, 0.5,  -0.5,
		-0.5, -0.5, -0.5,
		0.5, -0.5, -0.5,
		- 0.5, -0.5, -0.5,
		-0.5, 0.5, -0.5, 
		0.5, -0.5, -0.5,
		0.5, 0.5, -0.5,


		0.5, 0.5, 0.5, 
		0.5, 0.5, -0.5,
		0.5, -0.5, 0.5, 
		0.5, -0.5, -0.5,


		-0.5, 0.5, 0.5, 
		-0.5, 0.5, -0.5,
		-0.5, -0.5, 0.5,
		-0.5, -0.5, -0.5,
	};
	float screen_fps_quad[] = {
		0, 150, 0,
		500,0, 0,
		0,0,0,

		0, 150, 0,
		500, 150, 0,
		500, 0, 0

	};
	float texture_test[] = {
		-0.5, -0.5, 0,		0.0f, 0.0f,
		0.5, -0.5, 0,		1.0, 0.0,
		0.5, 0.5, 0,		1.0, 1.0,

		-0.5, -0.5, 0,		0.0, 0.0,
		0.5, 0.5, 0,		1.0, 1.0,
		-0.5, 0.5, 0,		0.0, 1.0

	};


	line_mesh = glw_build_mesh_without_normals(line_verticies, sizeof(line_verticies));
	glw_camera cam = { glm::vec3(0.0f, 20.0f, 5.0f), glm::radians(90.0), 0.0 };
	game_player player = {glm::vec3(0.0, HALF_BLOCK + PLAYER_HEAD_HEIGHT, 0), glm::vec3(0.0f), glm::radians(90.0), 0.0, true};
	cube_lines_mesh = glw_build_mesh_without_normals(cube_lines, sizeof(cube_lines));
	glw_screen_space_obj fps_quad = glw_build_screen_space_triangle_obj(screen_fps_quad, sizeof(screen_fps_quad), glw_hex_to_rgba(0x141414, 0.5));
	glw_texture awesome_face = glw_build_texture("face.png");
	glw_textured_mesh text_quad_mesh = glw_build_textured_mesh(texture_test, sizeof(texture_test));


	cube_mesh = glw_build_mesh_with_normals(cube_verticies, sizeof(cube_verticies));


	Skeleton player_skeleton(&shaderProgram, 100);
	build_person(player_skeleton);
	

	assert(glGetError() == GL_NO_ERROR);
	//float sample_frequency = 60;

	float sample_frequency = 1000;


	float next_sample_time = glfwGetTime();

	fps_measurement measurements[FPS_BUFFER_SIZE]; // todo should this malloc instead?/
	glm::vec3 fps_graph_line_data[FPS_BUFFER_SIZE];
	glw_screen_space_obj fps_graph_line = glw_build_screen_space_lines_strip_obj((float*)fps_graph_line_data, sizeof(fps_graph_line_data), glw_hex_to_rgba(0x0fd8b3, 0.8));


	for (int i = 0; i < FPS_BUFFER_SIZE; i++) {
		measurements[i].time_for_frame = 0.00001f;
		measurements[i].time_measured = 0.0;
	}


	fps_buffer main_fps_buffer = { FPS_BUFFER_SIZE,0, measurements , fps_graph_line_data, 1.0f, 0.0f};





	glfwGetCursorPos(window, &prev_cursor_x, &prev_cursor_y);
	

	// build the world
	chunk main_chunk = build_chunk();


	// Create FBO
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// Create texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH/10 , HEIGHT/10, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Attaches the texture to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH/10, HEIGHT/10 );
	glBindRenderbuffer(GL_RENDERBUFFER, 0); //TODO I DONT THINK THIS IS CORRECT
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	text_init();
	notification_init();


	float time_start = glfwGetTime();
	float delta_time = 0.0;

	while (!glfwWindowShouldClose(window)) {
		GLenum  e = glGetError();


		if ( e != GL_NO_ERROR) {
			printf("GL ERROR %x\n", e);
			assert(0);

		}
		//printf("%d\n", is_tabbed_out);

		float now = glfwGetTime();
		delta_time = now - time_start;
		time_start = now;




		//printf("FPS=%f\n", average_fps(&main_fps_buffer));



		// HANDLE INPUT
		if (free_cam_mode) {
			handle_camera_pitch_yaw(&cam, window);
			handle_free_cam_fly(&cam, window, delta_time);
		}
		else {

			handle_player_pitch_yaw(&player, window);
			handle_player_move(&player, window, delta_time);
			
		}		

		// STEP FORWARD IN TIME







		update_player_position(&player, &main_chunk, delta_time);

		if (!free_cam_mode) {
			cam.pos = player.head_position;
			cam.pitch = player.pitch;
			cam.yaw = player.yaw;

		}

		glm::vec3 player_look_direction = glw_get_forward_vector_for_pitch_yaw(player.pitch, player.yaw);
		

		bool collision_detection_on =  true;
		glw_raycast_result raycast_result;

		if (collision_detection_on) {
			raycast_result = get_selected_cube(&main_chunk, &player);
			if (should_place_block_this_frame && raycast_result.did_hit) {

				//glm::vec3 new_world_pos = raycast_result.cube->position + raycast_result.plane_normal * 1.1f;// TODO THIS IS BROKEN MAGIC NUMBER
				glm::ivec3 new_block_pos = raycast_result.cube->block_position + glm::ivec3(raycast_result.plane_normal);

				glm::vec3 new_world_pos = block_coordinates_to_world_coordinates(new_block_pos.x, new_block_pos.y, new_block_pos.z);
				glw_game_object cube = { cube_mesh, glm::vec4(0.0, 1.0, 1.0, 1.0), new_world_pos, new_block_pos, glm::translate(glm::mat4(1.0f), new_world_pos), 1 };
				set_block(&main_chunk, &cube);

				glm::vec3 feet_position = player.head_position - glm::vec3(0, PLAYER_HEAD_HEIGHT, 0);

				glm::vec3 feet_world_block_f = world_coordinates_to_block_coordinates(feet_position);


				//printf("BLOCK PLCED at %f %f %f\n", new_world_pos.x, new_world_pos.y, new_world_pos.z);
				//printf("Player is at %f %f %f\n", player.head_position.x, player.head_position.y, player.head_position.z);
				//printf("Player feet at %f %f %f\n", feet_position.x, feet_position.y, feet_position.z);

				char buffer[100];


				sprintf_s(buffer, sizeof(buffer), "Block placed at (%d, %d, %d)", new_block_pos.x, new_block_pos.y, new_block_pos.z);
				notification_add(buffer);


				printf("BLOCK %d %d %d", new_block_pos.x, new_block_pos.y, new_block_pos.z);
				printf("Player feet bl %f %f %f\n", feet_world_block_f.x, feet_world_block_f.y, feet_world_block_f.z);
			}
		}
	

		glm::mat4 id(1.0f);

		//player_skelinton.main_bone->children[0]->color = glm::vec4(0.0, 1.0, 1.0, 1.0);

		// LEFT AND RIGHT ARM ROTATION
		/*player_skelinton.main_bone->children[0]->current_pos = glm::translate(id, glm::vec3(-1, 0, 0)) * glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0, 1, 0)) * glm::translate(id, glm::vec3(1, 0, 0));
		player_skelinton.main_bone->children[1]->current_pos = glm::translate(id, glm::vec3(1, 0,0)) * glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0, 1, 0)) * glm::translate(id, glm::vec3(-1, 0, 0));*/

		// UP AND DOWN ARM ROTATION
		//player_skelinton.main_bone->children[0]->current_pos = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1, 0, 0));
		//player_skelinton.main_bone->children[1]->current_pos = glm::translate(id, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0, 1, 0)) * glm::translate(id, glm::vec3(-1, 0, 0)); */



		Bone* torso_ptr = &(player_skeleton.bone_storage_mem[1]);
		//torso_ptr->relative_transform = glm::rotate(glm::mat4(1.0f), sin((float)glfwGetTime()), glm::vec3(0, 0, 1));

		Bone* lleg_ptr = &(player_skeleton.bone_storage_mem[2]);
		lleg_ptr->relative_transform = glm::rotate(glm::mat4(1.0f),  (float)sin(glfwGetTime()), glm::vec3(1, 0, 0)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.5, 0, 0));


		player_skeleton.update();


		
		// RENDER
		// RENDER
		// RENDER
		// RENDER
		// RENDER
		glUseProgram(shaderProgram.program_id);


		// this sets uniforms for the view matrix, perspective matrix, and camera position


		if (should_show_alt_camera && frame_count % 1 == 0) {


			glm::vec3 player_look_dir_xz_only = player_look_direction;
			player_look_dir_xz_only.y = 0;
			player_look_dir_xz_only = 10.0f * glm::normalize(player_look_dir_xz_only);


			glm::mat4 view_matrix = glm::lookAt(player.head_position - player_look_dir_xz_only + glm::vec3(0, 20, 0), player.head_position, glm::vec3(0, 1, 0));


			glm::mat4 pers_matrix = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);
			glw_set_matrix_4f(&shaderProgram, "perspective", pers_matrix);
			glw_set_matrix_4f(&shaderProgram, "view", view_matrix);

			glViewport(0, 0, WIDTH / 10, HEIGHT / 10);
		
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			glClearColor(0.7, 0.7, 0.5, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// this binds vertex array to the cube vertex, then set uniforms for model matrix and color then calls glDrawArrays thousands of times
			draw_cubes(&shaderProgram, &main_chunk, &player, &raycast_result);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, WIDTH, HEIGHT);


		}



		glw_update_camera(&shaderProgram, &cam);



		glClearColor(0.7, 0.7, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw_cubes(&shaderProgram, &main_chunk, &player, &raycast_result);

		player_skeleton.render();

		glw_draw_parameterized_line(&shaderProgram, player.head_position, player_look_direction, 2000.0f);
		glw_draw_parameterized_line(&shaderProgram, player.head_position, glm::normalize(glm::vec3(200, 200, 0) - player.head_position), 200);


		float crosshair_size = 0.02;
		glw_draw_line_screen_coords(&shaderProgram, glm::vec3(crosshair_size, 0.0, 0.0), glm::vec3(-0.5, 0.0, 0.0));
		glw_draw_line_screen_coords(&shaderProgram, glm::vec3(0.0, crosshair_size * (float)WIDTH/HEIGHT, 0.0), glm::vec3(0.0, -0.5, 0.0));
		


		draw_fps_graph(&shaderProgram, &main_fps_buffer, &fps_quad, &fps_graph_line);

		if (should_show_alt_camera) {

			draw_textured_quad(&textureProgram, text_quad_mesh, awesome_face, texture);

		}


		notification_show();
	



		should_place_block_this_frame = false;
		frame_count++;

		glfwSwapBuffers(window);

		bool was_free_cam = free_cam_mode;
		glfwPollEvents();

		if (was_free_cam && !free_cam_mode) {
			player.head_position = cam.pos;
		}

		float time_end = glfwGetTime();
		float time_for_frame = time_end - time_start;

		if (glfwGetTime() > next_sample_time) {

			//add_fps_measurement(&main_fps_buffer, main_chunk.num_cubes);


			add_fps_measurement(&main_fps_buffer, time_for_frame);
			next_sample_time = next_sample_time + 1.0 / sample_frequency;
		}


		
	}

}