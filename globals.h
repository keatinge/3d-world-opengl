#pragma once
#include "gl_draw.h"

#define FPS_BUFFER_SIZE 300



const float BLOCK_WIDTH = 1.0f;
const float BLOCK_SPACING = 0.1f;


const float HALF_BLOCK = BLOCK_WIDTH / 2.0;
const float PLAYER_HEAD_HEIGHT = 3 * (BLOCK_WIDTH + BLOCK_SPACING) - HALF_BLOCK;





extern glw_mesh line_mesh;
extern glw_mesh cube_mesh;
extern glw_mesh cube_lines_mesh;

extern int WIDTH;
extern int HEIGHT;


extern double cursor_x;
extern double cursor_y;
extern double prev_cursor_x;
extern double prev_cursor_y;



extern bool is_tabbed_out;
extern bool free_cam_mode;
extern bool should_place_block_this_frame;
extern bool should_show_alt_camera;


