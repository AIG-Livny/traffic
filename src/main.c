#include <stdio.h>
#include <stdlib.h>
#include <iso646.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include <iso646.h>

#include "graphics.h"
#include "debug.h"
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

struct g_camera* cam;
bool pan = false;
struct nk_context *ctx;

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods ) {
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ) {
        glfwSetWindowShouldClose( window, GL_TRUE );
    }
}

void scroll( GLFWwindow* window, double xoffset, double yoffset ) {
    if ( nk_item_is_any_active(ctx) ) {
        nk_gflw3_scroll_callback(window,xoffset,yoffset);
    } else {
        g_camera_zoom( cam, -yoffset * g_camera_get_zoom(cam) * 0.04);
    }
}

void mouse_button(GLFWwindow* window, int button, int action, int mods) {
    if ( nk_item_is_any_active(ctx) ) {
        nk_glfw3_mouse_button_callback(window,button,action,mods);
    } else {
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS){
            pan = true;
        }

        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE){
            pan = false;
        }
    }
}

double oldx;
double oldy;

void cursor_pos(GLFWwindow* window, double xpos, double ypos){
    double zoom = g_camera_get_zoom(cam);
    if ( pan ) {
        g_camera_move(cam,(struct vec2d){-((xpos - oldx) * zoom), (ypos - oldy) * zoom});
    }
    oldx = xpos;
    oldy = ypos;
}

void error_callback( int error, const char* description ) {
    fputs( description, stderr );
}

struct car {

};

struct road {
    cvector_vec2d points;
};

double sfps = 0;

#define USEC_IN_SECOND 1000000.0
#define TARGET_SFPS 60.0
#define TARGET_USECS (USEC_IN_SECOND/TARGET_SFPS)
#define TARGET_SECS (TARGET_USECS/USEC_IN_SECOND)

void* simulation_thread(void* vargp){
    double previous_time    = glfwGetTime() - TARGET_SECS;
    unsigned int sleep_usec = TARGET_USECS;
    int step = 10;
    while(1) {
        double current_time = glfwGetTime();
        double delta = current_time - previous_time;
        previous_time = current_time;
        sfps = 1.0/delta;

        // PROCESS

        // END PROCESS

        if (sleep_usec > 100){
            usleep(sleep_usec);
        }
        // Load/Sleep balancer
        if((sfps < TARGET_SFPS -2) or (sfps > TARGET_SFPS +2)){
            step = (TARGET_SFPS/sfps)*50;
            if (sfps < TARGET_SFPS) {
                if(sleep_usec > step){sleep_usec -= step;}
            }else{
                if(sleep_usec < INT_MAX - step){sleep_usec += step;}
            }
        }

        // Send signal to redraw
        glfwPostEmptyEvent();
    }
}

