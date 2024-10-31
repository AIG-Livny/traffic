#include "graphics.h"

#include <GL/glew.h>
#include "iso646.h"
#include "cvector.h"
#include "cvector_utils.h"
#include "shaderutils.h"

GLuint segment_shader;
GLuint grid_shader;
GLuint line_shader;
GLuint arc_shader;
GLuint dot_shader;
GLuint segment_array_shader;
GLuint test_shader;

void g_init(){
    su_load_shader(&segment_shader, "shaders/segment");
    su_load_shader(&grid_shader, "shaders/grid2d");
    su_load_shader(&line_shader, "shaders/line");
    su_load_shader(&arc_shader, "shaders/arc");
    su_load_shader(&dot_shader, "shaders/dot");
    su_load_shader(&segment_array_shader, "shaders/segment_array");
    su_load_shader(&test_shader, "shaders/test");
}

// CAMERA

struct g_camera {
    struct vec2d position;
    float rotation;
    float zoom;
    struct vec2i viewport_size;
    struct mat4f projection;
    struct mat4f view;
    struct mat4f unproject;
};

static inline void g_camera_update_matrices(struct g_camera* cam) {
    struct vec2d tmp = vec2d_multiply_num(vec2d_divide_num(VEC2_CAST(d,cam->viewport_size),2), cam->zoom);
    cam->projection = mat4f_ortho(-tmp.x, tmp.x, -tmp.y, tmp.y, 0.0f, 1000.0f);

    struct vec3f position =  {cam->position.x, cam->position.y, 1};
    struct vec3f target   =  {cam->position.x, cam->position.y, 0};
    struct vec3f up       =  {0,1,0};

    cam->view = mat4f_look_at(position, target, up);
    cam->unproject = mat4f_multiply_mat4f(cam->projection, cam->view);
    cam->unproject = mat4f_inverse(cam->unproject);
}

struct g_camera* g_camera_create(struct vec2i* viewport_size) {
    struct g_camera* newcam = malloc(sizeof(struct g_camera));
    if ( not newcam ) {
        // ERROR
    }
    memset(newcam,0,sizeof(struct g_camera));
    newcam->viewport_size = *viewport_size;
    newcam->zoom = 1.0f;
    g_camera_update_matrices(newcam);
    return newcam;
}

void g_camera_free(struct g_camera* cam){
    free(cam);
}

void g_camera_move(struct g_camera* cam, struct vec2d delta){
    cam->position = vec2d_add(cam->position, delta);
    g_camera_update_matrices(cam);
}

void g_camera_zoom(struct g_camera* cam, double delta){
    cam->zoom += delta;
    g_camera_update_matrices(cam);
}

void g_camera_zoom_screen_position(struct g_camera* cam, double delta, struct vec2i pos){
    struct vec2i half_viewport  = vec2i_divide_num(cam->viewport_size,2);
    struct vec2i window_pos     = vec2i_subtract(pos, half_viewport);
    struct vec4f pos_viewport   = {window_pos.x/(float)half_viewport.x, -window_pos.y/(float)half_viewport.y, 0,1};

    struct vec4f old_world_pos  = mat4f_multiply_vec4f(cam->unproject,pos_viewport);
    g_camera_zoom(cam, delta);
    struct vec4f new_world_pos  = mat4f_multiply_vec4f(cam->unproject,pos_viewport);

    g_camera_move(cam, VEC2_CAST(d,vec4f_subtract(old_world_pos, new_world_pos)));
}


double g_camera_get_zoom(const struct g_camera* cam){
    return cam->zoom;
}

// END CAMERA

// GRAPHIC OBJECT
struct g_gpu_object {
    GLuint vertex_array_object;
    GLuint vertex_buffer_object;
    unsigned int vertices_count;
};

void g_use_shader(GLuint handler, const struct g_camera* cam){
    glUseProgram(handler);
    glUniformMatrix4fv(glGetUniformLocation(handler, "VIEW"), 1, GL_FALSE, &cam->view.v[0]);
    glUniformMatrix4fv(glGetUniformLocation(handler, "PROJECTION"), 1, GL_FALSE, &cam->projection.v[0]);
    glUniform1f(glGetUniformLocation(handler, "ZOOM"), cam->zoom);
}

