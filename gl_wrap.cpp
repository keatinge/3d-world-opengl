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




#define BUFFER_LENGTH 2048

GLenum gl_check_error(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;

		assert(0);

	}

	return errorCode;

}
#define glCheckError() gl_check_error(__FILE__, __LINE__) 



int read_to_buffer(char* filepath, char* buffer, int buff_size) {
	FILE* f = NULL;
	errno_t err = fopen_s(&f, filepath, "r");

	if (err) {
		printf("Couldn't fopen %s\n", filepath);
		return -1;
	}

	size_t read = fread(buffer, sizeof(char), buff_size, f);

	if (read == buff_size) {
		printf("Shader %s was too large, this code is too simple!\n", filepath);
		fclose(f);
		return -1;
	}

	buffer[read] = '\0';
	fclose(f);
	return 0;
}


void error_check_gl(char* desc, unsigned int id, GLenum e_type) {
	int success;
	char buffer[512];


	glGetShaderiv(id, e_type, &success);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("FUCKING ERROR!!!!!\n, %x", err);
	}

	if (!success) {
		glGetShaderInfoLog(id, 512, NULL, buffer);
		printf("Compilation for %s failed with error %s", desc, buffer);
	}
}


GLFWwindow* glw_make_window() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		printf("Unable to create window... exiting\n");
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, glw_framebuffer_size_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("No idea what the fuck this is\n");
		assert(0);

	}

	return window;

}





glw_program glw_build_gl_program(char* vertex_shader_path, char* fragment_shader_path) {

	//char buffer[BUFFER_LENGTH];

	char* buffer = (char*)malloc(BUFFER_LENGTH * sizeof(char));

	read_to_buffer(vertex_shader_path, buffer, BUFFER_LENGTH);
	int vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader_id, 1, &buffer, NULL);
	glCompileShader(vertex_shader_id);
	error_check_gl(vertex_shader_path, vertex_shader_id, GL_COMPILE_STATUS);


	read_to_buffer(fragment_shader_path, buffer, BUFFER_LENGTH);
	int frag_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader_id, 1, &buffer, NULL);
	glCompileShader(frag_shader_id);
	error_check_gl(fragment_shader_path, frag_shader_id, GL_COMPILE_STATUS);





	int program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, frag_shader_id);
	glLinkProgram(program_id);

	//printf("Error for program:\n");
	//error_check_gl("program", program_id, GL_LINK_STATUS);
	//printf("End error for program:\n");




	glDeleteShader(vertex_shader_id);
	glDeleteShader(frag_shader_id);

	free(buffer);

	return glw_program{
		vertex_shader_id,
		frag_shader_id,
		program_id
	};

}


void glw_framebuffer_size_callback(GLFWwindow* win, int w, int h) {
	WIDTH = w;
	HEIGHT = h;

	printf("AR is %f\n", float(w) / float(h));
	glViewport(0, 0, WIDTH, HEIGHT);
}




int glw_get_uniform_location(glw_program* prog, char* uniform_name) {
	return glGetUniformLocation(prog->program_id, uniform_name);
}

void glw_set_matrix_4f(glw_program* prog, char* uniform_name, glm::mat4 mat) {
	int loc = glw_get_uniform_location(prog, uniform_name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void glw_set_vec3f(glw_program* prog, char* uniform_name, glm::vec3 v) {
	glUniform3f(glw_get_uniform_location(prog, uniform_name), v.x, v.y, v.z);
}

void glw_set_vec4f(glw_program* prog, char* uniform_name, glm::vec4 v) {
	glUniform4fv(glw_get_uniform_location(prog, uniform_name), 1, glm::value_ptr(v));
}

void glw_set_float(glw_program* prog, char* uniform_name, float v) {
	glUniform1f(glw_get_uniform_location(prog, uniform_name), v);
}

void glw_set_bool(glw_program* prog, char* uniform_name, bool b) {
	glUniform1i(glw_get_uniform_location(prog, uniform_name), b);
}


