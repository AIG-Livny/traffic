#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fontconfig/fontconfig.h>
#include <wchar.h>
#include "shaderutils.h"
#include "iso646.h"
#include "unistd.h"
#include <GLFW/glfw3.h>
#include "geometry.h"

// CAMERA
struct Camera {
    union gm_dvec2 position;
    float rotation;
    double zoom;
    union gm_ivec2 viewport_size;
    union gm_fmat4 projection;
    union gm_fmat4 view;
    //union gm_fmat4 unproject;
};

void camera_update_matrices(struct Camera* cam) {
    union gm_dvec2 tmp = GM_DVEC2_FROM_VEC2(cam->viewport_size);
    GM_VEC2_DIV(tmp,2);
    GM_VEC2_MULT(tmp,cam->zoom);
    GM_MAT4_ORTHO(cam->projection, -tmp.x, tmp.x, -tmp.y, tmp.y, 0.0f, 1000.0f);
    union gm_fvec3 position =  GM_FVEC3_FROM_VEC2(cam->position, 1);
    union gm_fvec3 target   =  GM_FVEC3_FROM_VEC2(cam->position, 0);
    union gm_fvec3 up       =  {0,1,0};
    GM_FMAT4_LOOK_AT(cam->view, position, target, up);

    //cam->unproject = cam->projection;
    //GM_MAT4_MULT(cam->projection, cam->view);
    //GM_MAT4_INVERSE(cam->unproject);
}

struct Camera* camera_create(union gm_ivec2* viewport_size) {
    struct Camera* newcam = malloc(sizeof(struct Camera));
    if ( not newcam ) {
        // ERROR
    }
    memset(newcam,0,sizeof(struct Camera));
    newcam->viewport_size = *viewport_size;
    newcam->zoom = 1.0f;
    camera_update_matrices(newcam);
    return newcam;
}

void camera_free(struct Camera* cam){
    free(cam);
}

// END CAMERA

// GRAPHICS
struct graphics_base {
    GLuint vertex_array_object;
    GLuint vertex_buffer_object;
    unsigned int vertices_count;
};

void use_shader(GLuint handler, const struct Camera* cam){
    glUseProgram(handler);
    glUniformMatrix4fv(glGetUniformLocation(handler, "VIEW"), 1, GL_FALSE, &cam->view.data[0]);
    glUniformMatrix4fv(glGetUniformLocation(handler, "PROJECTION"), 1, GL_FALSE, &cam->projection.data[0]);
    glUniform1f(glGetUniformLocation(handler, "ZOOM"), cam->zoom);
}

void base_free(struct graphics_base* base){
    glDeleteBuffers(1, &base->vertex_buffer_object);
    glDeleteVertexArrays(1, &base->vertex_array_object);
    free(base);
}

void base_genVBAO(struct graphics_base* base){
    glGenVertexArrays(1, &base->vertex_array_object);
    glGenBuffers(1, &base->vertex_buffer_object);
    glBindVertexArray(base->vertex_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, base->vertex_buffer_object);
}

/*
    Not only just enum, also it's mask for line graphics
*/
enum LineType {
    SOLID   = 0xFFFF,
    DOTTED  = 0xAAAA,
    DASHED  = 0b1100001111000011,
    DOTDASH = 0xAFAF,
};

struct GPUSegment {
    union gm_dvec2 p1;
    union gm_dvec2 p2;
    unsigned int line_type;
    union gm_fvec4 color;
    float width;
};

void segment_draw(const struct graphics_base* base){
    glBindVertexArray(base->vertex_array_object);
    glDrawArrays(GL_POINTS, 0, 1);
}

void segment_load_GPU(struct graphics_base* base, struct GPUSegment* segment){
    base_genVBAO(base);

    glBufferData(GL_ARRAY_BUFFER, sizeof(struct GPUSegment), segment, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(struct GPUSegment), (void*)0);
    glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, sizeof(struct GPUSegment), (void*)offsetof(struct GPUSegment,p2));
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT,   sizeof(struct GPUSegment), (void*)offsetof(struct GPUSegment,line_type));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(struct GPUSegment), (void*)offsetof(struct GPUSegment,color));
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(struct GPUSegment), (void*)offsetof(struct GPUSegment,width));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
}
// END GRAPHICS

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods ) {
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ) {
        glfwSetWindowShouldClose( window, GL_TRUE );
    }
}

void error_callback( int error, const char* description ) {
    fputs( description, stderr );
}


int main( int argc, char **argv ) {
    glfwSetErrorCallback( error_callback );

    if ( not glfwInit() ) {
        return EXIT_FAILURE;
    }

    glfwWindowHint( GLFW_VISIBLE, GL_FALSE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

    union gm_ivec2 window_size = {800, 600};

    GLFWwindow* window = glfwCreateWindow( window_size.x, window_size.y, argv[0], NULL, NULL );
    if ( not window ) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent( window );
    glfwSwapInterval(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    //glfwSetFramebufferSizeCallback( window, reshape );
    //glfwSetWindowRefreshCallback( window, display );
    glfwSetKeyCallback( window, keyboard );

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if ( GLEW_OK != err ){
        fprintf( stderr, "Error: %s\n", glewGetErrorString(err) );
        return EXIT_FAILURE;
    }

    glfwShowWindow( window );

    // INIT
    struct Camera* cam = camera_create(&window_size);
    GLuint shader;
    su_load_shader(&shader, "/home/ivan/dev/traffic/shaders/segment");
    struct graphics_base base;
    struct GPUSegment segment = {
        .p1 = {-100,-100},
        .p2 = {100,100},
        .color = {0,0,1,0.5},
        .line_type = SOLID,
        .width = 2.0f
    };

    segment_load_GPU(&base, &segment);
    // END INIT

    while ( not glfwWindowShouldClose( window ) ) {
        glfwWaitEvents();

        glClearColor(0.40,0.40,0.45,1.00);
        glClear( GL_COLOR_BUFFER_BIT );

        // RENDER
        use_shader(shader, cam);
        segment_draw(&base);

        //cam->zoom -= 1;
        //camera_update_matrices(cam);
        // END RENDER

        glfwSwapBuffers( window );
    }
    camera_free(cam);
    return EXIT_SUCCESS;
}
