#include "geometry.h"

#include <memory.h>
#include <stdbool.h>
#include <iso646.h>

#include "cvector_utils.h"

double gm_triangle_dist( struct gm_triangle3i tri, struct vec3* point){
    struct vec3 tmp_v;
    vec3_subtract(&tmp_v, point, &tri.p[0]);
    struct vec3 tmp_vf = {.x=tmp_v.x, .y=tmp_v.y, .z=tmp_v.z};
    return vec3_dot(&tri.normal, &(struct vec3){.x=tmp_v.x, .y=tmp_v.y, .z=tmp_v.z});
}


#define GM_TRIANGLE3_CALC_NORM(tri){        \
    typeof((tri).p[0]) tmp_v1 = (tri).p[0]; \
    typeof((tri).p[0]) tmp_v2 = (tri).p[1]; \
    GM_VEC3_SUBST(tmp_v1, (tri).p[1]);      \
    GM_VEC3_SUBST(tmp_v2, (tri).p[2]);      \
    GM_VEC3_CROSS(tmp_v1, tmp_v2);          \
    (tri).normal = (struct vec3){           \
        .x=tmp_v1.x,                        \
        .y=tmp_v1.y,                        \
        .z=tmp_v1.z,                        \
    };                                      \
    GM_VEC3_NORMALIZE((tri).normal);        \
}

#define GM_RECT_EXTEND(rect, value){                    \
    typeof((rect).min) vct = {(value),(value),(value)}; \
    GM_VEC3_SUBST((rect).min, vct);                     \
    GM_VEC3_ADD((rect).max, vct);                       \
}

#define GM_VEC3_DISTANCE_TO_LINE(ret, err, point, line_point1, line_point2){ \
    struct vec3i tmp_v3 = (line_point2);                                     \
    GM_VEC3_SUBST(tmp_v3, (line_point1));                                    \
    double v3_len = GM_VEC3_LENGTH(tmp_v3);                                  \
    if (v3_len == 0){                                                        \
        err = 1;                                                             \
    } else {                                                                 \
        struct vec3i tmp_v1 = (point);                                       \
        struct vec3i tmp_v2 = (point);                                       \
        GM_VEC3_SUBST(tmp_v1, (line_point1));                                \
        GM_VEC3_SUBST(tmp_v2, (line_point2));                                \
        GM_VEC3_CROSS(tmp_v1,tmp_v2);                                        \
        ret = GM_VEC3_LENGTH(tmp_v1) / v3_len;                               \
    }                                                                        \
}

static void gm_find_extremes(struct gm_extremes3i* extremes, const cvector_vector_type(struct vec3i) points){
    *extremes = (struct gm_extremes3i){.data = {points,points,points,points,points,points}};

    #define SET_MAX(axis)                           \
        if (it->axis > extremes->max_##axis->axis){ \
            extremes->max_##axis=it;                \
        }
    #define SET_MIN(axis)                           \
        if (it->axis < extremes->min_##axis->axis){ \
            extremes->min_##axis=it;                \
        }

    struct vec3i* it;
    cvector_for_each_in(it, (cvector_vector_type(struct vec3i))points){
        SET_MIN(x);SET_MAX(x);
        SET_MIN(y);SET_MAX(y);
        SET_MIN(z);SET_MAX(z);
    }
}

struct convex_plane {
    struct gm_triangle3i triangle;
    cvector_vector_type(struct vec3i) outside_points;
};

// Calculating the horizon for an eye to make new faces
bool calc_horizon(
    cvector_vector_type(int) all_planes,
    cvector_vector_type(int) visited_planes,
    int current_plane,
    struct vec3i* eye_point,
    cvector_vector_type(struct gm_segment3i) segments,
    cvector_vector_type(struct convex_plane) plane_storage
    )
{
    cvector_iterator(int) plane;
    double dist = gm_triangle_dist( plane_storage[current_plane].triangle, eye_point);

    if( dist > 0){
        cvector_push_back(visited_planes, current_plane);
        for(int i = 0; i < 3; i++){

            // Edge
            struct vec3i* e1 = &plane_storage[current_plane].triangle.p[i];
            struct vec3i* e2;
            if (i < 2){e2 = &plane_storage[current_plane].triangle.p[i+1];}else{e2 = &plane_storage[current_plane].triangle.p[0];}

            // Finding adjacent planes to an edge
            int adjacent = -1;
            cvector_for_each_in(plane, all_planes){
                if (*plane == current_plane){
                    continue;
                }

                bool e1_found = false;
                bool e2_found = false;
                for(int i = 0; i < 3; i++){
                    if(not e1_found)
                    if(memcmp(e1,&plane_storage[*plane].triangle.p[i],sizeof(struct vec3i)) == 0){
                        e1_found = true;

                    }
                    if(not e2_found)
                    if(memcmp(e2,&plane_storage[*plane].triangle.p[i],sizeof(struct vec3i)) == 0){
                        e2_found = true;
                    }
                }

                if (e1_found and e2_found){
                    adjacent = *plane;
                    break;
                }
            }

            if(adjacent == -1){
                continue;
            }

            // If not adjacent in visited_planes
            cvector_iterator(int) it_vp;
            cvector_for_each_in(it_vp, visited_planes){
                if(adjacent == *it_vp){
                    break;
                }
            }
            if(it_vp == cvector_end(visited_planes)){
                bool result = calc_horizon(all_planes, visited_planes, adjacent, eye_point, segments, plane_storage);
                if (result){
                    cvector_push_back(segments, ((struct gm_segment3i){.start=*e1, .end=*e2}) );
                }
            }
        }
        return false;
    }else{
        return true;
    }
}

