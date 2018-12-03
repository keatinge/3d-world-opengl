
#include "numeric.h"
#include <stdbool.h>
#include <algorithm>


void clamp_float(float* val, float min, float max) {
	if (*val < min) {
		*val = min;
	}
	else if (*val > max) {
		*val = max;
	}
}

void clamp_int(int* val, int min, int max) {
	if (*val < min) {
		*val = min;
	}
	else if (*val > max) {
		*val = max;
	}
}

int round_to_int(float val) {
	int sign = val > 0 ? 1 : -1;


	float positive_val = fabs(val);
	int abs_rounded = (int)(positive_val + 0.5);

	return sign * abs_rounded;


}





int glw_min_int(int a, int b) {
	if (a <= b) {
		return a;
	}
	return b;

}

int glw_max_int(int a, int b) {
	if (a >= b) {
		return a;
	}
	return b;
}


glm::vec4 glw_rgb_to_color_vec(int r, int g, int b) {
	return glm::vec4((float)r / 255.0, (float)g / 255.0, (float)b / 255.0, 1.0);
}

glm::vec4 glw_hex_to_rgb(int hex) {
	int first = 0xFF0000;
	int second = 0xFF00;
	int third = 0xFF;


	int r = (hex & first) >> 16;
	int g = (hex & second) >> 8;
	int b = (hex & third);

	return glw_rgb_to_color_vec(r, g, b);

}


glm::vec4 glw_hex_to_rgba(int hex, float a) {
	glm::vec4 rgb_only = glw_hex_to_rgb(hex);

	return glm::vec4(rgb_only.r, rgb_only.g, rgb_only.b, a);
}


template <typename T>
T clamp(T val, T min, T max) {


	if (val < min) {
		return min;
	}
	
	if (val > max) {
		return max;

	}

	return val;
}

float lerp(float t0, float t1, float now) {
	return clamp((now - t0) / (t1 - t0), 0.0f, 1.0f);

}




float glw_ray_plane_intersection(glm::vec3 ray_start, glm::vec3 ray_path, glm::vec3 normal, glm::vec3 point) {

	float t = (point.x*normal.x + point.y*normal.y + point.z*normal.z - normal.x*ray_start.x - normal.y*ray_start.y - normal.z*ray_start.z)
		/ (normal.x*ray_path.x + normal.y*ray_path.y + normal.z*ray_path.z);
	return t;

}

bool intersection_inside_rect(glm::vec3 less_pt, glm::vec3 more_pt, glm::vec3 normal, glm::vec3 point_to_test) {
	for (int i = 0; i < 3; i++) {
		if (normal[i] == 1.0 || normal[i] == -1.0) {
			continue;
		}

		assert(normal[i] == 0.0);

		bool inside = point_to_test[i] >= less_pt[i] && point_to_test[i] <= more_pt[i];
		if (!inside) {
			return false;
		}

	}
	return true;
}



glw_box_ray_result get_closest_box_ray_intersection(glm::vec3 ray_start, glm::vec3 ray_path, glm::vec3 center, glm::vec3 widths) {

	bool optimization_on = true;
	if (optimization_on) {
		glm::vec3 ray_start_to_center = glm::normalize(center - ray_start);
		float cos_theta = glm::dot(ray_start_to_center, ray_path);
		if (cos_theta <= 0.97) {


			return { false, glm::vec3(0.0), 0 };
		}
	}

	glm::vec3 half_widths = 0.5f * widths;
	glm::vec3 full_vec(1.0, 1.0, 1.0);

	glm::vec3 normals[] = {
		glm::vec3(0.0, 1.0, 0.0),
		glm::vec3(1.0, 0.0, 0.0),
		glm::vec3(0.0, 0.0, -1.0),
	};

	float t_values[6];
	glm::vec3 t_normals[6];

	unsigned int t_values_size = 0;


	for (int i = 0; i < 3; i++) {
		glm::vec3 this_normal = normals[i];

		float size_in_this_dim = glm::dot(half_widths, this_normal);

		glm::vec3 pt_1 = center + size_in_this_dim;
		glm::vec3 pt_2 = center - size_in_this_dim;

		float t = glw_ray_plane_intersection(ray_start, ray_path, this_normal, pt_1);
		float t2 = glw_ray_plane_intersection(ray_start, ray_path, this_normal, pt_2);


		glm::vec3 other_dimensions = full_vec - this_normal;

		// component wise multiplication
		glm::vec3 vec_to_edge = other_dimensions * half_widths;


		glm::vec3 corner_more = center + vec_to_edge;
		glm::vec3 corner_less = center - vec_to_edge;


		// todo modify here for rotation, i think t and t2 need to be handled differently

		if (t >= 0) {
			glm::vec3 intersection_pt = ray_start + ray_path * t;
			bool is_inside = intersection_inside_rect(corner_less, corner_more, this_normal, intersection_pt);

			if (is_inside) {
				t_values[t_values_size] = t;
				t_normals[t_values_size] = this_normal;
				t_values_size++;
			}

		}

		if (t2 >= 0) {
			glm::vec3 intersection_pt = ray_start + ray_path * t2;
			bool is_inside = intersection_inside_rect(corner_less, corner_more, this_normal, intersection_pt);

			if (is_inside) {
				t_values[t_values_size] = t2;
				t_normals[t_values_size] = -1.0f * this_normal;
				t_values_size++;
			}
		}
	}



	glw_box_ray_result result{ false, glm::vec3(0), -1.0f };


	for (int i = 0; i < t_values_size; i++) {
		if (result.did_hit == false || t_values[i] < result.t) {

			result.did_hit = true;
			result.normal = t_normals[i];
			result.t = t_values[i];
		}
	}

	return result;
}