void g_gpu_object_free(struct g_gpu_object* obj){
    glDeleteBuffers(1, &obj->vertex_buffer_object);
    glDeleteVertexArrays(1, &obj->vertex_array_object);
    free(obj);
}

void g_gpu_object_make_VBAO(struct g_gpu_object* obj){
    glGenVertexArrays(1, &obj->vertex_array_object);
    glGenBuffers(1, &obj->vertex_buffer_object);
    glBindVertexArray(obj->vertex_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vertex_buffer_object);
}

// END GRAPHIC OBJECT

struct g_manager {
    cvector(struct g_gpu_object) dots;
    cvector(struct g_gpu_object) segments;
    cvector(struct g_gpu_object) lines;
    cvector(struct g_gpu_object) arcs;
    cvector(struct g_gpu_object) grids;
    cvector(struct g_gpu_object) segment_arrays_strip;
    cvector(struct g_gpu_object) segment_arrays;
    cvector(struct g_gpu_object) tests;
};

struct g_manager* g_manager_create(){
    struct g_manager* newman = malloc(sizeof(struct g_manager));
    if ( not newman ) {
        // ERROR
    }
    memset(newman,0,sizeof(struct g_manager));
    return newman;
}

void g_manager_free(struct g_manager* man){
    cvector_iterator(struct g_gpu_object) obj;
    cvector_for_each_in(obj, man->segments)             { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->grids)                { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->arcs)                 { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->dots)                 { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->lines)                { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->segment_arrays_strip) { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->segment_arrays)       { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->tests)                { g_gpu_object_free(obj);}
    free(man);
}

struct g_gpu_object* g_add_dot(struct g_manager* man, struct g_dot* dot){
    struct g_gpu_object newobj;

    g_gpu_object_make_VBAO(&newobj);

    glBufferData(GL_ARRAY_BUFFER, sizeof(struct g_dot), dot, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(struct g_dot), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct g_dot), (void*)offsetof(struct g_dot,color));
    glVertexAttribIPointer(2, 1, GL_INT, sizeof(struct g_dot), (void*)(offsetof(struct g_dot,dot_type)));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(struct g_dot), (void*)offsetof(struct g_dot,size));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    cvector_push_back(man->dots, newobj);
    return cvector_back(man->dots);
}

struct g_gpu_object* g_add_segment(struct g_manager* man, struct g_segment* segment){
    struct g_gpu_object newobj;

    g_gpu_object_make_VBAO(&newobj);

    glBufferData(GL_ARRAY_BUFFER, sizeof(struct g_segment), segment, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(struct g_segment), (void*)0);
    glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, sizeof(struct g_segment), (void*)offsetof(struct g_segment,p2));
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT,   sizeof(struct g_segment), (void*)offsetof(struct g_segment,line_type));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(struct g_segment), (void*)offsetof(struct g_segment,color));
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(struct g_segment), (void*)offsetof(struct g_segment,width));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    cvector_push_back(man->segments, newobj);
    return cvector_back(man->segments);
}

struct g_gpu_object* g_add_arc(struct g_manager* man, struct g_arc* arc){
    struct g_gpu_object newobj;

    g_gpu_object_make_VBAO(&newobj);

    glBufferData(GL_ARRAY_BUFFER, sizeof(struct g_arc), arc, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(struct g_arc), (void*)0);
    glVertexAttribPointer(1, 1, GL_DOUBLE, GL_FALSE, sizeof(struct g_arc), (void*)offsetof(struct g_arc,radius));
    glVertexAttribPointer(2, 1, GL_DOUBLE, GL_FALSE, sizeof(struct g_arc), (void*)offsetof(struct g_arc,start_degree));
    glVertexAttribPointer(3, 1, GL_DOUBLE, GL_FALSE, sizeof(struct g_arc), (void*)offsetof(struct g_arc,end_degree));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(struct g_arc), (void*)offsetof(struct g_arc,color));
    glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT,   sizeof(struct g_arc), (void*)offsetof(struct g_arc,line_type));
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(struct g_arc), (void*)offsetof(struct g_arc,width));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);

    cvector_push_back(man->arcs, newobj);
    return cvector_back(man->arcs);
}

