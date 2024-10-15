#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "geometry.h"
#include "cvector.h"

enum g_line_type {
    g_ltSOLID   = 0xFFFF,
    g_ltDOTTED  = 0xAAAA,
    g_ltDASHED  = 0b1100001111000011,
    g_ltDOTDASH = 0xAFAF,
};

enum g_dot_type {
    g_dtPLUS    = 0,
    g_dtCIRCLE  = 1,
    g_dtCROSS   = 2,
    g_dtDOT     = 3,
};

struct g_dot {
    union gm_dvec2 pos;
    union gm_fvec4 color;
    unsigned int dot_type;
    float size;
};

struct g_arc {
    union gm_dvec2 pos;
    double radius;
    double start_degree;
    double end_degree;
    union gm_fvec4 color;
    unsigned int line_type;
    float width;
};

struct g_segment {
    union gm_dvec2 p1;
    union gm_dvec2 p2;
    unsigned int line_type;
    union gm_fvec4 color;
    float width;
};

struct g_broken {
    unsigned int line_type;
    union gm_fvec4 color;
    float width;
    union gm_fmat4 transform;
    cvector(union gm_dvec2) vertices;
};

struct g_grid {
    union gm_fvec2 size;
    float step;
    union gm_fvec4 color;
    unsigned int line_type;
};

struct g_line {
    union gm_dvec2 pos;
    union gm_dvec2 dir;
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
struct g_gpu_object* g_add_grid(struct g_manager* man, struct g_grid* grid);
struct g_gpu_object* g_add_line(struct g_manager* man, struct g_line* line);
struct g_gpu_object* g_add_arc(struct g_manager* man, struct g_arc* arc);
struct g_gpu_object* g_add_dot(struct g_manager* man, struct g_dot* dot);
struct g_gpu_object* g_add_broken(struct g_manager* man, struct g_broken* broken);

void g_draw(struct g_manager* man, struct g_camera* cam);

#endif