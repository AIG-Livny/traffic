#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "geometry.h"

enum g_line_type {
    SOLID   = 0xFFFF,
    DOTTED  = 0xAAAA,
    DASHED  = 0b1100001111000011,
    DOTDASH = 0xAFAF,
};

struct g_segment {
    union gm_dvec2 p1;
    union gm_dvec2 p2;
    unsigned int line_type;
    union gm_fvec4 color;
    float width;
};

void g_init();

struct g_camera* g_camera_create(union gm_ivec2* viewport_size);
void g_camera_free(struct g_camera* cam);
void g_camera_move(struct g_camera* cam, union gm_dvec2* delta);
void g_camera_zoom(struct g_camera* cam, double delta);

struct g_manager* g_manager_create();
void g_manager_free(struct g_manager* man);
struct g_gpu_object* g_add_segment(struct g_manager* man, struct g_segment* segment);

void g_draw(struct g_manager* man, struct g_camera* cam);

#endif