int main( int argc, char **argv ) {
    glfwSetErrorCallback( error_callback );

    if ( not glfwInit() ) {
        return EXIT_FAILURE;
    }

    glfwWindowHint( GLFW_VISIBLE, GL_FALSE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    struct vec2i window_size = {800, 600};

    GLFWwindow* window = glfwCreateWindow( window_size.x, window_size.y, argv[0], NULL, NULL );
    if ( not window ) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent( window );
    glfwSwapInterval(1);

    //glfwSetFramebufferSizeCallback( window, reshape );
    //glfwSetWindowRefreshCallback( window, display );
    glfwGetCursorPos(window, &oldx, &oldy);
    glfwSetKeyCallback( window, keyboard );
    glfwSetScrollCallback( window, scroll );
    glfwSetMouseButtonCallback( window, mouse_button );
    glfwSetCursorPosCallback( window, cursor_pos );
    glfwSetCharCallback(window, nk_glfw3_char_callback);

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
    cam = g_camera_create(&window_size);

    g_add_segment(man,&(struct g_segment){
        .p1 = {-100,-100},
        .p2 = {100,100},
        .color = {0,0,1,0.5},
        .line_type = g_ltDOTDASH,
        .width = 2.0f
    });

    g_add_grid(man,&(struct g_grid){
        .size   = {window_size.x, window_size.y},
        .step   = 10,
        .color  = {0.1,0.1,0.1,0.05},
        .line_type = g_ltSOLID
    });

    g_add_grid(man,&(struct g_grid){
        .size   = {window_size.x, window_size.y},
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

    struct road road = {};

    cvector_push_back(road.points, ((struct vec2d){.x=-100, .y=-100}));
    cvector_push_back(road.points, ((struct vec2d){.x=-50,  .y=0}));
    cvector_push_back(road.points, ((struct vec2d){.x=100,  .y=100}));
    cvector_push_back(road.points, ((struct vec2d){.x=200,  .y=50}));
    cvector_push_back(road.points, ((struct vec2d){.x=250,  .y=10}));

    struct mat4f tr;
    mat4f_identity(&tr);
    tr.m14 = 0;
    tr.m24 = 0;

    g_add_segment_array_strip(man,&(struct g_segment_array){
        .transform = tr,
        .line_type = g_ltSOLID,
        .color = {1,0,0,1},
        .width = 1,
        .vertices = road.points
    });

    g_add_test(man,&(struct g_test){
        .p1 = {-100,100},
        .p2 = {100,-100},
        .color = {1,0,1,1.0},
        .line_type = g_ltDOTDASH,
        .width = 10.0f
    });

    /*
    cvector(struct vec2d) new_points = NULL;
    line_array2d_equidistant(&new_points, vertices, 1);

    g_add_segment_array_strip(man,&(struct g_segment_array){
        .transform = tr,
        .line_type = g_ltSOLID,
        .color = {1,1,0,1},
        .width = 1,
        .vertices = new_points
    });
    */

    // NUKLEAR
    ctx = nk_glfw3_init(window, NK_GLFW3_DEFAULT);
    struct nk_font_atlas *atlas;
    struct nk_font_config config = nk_font_config(14);
    config.oversample_h = 4;
    config.oversample_v = 4;
    config.range = nk_font_cyrillic_glyph_ranges();
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font *font = nk_font_atlas_add_from_file(atlas,  "lib/nuklear/extra_font/Ubuntu-R.ttf", 13, &config);
    nk_glfw3_font_stash_end();
    nk_style_set_font(ctx, &font->handle);
    // END NUKLEAR

    // THREAD
    //pthread_t thread_id;
    //pthread_create(&thread_id, NULL, simulation_thread, NULL);
    // END THREAD

    // END INIT

    char fps_buf[16];
    char sfps_buf[16];
    double previousTime = glfwGetTime();
    int frameCount = 0;

    while ( not glfwWindowShouldClose( window ) ) {
        glfwWaitEvents();

        frameCount++;
        double currentTime = glfwGetTime();
        if ( currentTime - previousTime >= 1.0 ) {
            snprintf(fps_buf,sizeof(fps_buf),"FPS:%d",frameCount);
            snprintf(sfps_buf,sizeof(sfps_buf),"SFPS:%.2f",sfps);
            frameCount = 0;
            previousTime = currentTime;
        }

        glClearColor(0.40,0.40,0.45,1.00);
        glClear( GL_COLOR_BUFFER_BIT );

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        g_draw(man, cam);

        // NUKLEAR
        nk_glfw3_new_frame();
        if (nk_begin(ctx, "Info", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            nk_layout_row_dynamic(ctx, 0, 1);
            nk_label(ctx, fps_buf, NK_TEXT_LEFT);
            nk_label(ctx, sfps_buf, NK_TEXT_LEFT);
        }
        nk_end(ctx);

        nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
        // END NUKLEAR

        glfwSwapBuffers( window );
    }
    //pthread_exit(NULL);
    g_camera_free(cam);
    return EXIT_SUCCESS;
}
