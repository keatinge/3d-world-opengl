#pragma once

void text_init();

void text_draw(float screen_x, float screen_y, char* text, float font_height = 30.0f, glm::vec4 color = glm::vec4(1.0, 1.0, 1.0, 1.0));


void text_free();