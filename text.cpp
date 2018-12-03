
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

#include "gl_wrap.h"
#include "globals.h"

//const int CELL_HEIGHT = 22;
//const int CELL_WIDTH = 11;
//
//const int BMP_IM_WIDTH = 512;
//const int BMP_IM_HEIGHT = 128;



const int CELL_HEIGHT = 128;
const int CELL_WIDTH = 64;
const int BMP_IM_WIDTH = 1024;
const int BMP_IM_HEIGHT = 1024;


const int CHARS_PER_ROW = BMP_IM_WIDTH / CELL_WIDTH;

const int START_CHAR = 32;

glw_program the_prog;
unsigned int the_vao;


// X, Y,   U,V
float test_quad[] = {
	-0.5, -0.5,  	0.0, 0.0f,
	0.5, -0.5,		1.0, 0.0,
	0.5, 0.5,		1.0, 1.0,

	-0.5, -0.5,		0.0, 0.0,
	0.5, 0.5,		1.0, 1.0,
	-0.5, 0.5,		0.0, 1.0

};

//top_left = 26, 59.0
//bottom_right = 51, 87


struct letter_coordinate {
	float x;
	float y;
	float u;
	float v;

};


struct letter {
	letter_coordinate bl;
	letter_coordinate br;
	letter_coordinate tr;
	letter_coordinate tl;
};


letter* letter_arr = nullptr;
unsigned int n_letters;
unsigned int color_loc;


void init_letter_arr() {
	float width_f = (float)BMP_IM_WIDTH;
	float height_f = (float)BMP_IM_HEIGHT;


	n_letters = 255 - START_CHAR + 1;

	letter_arr = (letter*)malloc(n_letters * sizeof(letter));




	unsigned int chr_index = 0;
	for (unsigned int chr = START_CHAR; chr < 256; chr++) {

		unsigned int row_index = chr_index / CHARS_PER_ROW;
		unsigned int col_index = chr_index % CHARS_PER_ROW;

		unsigned int left_pixel_x = col_index * CELL_WIDTH;
		unsigned int top_pixel_y = row_index * CELL_HEIGHT;

	/*	unsigned int right_pixel_x = left_pixel_x + CELL_WIDTH;
		unsigned int bottom_pixel_y = top_pixel_y + CELL_HEIGHT;*/


		unsigned int right_pixel_x = left_pixel_x + CELL_WIDTH;
		unsigned int bottom_pixel_y = top_pixel_y + CELL_HEIGHT;


		letter this_letter = {
			{ -0.5f, -0.5f, left_pixel_x / width_f, bottom_pixel_y / height_f }, //bottom left
			{ 0.5f , -0.5f, right_pixel_x / width_f, bottom_pixel_y / height_f }, //bottom right
			{ 0.5f , 0.5f, right_pixel_x / width_f, top_pixel_y / height_f }, //top right
			{ -0.5f , 0.5f, left_pixel_x / width_f, top_pixel_y / height_f }, //top left

		};


		assert(chr_index < n_letters);


		letter_arr[chr_index] = this_letter;

		chr_index++;
	}
}

void text_init() {
	unsigned int texture_unit_to_use = 1; // TODO HACK!!

	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0 + texture_unit_to_use);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // todo these are wrong
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);



	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load("font_huge_aa.bmp", &width, &height, &nrChannels, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);



	stbi_image_free(data);

	init_letter_arr();


	unsigned int vao;
	unsigned int vbo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);


	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, n_letters * sizeof(letter), letter_arr, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);



	glw_program p = glw_build_gl_program("text_vshader.txt", "text_fshader.txt");
	the_prog = p;

	the_vao = vao;


	color_loc = glGetUniformLocation(p.program_id, "color");
}


void text_draw(float screen_x, float screen_y, char* text, float font_height=30.0f, glm::vec4 color=glm::vec4(1.0, 1.0, 1.0, 1.0)) {
	// screen_x: x coordinate of the top left of the first character in screen coordinates
	// screen_y: y coordinate of the top left of the first character in screen coordinates
	// text: null terminated cstring of what to draw on the screen
	// font_height: height in pixels of the character bounding box, the actual displayed text will be smaller 


	// If you call this you should have done the following:
	//     1. Turned on blending
	//     2. Have already called text_init()





	// Coordinate system used
	//      |(0, HEIGHT)
	//      | 
	//      |
	// (0,0)| ________ (WIDTH, 0)


	assert(letter_arr != nullptr); // You must init the letter array before you call this function!!!
	glUseProgram(the_prog.program_id);

	glUniform4fv(color_loc, 1, glm::value_ptr(color));

	unsigned int loc = glGetUniformLocation(the_prog.program_id, "model");
	unsigned int samp_loc = glGetUniformLocation(the_prog.program_id, "samp");


	float ar = (float)WIDTH / (float)HEIGHT;


	float text_height_screen_px = font_height;


	float height_scale_factor = text_height_screen_px * 2.0 / (float)HEIGHT;
	//float width_scale_factor = (float)HEIGHT / (float)WIDTH * height_scale_factor * (float)CELL_WIDTH / (float)CELL_HEIGHT; // Scales a width of 1 in world to appropriate cell width in screen coordinatates

	float width_in_pixels = text_height_screen_px * (float)CELL_WIDTH / (float)CELL_HEIGHT;

	float width_scale_factor = width_in_pixels * 2.0f / (float)WIDTH;





	glUniform1i(samp_loc, 1);
	glBindVertexArray(the_vao);

	glDisable(GL_DEPTH_TEST);


	char* current = text;


	//float screen_x = 0;
	//float screen_y = HEIGHT;

	float model_tx = -1.0f + (screen_x + width_in_pixels/2.0f) * (2.0/ WIDTH); // -1 is so our coordinate system has 0,0 in the bottom_left of the screen
	float model_ty = -1.0f + (screen_y - text_height_screen_px/2.0f) * (2.0/ HEIGHT);


	unsigned int i = 0;
	while (*current != '\0') {


		unsigned int indicies[] = { 0, 1, 2, 0, 2, 3 };

		unsigned char to_draw = *current;
		unsigned int distance = to_draw - START_CHAR;
		unsigned int vertex_offset = 4 * distance; // TODO ADJUST THIS IF YOU ADJUST LETTER STRUCT


		for (int i = 0; i < 6; i++) {
			indicies[i] += vertex_offset;

		}


		float letter_tx = i * width_scale_factor;


		glm::mat4 aspect_correction_model = glm::scale(glm::mat4(1.0f), glm::vec3(width_scale_factor, height_scale_factor, 0.1));
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(model_tx + i * width_scale_factor, model_ty, 0)) * aspect_correction_model;

		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indicies);

		current++;
		i++;
	}
	glEnable(GL_DEPTH_TEST);
}

void text_free() {
	free(letter_arr);
}