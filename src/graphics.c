#include "graphics.h"

#include <GL/glew.h>
#include "iso646.h"
#include "cvector.h"
#include "cvector_utils.h"
#include "shaderutils.h"

GLuint segment_shader;

void g_init(){
    su_load_shader(&segment_shader, "shaders/segment");
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

void g_segment_draw(const struct g_gpu_object* obj){

}

struct g_manager {
    cvector(struct g_gpu_object)     dots;
    cvector(struct g_gpu_object) segments;
    cvector(struct g_gpu_object)    lines;
    cvector(struct g_gpu_object)     arcs;
    cvector(struct g_gpu_object)    grids;
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

void g_draw(struct g_manager* man, struct g_camera* cam) {
    if ( cvector_size(man->segments) ) {
        g_use_shader(segment_shader, cam);
        cvector_iterator(struct g_gpu_object) obj;
        cvector_for_each_in(obj, man->segments){
            glBindVertexArray(obj->vertex_array_object);
            glDrawArrays(GL_POINTS, 0, 1);
        }
    }
}