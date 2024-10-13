#include <stdio.h>
#include <stdlib.h>
#include "iso646.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

    // END INIT

    while ( not glfwWindowShouldClose( window ) ) {
        glfwWaitEvents();

        glClearColor(0.40,0.40,0.45,1.00);
        glClear( GL_COLOR_BUFFER_BIT );

        g_draw(man, cam);

        //g_camera_zoom(cam,0.1f);

        glfwSwapBuffers( window );
    }
    g_camera_free(cam);
    return EXIT_SUCCESS;
}