struct g_gpu_object* g_add_grid(struct g_manager* man, struct g_grid* grid){
    double half_width = grid->size.x/2;
    double half_height = grid->size.y/2;

    cvector(struct vec2f) vertices = NULL;

    for(double i=-half_width; i<half_width; i+=grid->step){
        cvector_push_back(vertices,((struct vec2f){i,-half_height}));
        cvector_push_back(vertices,((struct vec2f){i,half_height}));
    }

    for(double i=-half_height; i<half_height; i+=grid->step){
        cvector_push_back(vertices,((struct vec2f){-half_width,i}));
        cvector_push_back(vertices,((struct vec2f){half_width,i}));
    }

    struct g_gpu_object newobj;
    g_gpu_object_make_VBAO(&newobj);

    struct Data {
        struct vec4f color;
        unsigned int type;
    } data = {
        .color = grid->color,
        .type  = grid->line_type
    };

    glBufferData(GL_ARRAY_BUFFER, (sizeof(struct vec2f) * cvector_size(vertices)) + sizeof(data), &data, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(data), (sizeof(struct vec2f)*cvector_size(vertices)), vertices);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vec2f), (void*)(sizeof(data) ));

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct vec4f), (void*)0);
    glVertexAttribDivisor(1,1);

    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT,   sizeof(int), (void*)offsetof(struct Data,type));
    glVertexAttribDivisor(2,1);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    newobj.vertices_count = cvector_size(vertices);


    cvector_push_back(man->grids, newobj);
    return cvector_back(man->grids);
}

struct g_gpu_object* g_add_line(struct g_manager* man, struct g_line* line){
    struct g_gpu_object newobj;

    g_gpu_object_make_VBAO(&newobj);

    glBufferData(GL_ARRAY_BUFFER, sizeof(struct g_line), line, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(struct g_line), (void*)0);
    glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, sizeof(struct g_line), (void*)offsetof(struct g_line,dir));
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT,   sizeof(struct g_line), (void*)offsetof(struct g_line,line_type));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(struct g_line), (void*)offsetof(struct g_line,color));
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(struct g_line), (void*)offsetof(struct g_line,width));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    cvector_push_back(man->lines, newobj);
    return cvector_back(man->lines);
}

struct g_gpu_object* g_add_array0(struct g_manager* man, struct g_segment_array* line_array, cvector(struct g_gpu_object)* vct){
    struct g_gpu_object newobj;

    g_gpu_object_make_VBAO(&newobj);

    // Size of struct without vertices pointer
    const unsigned int segment_array_size = sizeof(struct g_segment_array);
    const unsigned int vertices_size = sizeof(struct vec2d) * cvector_size(line_array->vertices);

    // Create buffer
    glBufferData(GL_ARRAY_BUFFER, vertices_size + segment_array_size, NULL, GL_STATIC_DRAW);

    // Load struct
    glBufferSubData(GL_ARRAY_BUFFER, 0, segment_array_size, line_array);

    // Load vertices
                    //destination   //offset            //size         //data pointer
    glBufferSubData(GL_ARRAY_BUFFER, segment_array_size, vertices_size, line_array->vertices);

    // Vertices settings
                          //id,count,type,normalized,stride                 //offset
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(struct vec2d), (void*)0 - sizeof(struct vec2d) + segment_array_size);

    // Struct settings, these will be same for each vert
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT,   0, (void*)offsetof(struct g_segment_array,line_type));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_segment_array,color));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_segment_array,width));

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_segment_array,transform));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_segment_array,transform)+4*sizeof(float));
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_segment_array,transform)+8*sizeof(float));
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_segment_array,transform)+12*sizeof(float));

    // Update attrs only one time, they are same for each vert
    glVertexAttribDivisor(1,1);
    glVertexAttribDivisor(2,1);
    glVertexAttribDivisor(3,1);
    glVertexAttribDivisor(4,1);
    glVertexAttribDivisor(5,1);
    glVertexAttribDivisor(6,1);
    glVertexAttribDivisor(7,1);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glEnableVertexAttribArray(7);

    newobj.vertices_count = cvector_size(line_array->vertices);
    cvector_push_back(*vct, newobj);
    return cvector_back(*vct);
}