// Quickhull for 3d integer space
static void gm_update_iconvex_hull30(struct gm_convex_hull3i* ch, const cvector_vector_type(struct vec3i) points, const struct gm_extremes3i* extremes){
    // Delete old data
    cvector_free(ch->tris);
    ch->tris = NULL;

    // If no extremes then find them
    struct gm_extremes3i tmp_extremes;
    struct gm_extremes3i* pextremes = (struct gm_extremes3i*)extremes;

    if ( not extremes ) {
        pextremes = &tmp_extremes;
        gm_find_extremes(pextremes, points);
    }

    // Check if we have 3d structure
    if(pextremes->min_x == pextremes->max_x){return;}
    if(pextremes->min_y == pextremes->max_y){return;}
    if(pextremes->min_z == pextremes->max_z){return;}

    // From the extreme points calculate the 2 most distant points
    double max_dist = 0;
    const struct vec3i* start_points[4];
    for(int i = 0; i < 3*2; i++){
        for(int j = i+1; j < 3*2; j++){
            double dist = GM_VEC3_DISTANCE_SQUARED(*pextremes->data[i],*pextremes->data[j]);
            if(dist > max_dist){
                max_dist = dist;
                start_points[0] = pextremes->data[i];
                start_points[1] = pextremes->data[j];
            }
        }
    }

    if (max_dist == 0){
        // p[0] - p[1] is not line!
        return;
    }

    // Let's find third point
    // Most distant from line
    max_dist = 0;
    cvector_iterator(struct vec3i) point;
    cvector_for_each_in(point, (cvector_vector_type(struct vec3i))points){
        if( (start_points[0] == point) or (start_points[1] == point) ){
            continue;
        }

        double dist = 0;
        bool error = false;
        GM_VEC3_DISTANCE_TO_LINE_SQUARED(dist, error, *point, *start_points[0],*start_points[1]);
        if(error){continue;}
        if ( fabs(dist) > max_dist ){
            max_dist = dist;
            start_points[2] = point;
        }
    }

    // Make first plane
    cvector_vector_type(struct convex_plane) plane_storage = NULL;
    cvector_reserve(plane_storage, 40);

    cvector_push_back(plane_storage,
        ((struct convex_plane){
            .triangle=(struct gm_triangle3i){.p={*start_points[0],*start_points[1],*start_points[2]}},
            .outside_points = NULL,
        }));
    cvector_init(cvector_back(plane_storage)->outside_points, 20, NULL);
    struct convex_plane* cp = cvector_back(plane_storage);
    GM_TRIANGLE3_CALC_NORM(cp->triangle);

    // Fourth point
    max_dist = 0;
    cvector_for_each_in(point, (cvector_vector_type(struct vec3i))points){
        double dist;
        struct vec3 pnt = {point->x,point->y,point->z};
        GM_TRIANGLE3_DIST(dist, cp->triangle, pnt);
        if ( fabs(dist) > max_dist ){
            max_dist = dist;
            start_points[3] = point;
        }
    }

    // Other 3 planes
    cvector_push_back(plane_storage,
        ((struct convex_plane){
            .triangle=(struct gm_triangle3i){.p={*start_points[0],*start_points[1],*start_points[3]}},
            .outside_points = NULL,
        }));
    cvector_init(cvector_back(plane_storage)->outside_points, 20, NULL);
    GM_TRIANGLE3_CALC_NORM(cvector_back(plane_storage)->triangle);

    cvector_push_back(plane_storage,
        ((struct convex_plane){
            .triangle=(struct gm_triangle3i){.p={*start_points[0],*start_points[3],*start_points[2]}},
            .outside_points = NULL,
        }));
    cvector_init(cvector_back(plane_storage)->outside_points, 20, NULL);
    GM_TRIANGLE3_CALC_NORM(cvector_back(plane_storage)->triangle);

    cvector_push_back(plane_storage,
        ((struct convex_plane){
            .triangle=(struct gm_triangle3i){.p={*start_points[1],*start_points[2],*start_points[3]}},
            .outside_points = NULL,
        }));
    cvector_init(cvector_back(plane_storage)->outside_points, 20, NULL);
    GM_TRIANGLE3_CALC_NORM(cvector_back(plane_storage)->triangle);


    cvector_iterator(struct convex_plane) stored_plane;
    cvector_for_each_in(stored_plane, plane_storage){
        // Correct normals
        for (int pi = 0; pi < 4; pi++){
            struct vec3i tmp_v = *start_points[pi];
            GM_VEC3_SUBST(tmp_v, stored_plane->triangle.p[0]);
            struct vec3 tmp_vf = {.x=tmp_v.x, .y=tmp_v.y, .z=tmp_v.z};
            double dist = GM_VEC3_DOT( stored_plane->triangle.normal, tmp_vf);
            if(dist > 0){
                GM_VEC3_MULT(stored_plane->triangle.normal, -1);
            }
        }

        // Find all outside points
        cvector_for_each_in(point, (cvector_vector_type(struct vec3i))points){
            double dist;
            GM_TRIANGLE3_DIST(dist, stored_plane->triangle, *point);
            if (dist > 0){
                cvector_push_back(stored_plane->outside_points,*point);
            }
        }
    }

    cvector_vector_type(int) visited_planes             = NULL;
    cvector_vector_type(int) planes                     = NULL;
    cvector_vector_type(struct gm_segment3i) segments    = NULL;

    cvector_reserve(visited_planes, 40);
    cvector_reserve(planes, 40);
    cvector_reserve(segments, 40);

    cvector_iterator(struct gm_segment3i) segment;
    cvector_iterator(int) visited_plane;
    cvector_iterator(int) plane;
    cvector_iterator(int) plane2;

    // Add all plane indicies to vector
    cvector_for_each_in(stored_plane, plane_storage){
        cvector_push_back(planes, stored_plane - plane_storage);
    }

    bool any_left = true;
    while (any_left){
        any_left = false;
        cvector_for_each_in(plane, planes){
            if(cvector_size(plane_storage[*plane].outside_points)){
                any_left = true;

                // Calculate the eye point of the face
                struct vec3i eye_point;
                max_dist = 0;
                cvector_for_each_in(point, plane_storage[*plane].outside_points){
                    double dist;
                    GM_TRIANGLE3_DIST(dist, plane_storage[*plane].triangle, *point);
                    if(dist > max_dist){
                        max_dist = dist;
                        eye_point = *point;
                    }
                }

                calc_horizon(planes, visited_planes, *plane, &eye_point, segments, plane_storage);

                // Delete all visited indicies from planes
                cvector_for_each_in(visited_plane, visited_planes){
                    cvector_for_each_in(plane2, planes){
                        if(*plane2 == *visited_plane){
                            cvector_erase(planes,plane2-planes);
                            break;
                        }
                    }
                }

                cvector_for_each_in(segment, segments){
                    cvector_push_back(plane_storage, ((struct convex_plane){
                        .triangle={.p={
                                segment->start,
                                segment->end,
                                eye_point
                            }
                        },
                        .outside_points = NULL
                    }));

                    struct convex_plane* new_plane;
                    new_plane = cvector_back(plane_storage);
                    cvector_push_back(planes, cvector_size(plane_storage)-1);

                    GM_TRIANGLE3_CALC_NORM(new_plane->triangle);

                    // Correct normal new plane
                    for (int pi = 0; pi < 4; pi++){
                        struct vec3i tmp_v = *start_points[pi];
                        GM_VEC3_SUBST(tmp_v, new_plane->triangle.p[0]);
                        struct vec3 tmp_vf = {.x=tmp_v.x, .y=tmp_v.y, .z=tmp_v.z};
                        double dist = GM_VEC3_DOT( new_plane->triangle.normal, tmp_vf);
                        if(dist > 0){
                            GM_VEC3_MULT(new_plane->triangle.normal, -1);
                        }
                    }

                    // Find all outside points for new plane
                    cvector_for_each_in(visited_plane, visited_planes){
                        cvector_for_each_in(point, plane_storage[*visited_plane].outside_points){
                            double dist;
                            GM_TRIANGLE3_DIST(dist, new_plane->triangle, *point);
                            if (dist > 0){
                                cvector_push_back(new_plane->outside_points,*point);
                            }
                        }
                    }
                }

                cvector_clear(visited_planes);
                cvector_clear(segments);
            }
        }
    }

    // Copy planes to convex hull
    cvector_iterator(struct gm_triangle3i) tris;
    cvector_init(ch->tris, cvector_size(planes), NULL);
    cvector_for_each_in(plane, planes){
        cvector_push_back(ch->tris, plane_storage[*plane].triangle);
    }

    // Free data
    cvector_free(planes);
    cvector_free(visited_planes);
    cvector_free(segments);
    cvector_for_each_in(stored_plane, stored_plane){
        cvector_free(stored_plane->outside_points);
    }
    cvector_free(plane_storage);
}

void gm_update_iconvex_hull3(struct gm_convex_hull3i* ch, const cvector_vector_type(struct vec3i) points, const struct gm_extremes3i* extremes){
    gm_update_iconvex_hull30(ch,points,extremes);
}

bool gm_is_ivec3_inside_convex_hull(const struct gm_convex_hull3i* ch, const struct vec3i* point){
    cvector_iterator(struct gm_triangle3i) it;
    cvector_for_each_in(it, ch->tris){
        double dist;
        GM_TRIANGLE3_DIST(dist, *it, *(struct vec3i*)point);
        if(dist > 0){
            return false;
        }
    }
    return true;
}