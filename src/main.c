#include <stdio.h>
#include <stdlib.h>
#include <iso646.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "graphics.h"
#include "debug.h"
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

struct g_camera* cam;
bool pan = false;

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods ) {
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ) {
        glfwSetWindowShouldClose( window, GL_TRUE );
    }
}

void scroll( GLFWwindow* window, double xoffset, double yoffset ) {
    g_camera_zoom( cam, -yoffset * g_camera_get_zoom(cam) * 0.04);
}

void mouse_button(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        pan = true;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        pan = false;
    }
}

double oldx;
double oldy;

void cursor_pos(GLFWwindow* window, double xpos, double ypos){
    double zoom = g_camera_get_zoom(cam);
    if ( pan ) {
        g_camera_move(cam,&(struct vec2d){-((xpos - oldx) * zoom), (ypos - oldy) * zoom});
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


int main( int argc, char **argv ) {
    DEBUG_TIMEBLOCK_START(startup);
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
    glfwSwapInterval(0);

    //glfwSetFramebufferSizeCallback( window, reshape );
    //glfwSetWindowRefreshCallback( window, display );
    glfwGetCursorPos(window, &oldx, &oldy);
    glfwSetKeyCallback( window, keyboard );
    glfwSetScrollCallback( window, scroll );
    glfwSetMouseButtonCallback( window, mouse_button );
    glfwSetCursorPosCallback( window, cursor_pos );

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
    cvector_push_back(road.points, ((struct vec2d){.x=-50, .y=0}));
    cvector_push_back(road.points, ((struct vec2d){.x=100,   .y=100}));
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
    #define NK_SHADER_VERSION "#version 330\n"
    struct nk_context *ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
    nk_glfw3_font_stash_end();
    struct nk_colorf bg;
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    // END NUKLEAR

    // END INIT
    DEBUG_TIMEBLOCK_STOP(startup);

    while ( not glfwWindowShouldClose( window ) ) {
        glfwWaitEvents();

        glClearColor(0.40,0.40,0.45,1.00);
        glClear( GL_COLOR_BUFFER_BIT );

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        g_draw(man, cam);

        // NUKLEAR
        nk_glfw3_new_frame();
        if (nk_begin(ctx, "Демо", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
        // END NUKLEAR

        glfwSwapBuffers( window );
    }
    g_camera_free(cam);
    return EXIT_SUCCESS;
}