struct g_gpu_object* g_add_segment_array_strip(struct g_manager* man, struct g_segment_array* line_array){
    return g_add_array0(man,line_array,&man->segment_arrays_strip);
}

struct g_gpu_object* g_add_segment_array(struct g_manager* man, struct g_segment_array* line_array){
    return g_add_array0(man,line_array,&man->segment_arrays);
}

void g_segment_array_load_matrix(struct g_gpu_object* obj, const struct mat4f* matrix){
    glBindBuffer(GL_ARRAY_BUFFER, obj->vertex_buffer_object);
    glBufferSubData(GL_ARRAY_BUFFER, offsetof(struct g_segment_array,transform), sizeof(struct mat4f), matrix);
}

struct g_gpu_object* g_add_test(struct g_manager* man, struct g_test* test){
    struct g_gpu_object newobj;

    float half_width = (test->width /2.0);
    struct vec2f dir = vec2f_normalize(vec2f_subtract(VEC2_CAST(f,test->p2), VEC2_CAST(f,test->p1)));

    struct vec2f perpendicular = vec2f_tangent(dir);

    struct vec2f vertices[] = {
        vec2f_subtract(VEC2_CAST(f,test->p1),perpendicular),
        vec2f_add(VEC2_CAST(f,test->p1),perpendicular),
        vec2f_add(VEC2_CAST(f,test->p2),perpendicular),
        vec2f_subtract(VEC2_CAST(f,test->p2),perpendicular),
    };

    g_gpu_object_make_VBAO(&newobj);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(struct g_test), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(struct g_test), test);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vec2f), (void*)0);

    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT,   sizeof(struct  g_test), (void*)offsetof(struct g_test,line_type));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(struct  g_test), (void*)offsetof(struct g_test,color));

    glVertexAttribDivisor(1,4);
    glVertexAttribDivisor(2,4);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    cvector_push_back(man->tests, newobj);
    return cvector_back(man->tests);
}

void g_draw(struct g_manager* man, struct g_camera* cam) {
    cvector_iterator(struct g_gpu_object) obj;

    if ( cvector_size(man->segments) ) {
        g_use_shader(segment_shader, cam);
        cvector_for_each_in(obj, man->segments){
            glBindVertexArray(obj->vertex_array_object);
            glDrawArrays(GL_POINTS, 0, 1);
        }
    }

    if ( cvector_size(man->grids) ) {
        g_use_shader(grid_shader, cam);
        cvector_for_each_in(obj, man->grids){
            glBindVertexArray(obj->vertex_array_object);
            glDrawArrays(GL_LINES, 0, obj->vertices_count);
        }
    }

    if ( cvector_size(man->lines) ) {
        g_use_shader(line_shader, cam);
        cvector_for_each_in(obj, man->lines){
            glBindVertexArray(obj->vertex_array_object);
            glDrawArrays(GL_POINTS, 0, 1);
        }
    }

    if ( cvector_size(man->dots) ) {
        g_use_shader(dot_shader, cam);
        cvector_for_each_in(obj, man->dots){
            glBindVertexArray(obj->vertex_array_object);
            glDrawArrays(GL_POINTS, 0, 1);
        }
    }

    if ( cvector_size(man->arcs) ) {
        g_use_shader(arc_shader, cam);
        cvector_for_each_in(obj, man->arcs){
            glBindVertexArray(obj->vertex_array_object);
            glDrawArrays(GL_POINTS, 0, 1);
        }
    }

    if ( cvector_size(man->segment_arrays_strip) ) {
        g_use_shader(segment_array_shader, cam);
        cvector_for_each_in(obj, man->segment_arrays_strip){
            glBindVertexArray(obj->vertex_array_object);
            glDrawArrays(GL_LINE_STRIP, 1, obj->vertices_count);
        }
    }

    if ( cvector_size(man->segment_arrays) ) {
        g_use_shader(segment_array_shader, cam);
        cvector_for_each_in(obj, man->segment_arrays){
            glBindVertexArray(obj->vertex_array_object);
            glDrawArrays(GL_LINES, 1, obj->vertices_count);
        }
    }

    if ( cvector_size(man->tests) ) {
        g_use_shader(test_shader, cam);
        cvector_for_each_in(obj, man->tests){
            glBindVertexArray(obj->vertex_array_object);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
}
