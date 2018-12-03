#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "anim.h"
#include <cassert>

struct v_shader_vertex {
	glm::vec3 position;
	glm::vec3 normal;

};



Skeleton::Skeleton(glw_program* prog, unsigned int max_bones) {
	this->prog = prog;
	this->main_bone = nullptr;

	this->max_bones = max_bones;
	this->n_bones = 0;

	this->points_mem = nullptr; // The list of verticies all for GPU to draw
	this->index_arr = nullptr; // The element indicies which specify the other to connect the verticies into lines
	this->bone_storage_mem = (Bone*) malloc(max_bones * sizeof(Bone)); // Where we store the actual bone's data


	this->model_loc = glGetUniformLocation(prog->program_id, "model");
	this->color_loc = glGetUniformLocation(prog->program_id, "color");
	this->rn_loc = glGetUniformLocation(prog->program_id, "respect_normals");


	this->points_vao = -1;
	this->points_vbo = -1;
}

void Skeleton::render() {
	assert(this->points_vbo != -1);
	assert(this->points_vao != -1);
	assert(this->main_bone);
	glUseProgram(this->prog->program_id);



	glm::mat4 id(1.0f);
	glUniformMatrix4fv(this->model_loc, 1, GL_FALSE, glm::value_ptr(id));


	glm::vec4 color(0.0, 1.0, 0.5, 1.0);
	glUniform4fv(this->color_loc, 1, glm::value_ptr(color));

	glUniform1i(this->rn_loc, false);

	glBindBuffer(GL_ARRAY_BUFFER, this->points_vao);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->n_bones * sizeof(v_shader_vertex), this->points_mem);






	glBindVertexArray(this->points_vao);

	glLineWidth(20.0f);
	glDrawElements(GL_LINES, 2 * (this->n_bones - 1), GL_UNSIGNED_INT, this->index_arr);


}

Bone* Skeleton::add_bone(Bone* parent_bone, Bone& to_add) {

	to_add.skel = this;
	this->bone_storage_mem[this->n_bones] = Bone(to_add);

	Bone* this_added_bone = &(this->bone_storage_mem[this->n_bones]);
	this->n_bones++;


	assert(this->n_bones < this->max_bones);


	if (parent_bone == nullptr) {
		this->main_bone =this_added_bone;
	}
	else {
		parent_bone->children[parent_bone->n_children] = this_added_bone;
		parent_bone->n_children++;
	}

	return this_added_bone;
}






void Skeleton::finalize() {


	v_shader_vertex* _points_mem = (v_shader_vertex*)calloc(this->n_bones, sizeof(v_shader_vertex));

	unsigned int vao;
	unsigned int vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);


	glBufferData(GL_ARRAY_BUFFER, this->n_bones * sizeof(v_shader_vertex), points_mem, GL_DYNAMIC_DRAW);



	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	this->points_vao = vao;
	this->points_vbo = vbo;
	this->points_mem = _points_mem;


	unsigned int num_points = 2 * (this->n_bones - 1);

	this->index_arr = (unsigned int*) malloc(num_points * sizeof(unsigned int));
	unsigned int current_index = 0;

	this->main_bone->fill_index_arr(current_index, this->index_arr, num_points);
	this->update();



}



void Skeleton::update() {
	glm::mat4 current_tranformation(1.0f);


	this->main_bone->update(current_tranformation);
}





Bone::Bone(glm::vec3& position) {

	this->relative_position = position;
	this->max_children = 10;
	this->n_children = 0;
	this->skel = nullptr;
	this->relative_transform = glm::mat4(1.0f);


}

void Bone::fill_index_arr(unsigned int& i, unsigned int* arr, unsigned int len) {

	unsigned int this_index = this->get_index_in_mem();

	for (int j = 0; j < this->n_children; j++) {
		assert(i < len);
		arr[i] = this_index;
		i++;
		assert(i < len);
		arr[i] = this->children[j]->get_index_in_mem();
		i++;


		this->children[j]->fill_index_arr(i, arr, len);
	}
}



unsigned int Bone::get_index_in_mem() {
	unsigned int offset = this - this->skel->bone_storage_mem;
	return offset;
}


void Bone::update(glm::mat4 previous_transform) {
	glm::mat4 this_bone_transformation = previous_transform * this->relative_transform;
	glm::vec3 this_vertex_pos = this_bone_transformation * glm::vec4(this->relative_position, 1.0f);


	v_shader_vertex this_vert = {
		this_vertex_pos,
		glm::vec3(0.0, 0.0, 0.0),
	};


	unsigned int offset = this->get_index_in_mem();

	this->skel->points_mem[offset] = this_vert;


	glm::mat4 transform_for_children = this_bone_transformation * glm::translate(glm::mat4(1.0f), glm::vec3(this->relative_position));


	for (size_t i = 0; i < this->n_children; i++) {
		this->children[i]->update(transform_for_children);
	}

}




void build_person(Skeleton& s) {

	Bone hip(glm::vec3(0, 0, 0));
	Bone hip_torso(glm::vec3(0, 1, 0));
	Bone hip_torso_rarm(glm::vec3(1, 0, 0));
	Bone hip_torso_larm(glm::vec3(-1, 0, 0));
	Bone hip_torso_head(glm::vec3(0, .5, 0));
	Bone hip_lleg(glm::vec3(-1, -1, 0));
	Bone hip_rleg(glm::vec3(1, -1, 0));


	Bone* hip_ptr = s.add_bone(nullptr, hip);
	Bone* torso_ptr = s.add_bone(hip_ptr, hip_torso);
	Bone* lleg_ptr = s.add_bone(hip_ptr, hip_lleg);
	Bone* rleg_ptr = s.add_bone(hip_ptr, hip_rleg);

	Bone* rarm_ptr = s.add_bone(torso_ptr, hip_torso_rarm);
	Bone* larm_ptr = s.add_bone(torso_ptr, hip_torso_larm);
	Bone* head_ptr = s.add_bone(torso_ptr, hip_torso_head);


	s.finalize();

}


Skeleton::~Skeleton() {
	free(this->bone_storage_mem);
	free(this->points_mem);
	free(this->index_arr);

}