/*
    GeoMetry
    v.0.2
*/

#ifndef GM_H
#define GM_H

#include "mathc.h"
#include "cvector.h"

struct gm_segment3i {
    struct vec3i start;
    struct vec3i end;
};

struct gm_triangle3i {
    struct vec3i p[3];
    struct vec3 normal;
};

struct gm_extremes3i {
    union {
        struct {
            const struct vec3i* min_x;
            const struct vec3i* max_x;
            const struct vec3i* min_y;
            const struct vec3i* max_y;
            const struct vec3i* min_z;
            const struct vec3i* max_z;
        };
        const struct vec3i* data[6];
    };
};

struct gm_convex_hull3i {
    cvector_vector_type(struct gm_triangle3i) tris;
};

void gm_update_iconvex_hull3(struct gm_convex_hull3i* ch, const cvector_vector_type(struct vec3i) points, const struct gm_extremes3i* extremes);
bool gm_is_ivec3_inside_convex_hull(const struct gm_convex_hull3i* ch, const struct vec3i* point);

#ifdef DEBUG
#include <stdio.h>

#define GM_TRACE printf("%s:%d ",__MAPYR__FILENAME__,__LINE__)

#define GM_PRINT_IVEC3(vector)          \
printf("[x=%d y=%d z=%d]",              \
vector.x,                               \
vector.y,                               \
vector.z);

#define GM_PRINT_FVEC3(vector)          \
printf("[x=%.2f y=%.2f z=%.2f]",        \
vector.x,                               \
vector.y,                               \
vector.z);

#define GM_PRINT_DVEC3(vector) GM_PRINT_FVEC3(vector)

#define GM_PRINT_FMAT4(mat)                                                         \
printf("| %5.2f | %5.2f | %5.2f | %5.2f |\n", (mat).m11, (mat).m12, (mat).m13, (mat).m14);\
printf("| %5.2f | %5.2f | %5.2f | %5.2f |\n", (mat).m21, (mat).m22, (mat).m23, (mat).m24);\
printf("| %5.2f | %5.2f | %5.2f | %5.2f |\n", (mat).m31, (mat).m32, (mat).m33, (mat).m34);\
printf("| %5.2f | %5.2f | %5.2f | %5.2f |\n", (mat).m41, (mat).m42, (mat).m43, (mat).m44);

#define GM_PRINT_ISEGMENT3(segment)     \
GM_PRINT_IVEC3(segment.start)           \
GM_PRINT_IVEC3(segment.end)

#define GM_PRINT_ITRIANGLE3(triangle)   \
for(int i =0; i<3; i++){                \
    GM_PRINT_IVEC3(triangle.p[i])       \
}                                       \
                                        \
printf(" normal=");                     \
GM_PRINT_FVEC3(triangle.normal);

#define GM_PRINT_CONVEX_PLANE(plane)                \
GM_PRINT_ITRIANGLE3(plane.triangle)                 \
printf(" outside_points=");                         \
cvector_iterator(gm_vct3i) pnt;                     \
cvector_for_each_in(pnt,(plane.outside_points)){    \
    GM_PRINT_IVEC3((*pnt));                         \
}

#endif

#endif