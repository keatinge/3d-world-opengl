#pragma once
#include <glm/glm.hpp>
#include "gl_wrap.h"
#include <vector>


class Skeleton;
struct v_shader_vertex;


class Bone {

public:

	glm::vec3 relative_position;
	glm::mat4 relative_transform;
	Skeleton* skel;
	Bone* children[10];
	unsigned int n_children;
	unsigned int max_children;

	Bone(glm::vec3& pos);

	void fill_index_arr(unsigned int & i, unsigned int * arr, unsigned int len);

	unsigned int get_index_in_mem();

	void update(glm::mat4 previous_transform);


};



class Skeleton {

public:
	Bone* bone_storage_mem;
	v_shader_vertex* points_mem;
	Bone* main_bone;
	unsigned int* index_arr;
	unsigned int max_bones;
	unsigned int n_bones;

	unsigned int points_vao;
	unsigned int points_vbo;
	glw_program* prog;


	unsigned int model_loc;
	unsigned int color_loc;
	unsigned int rn_loc;


	Skeleton(glw_program* prog, unsigned int max_bones);
	~Skeleton();
	void render();
	Bone* add_bone(Bone * parent_bone, Bone & to_add);
	void finalize();
	void update();
};

void build_person(Skeleton& s);
