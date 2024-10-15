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
GLuint broken_shader;

void g_init(){
    su_load_shader(&segment_shader, "shaders/segment");
    su_load_shader(&grid_shader, "shaders/grid2d");
    su_load_shader(&line_shader, "shaders/line");
    su_load_shader(&arc_shader, "shaders/arc");
    su_load_shader(&dot_shader, "shaders/dot");
    su_load_shader(&broken_shader, "shaders/broken");
}

// CAMERA

struct g_camera {
    union gm_dvec2 position;
    float rotation;
    double zoom;
    union gm_ivec2 viewport_size;
    union gm_fmat4 projection;
    union gm_fmat4 view;
    union gm_fmat4 unproject;
};

void g_camera_update_matrices(struct g_camera* cam) {
    union gm_dvec2 tmp = GM_DVEC2_FROM_VEC2(cam->viewport_size);
    GM_VEC2_DIV(tmp,2);
    GM_VEC2_MULT(tmp,cam->zoom);
    GM_MAT4_ORTHO(cam->projection, -tmp.x, tmp.x, -tmp.y, tmp.y, 0.0f, 1000.0f);
    union gm_fvec3 position =  GM_FVEC3_FROM_VEC2(cam->position, 1);
    union gm_fvec3 target   =  GM_FVEC3_FROM_VEC2(cam->position, 0);
    union gm_fvec3 up       =  {0,1,0};
    GM_FMAT4_LOOK_AT(cam->view, position, target, up);
    cam->unproject = cam->projection;
    GM_MAT4_MULT(cam->projection, cam->view);
    GM_MAT4_INVERSE(cam->unproject);
}

struct g_camera* g_camera_create(union gm_ivec2* viewport_size) {
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

void g_camera_move(struct g_camera* cam, union gm_dvec2* delta){
    GM_VEC2_ADD(cam->position, *delta);
    g_camera_update_matrices(cam);
}

void g_camera_zoom(struct g_camera* cam, double delta){
    cam->zoom += delta;
    g_camera_update_matrices(cam);
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
    glUniformMatrix4fv(glGetUniformLocation(handler, "VIEW"), 1, GL_FALSE, &cam->view.data[0]);
    glUniformMatrix4fv(glGetUniformLocation(handler, "PROJECTION"), 1, GL_FALSE, &cam->projection.data[0]);
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
    cvector(struct g_gpu_object)     dots;
    cvector(struct g_gpu_object) segments;
    cvector(struct g_gpu_object)    lines;
    cvector(struct g_gpu_object)     arcs;
    cvector(struct g_gpu_object)    grids;
    cvector(struct g_gpu_object)  brokens;
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
    cvector_for_each_in(obj, man->segments) { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->grids)    { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->arcs)     { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->dots)     { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->lines)    { g_gpu_object_free(obj);}
    cvector_for_each_in(obj, man->brokens)  { g_gpu_object_free(obj);}
    free(man);
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

struct g_gpu_object* g_add_grid(struct g_manager* man, struct g_grid* grid){
    float half_width = grid->size.x/2;
    float half_height = grid->size.y/2;

    cvector(union gm_fvec2) vertices = NULL;

    for(float i=-half_width; i<half_width; i+=grid->step){
        cvector_push_back(vertices,((union gm_fvec2){i,-half_height}));
        cvector_push_back(vertices,((union gm_fvec2){i,half_height}));
    }

    for(float i=-half_height; i<half_height; i+=grid->step){
        cvector_push_back(vertices,((union gm_fvec2){-half_width,i}));
        cvector_push_back(vertices,((union gm_fvec2){half_width,i}));
    }

    struct g_gpu_object newobj;
    g_gpu_object_make_VBAO(&newobj);

    struct Data {
        union gm_fvec4 color;
        unsigned int type;
    } data = {
        .color = grid->color,
        .type  = grid->line_type
    };

    glBufferData(GL_ARRAY_BUFFER, (sizeof(union gm_fvec2)* cvector_size(vertices)) + sizeof(data), &data, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(data), (sizeof(union gm_fvec2)*cvector_size(vertices)), vertices);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(union gm_fvec2), (void*)(sizeof(union gm_fvec4) + sizeof(unsigned int)));

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(union gm_fvec4), (void*)0);
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

struct g_gpu_object* g_add_broken(struct g_manager* man, struct g_broken* broken){
    struct g_gpu_object newobj;

    g_gpu_object_make_VBAO(&newobj);

    // Size of struct without vertices pointer
    const unsigned int broken_size = sizeof(struct g_broken) - sizeof(union gm_dvec2*);
    const unsigned int vertices_size = sizeof(union gm_dvec2) * cvector_size(broken->vertices);

    // Load struct
    glBufferData(GL_ARRAY_BUFFER, vertices_size + broken_size, broken, GL_STATIC_DRAW);

    // Load vertices
                    //destination   //offset     //size         //data pointer
    glBufferSubData(GL_ARRAY_BUFFER, broken_size, vertices_size, broken->vertices);

    // Vertices settings
                          //id,count,type,normalized,stride                 //offset
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(union gm_dvec2), (void*)0 + broken_size);

    // Struct settings, these will be same for each vert
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT,   0, (void*)offsetof(struct g_broken,line_type));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_broken,color));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_broken,width));

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_broken,transform));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_broken,transform)+16);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_broken,transform)+32);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(struct g_broken,transform)+48);

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

    newobj.vertices_count = cvector_size(broken->vertices);
    cvector_push_back(man->brokens, newobj);
    return cvector_back(man->brokens);
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

    if ( cvector_size(man->brokens) ) {
        g_use_shader(broken_shader, cam);
        cvector_for_each_in(obj, man->brokens){
            glBindVertexArray(obj->vertex_array_object);
            glDrawArrays(GL_LINE_STRIP, 0, obj->vertices_count);
        }
    }
}
