#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "cgeom.h"

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
    struct vec2d pos;
    struct vec4f color;
    unsigned int dot_type;
    float size;
};

struct g_segment {
    struct vec2d p1;
    struct vec2d p2;
    unsigned int line_type;
    struct vec4f color;
    float width;
};

struct g_arc {
    struct vec2d pos;
    double radius;
    double start_degree;
    double end_degree;
    struct vec4f color;
    unsigned int line_type;
    float width;
};

struct g_grid {
    struct vec2f size;
    float step;
    struct vec4f color;
    unsigned int line_type;
};

struct g_line {
    struct vec2d pos;
    struct vec2d dir;
    unsigned int line_type;
    struct vec4f color;
    float width;
};

struct g_segment_array {
    unsigned int line_type;
    struct vec4f color;
    float width;
    struct mat4f transform;
    cvector(struct vec2d) vertices;
};

struct g_test {
    struct vec2d p1;
    struct vec2d p2;
    unsigned int line_type;
    struct vec4f color;
    float width;
};

void g_init();

struct g_camera* g_camera_create(struct vec2i* viewport_size);
void g_camera_free(struct g_camera* cam);
void g_camera_move(struct g_camera* cam, struct vec2d delta);
void g_camera_zoom(struct g_camera* cam, double delta);
double g_camera_get_zoom(const struct g_camera* cam);

struct g_manager* g_manager_create();
void g_manager_free(struct g_manager* man);
struct g_gpu_object* g_add_segment(struct g_manager* man, struct g_segment* segment);
struct g_gpu_object* g_add_grid(struct g_manager* man, struct g_grid* grid);
struct g_gpu_object* g_add_line(struct g_manager* man, struct g_line* line);
struct g_gpu_object* g_add_arc(struct g_manager* man, struct g_arc* arc);
struct g_gpu_object* g_add_dot(struct g_manager* man, struct g_dot* dot);
struct g_gpu_object* g_add_segment_array_strip(struct g_manager* man, struct g_segment_array* line_array);
struct g_gpu_object* g_add_segment_array(struct g_manager* man, struct g_segment_array* line_array);
struct g_gpu_object* g_add_test(struct g_manager* man, struct g_test* test);
void g_draw(struct g_manager* man, struct g_camera* cam);

#endif