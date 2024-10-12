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
    union gm_vct2f position;
    double rotation;
    double zoom;
    union gm_vct2i viewport_size;
    union gm_mat4f projection;
    union gm_mat4f view;
    union gm_mat4f unproject;
};
/*

class Camera {
    private:
        glm::vec2       position        = {0,0};
        float           rotation        = 0;
        float           zoom            = 1;
        glm::ivec2      viewport_size   = {640,480};

        glm::mat4       projection;
        glm::mat4       view;
        glm::mat4       unproject;

        void updateCameraMatrices();

    public:
        Camera(const glm::ivec2& viewport_size);

        void move(const glm::vec2& pos);
        void scale(float zoom);

        [[nodiscard]] float getZoom() const { return zoom; }
        [[nodiscard]] const glm::vec2& getPosition() const { return position; }
        [[nodiscard]] const glm::ivec2& getViewportSize() const { return viewport_size; }
        [[nodiscard]] const glm::mat4& getProjectionMatrix() const { return projection; }
        [[nodiscard]] const glm::mat4& getViewMatrix() const { return view; }
        [[nodiscard]] const glm::mat4& getUnprojectMatrix() const { return unproject; }
};
*/
// END CAMERA

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

    GLFWwindow* window = glfwCreateWindow( 800, 600, argv[0], NULL, NULL );
    if ( not window ) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent( window );
    glfwSwapInterval(0);

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
    // END INIT
    
    while ( not glfwWindowShouldClose( window ) ) {
        glfwWaitEvents();

        glClearColor(0.40,0.40,0.45,1.00);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // RENDER
        // END RENDER

        glfwSwapBuffers( window );    
    }
    return EXIT_SUCCESS;
}
