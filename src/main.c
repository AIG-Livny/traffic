#include <stdio.h>
#include <stdlib.h>
#include <iso646.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "geometry.h"
#include "graphics.h"

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
    g_init();
    struct g_manager* man = g_manager_create();
    struct g_camera* cam = g_camera_create(&window_size);

    g_add_segment(man,&(struct g_segment){
        .p1 = {-100,-100},
        .p2 = {100,100},
        .color = {0,0,1,0.5},
        .line_type = g_ltDOTDASH,
        .width = 2.0f
    });

    g_add_grid(man,&(struct g_grid){
        .size   = GM_FVEC2_FROM_VEC2(window_size),
        .step   = 10,
        .color  = {0.1,0.1,0.1,0.05},
        .line_type = g_ltSOLID
    });

    g_add_grid(man,&(struct g_grid){
        .size   = GM_FVEC2_FROM_VEC2(window_size),
        .step   = 100,
        .color  = {0.1,0.1,0.1,0.1},
        .line_type = g_ltSOLID,
    });

    //X
    g_add_line(man,&(struct g_line){
        .pos = {0,0},
        .dir = {-1,0},
        .color = {0.5,0.0,0.0,0.3},
        .width = 2,
    });

    //Y
    g_add_line(man,&(struct g_line){
        .pos = {0,0},
        .dir = {0,1},
        .color = {0.0,0.5,0.0,0.3},
        .width = 2,
    });

    g_add_arc(man,&(struct g_arc){
        .pos    = {-100,-100},
        .radius = 62,
        .start_degree = M_PI_4,
        .end_degree = (2*M_PI) - M_PI_4,
        .color = {0,0,0.5,1},
        .line_type = g_ltSOLID,
        .width = 2,
    });

    g_add_arc(man,&(struct g_arc){
        .pos    = {-100,-100},
        .radius = 70,
        .start_degree = -M_PI_4,
        .end_degree = M_PI + M_PI_4,
        .color = {0,0.3,0.5,1},
        .line_type = g_ltDOTDASH,
        .width = 3,
    });

    g_add_dot(man,&(struct g_dot){
        .pos = {-100,100},
        .color = {1,0,0,1},
        .dot_type = g_dtPLUS,
        .size = 4
    });

    g_add_dot(man,&(struct g_dot){
        .pos = {-80,100},
        .color = {1,0,0,1},
        .dot_type = g_dtCIRCLE,
        .size = 4
    });

    g_add_dot(man,&(struct g_dot){
        .pos = {-100,80},
        .color = {1,0,0,1},
        .dot_type = g_dtCROSS,
        .size = 4
    });

    g_add_dot(man,&(struct g_dot){
        .pos = {-80,80},
        .color = {1,0,0,1},
        .dot_type = g_dtDOT,
        .size = 4
    });

    cvector(union gm_dvec2) vertices = NULL;
    cvector_push_back(vertices, ((union gm_dvec2){.x=0,.y=0}));
    cvector_push_back(vertices, ((union gm_dvec2){.x=-10,.y=10}));
    cvector_push_back(vertices, ((union gm_dvec2){.x=0,.y=15}));
    cvector_push_back(vertices, ((union gm_dvec2){.x=10,.y=0}));

    union gm_fmat4 tr = GM_MAT4_IDENTITY;
    tr.m14 = 10;
    tr.m24 = 10;

    g_add_line_array_segments(man,&(struct g_line_array){
        .transform = tr,
        .line_type = g_ltDOTDASH,
        .color = {1,0,0,1},
        .width = 4,
        .vertices = vertices
    });
    // END INIT

    while ( not glfwWindowShouldClose( window ) ) {
        glfwWaitEvents();

        glClearColor(0.40,0.40,0.45,1.00);
        glClear( GL_COLOR_BUFFER_BIT );

        g_draw(man, cam);

        g_camera_zoom(cam,-0.001f);

        glfwSwapBuffers( window );
    }
    g_camera_free(cam);
    return EXIT_SUCCESS;
}
