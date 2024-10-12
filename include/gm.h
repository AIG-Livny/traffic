/*
    GeoMetry
*/

#ifndef GM_H
#define GM_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "cvector.h"

#ifndef GM_TYPE
#define GM_TYPE int32_t
#endif

#define GM_DIMS 3
#define GM_X 0
#define GM_Y 1
#define GM_Z 2

union _gm_vct3f {
    struct {
        double x,y,z;
    };
    double data[GM_DIMS];
};

typedef union _gm_vct3f gm_vct3f;

union _gm_vct3i {
    struct {
        GM_TYPE x,y,z;
    };
    GM_TYPE data[GM_DIMS];
};

typedef union _gm_vct3i gm_vct3i;

struct gm_recti {
    gm_vct3i min;
    gm_vct3i max;
};

struct gm_segmenti {
    gm_vct3i start;
    gm_vct3i end;
};

struct gm_trianglei {
    gm_vct3i p[3];
    gm_vct3f normal;
};

union gm_extremes {
    struct {
        const gm_vct3i* min_x; 
        const gm_vct3i* max_x; 
        const gm_vct3i* min_y; 
        const gm_vct3i* max_y; 
        const gm_vct3i* min_z; 
        const gm_vct3i* max_z;
    };
    const gm_vct3i* data[6];
};

struct gm_convex_hull {
    cvector_vector_type(struct gm_trianglei) tris;
};

#define GM_EL(r,c) r*GM_DIMS+c
static const int8_t _ortho_bases[24][9] = {
    { 1, 0, 0, 0, 1, 0, 0, 0, 1},//0
    { 0,-1, 0, 1, 0, 0, 0, 0, 1},//1
    {-1, 0, 0, 0,-1, 0, 0, 0, 1},//2
    { 0, 1, 0,-1, 0, 0, 0, 0, 1},//3

    { 1, 0, 0, 0, 0,-1, 0, 1, 0},//4
    { 0, 0, 1, 1, 0, 0, 0, 1, 0},//5
    {-1, 0, 0, 0, 0, 1, 0, 1, 0},//6
    { 0, 0,-1,-1, 0, 0, 0, 1, 0},//7

    { 1, 0, 0, 0,-1, 0, 0, 0,-1},//8
    { 0, 1, 0, 1, 0, 0, 0, 0,-1},//9
    {-1, 0, 0, 0, 1, 0, 0, 0,-1},//10
    { 0,-1, 0,-1, 0, 0, 0, 0,-1},//11

    { 1, 0, 0, 0, 0, 1, 0,-1, 0},//12
    { 0, 0,-1, 1, 0, 0, 0,-1, 0},//13
    {-1, 0, 0, 0, 0,-1, 0,-1, 0},//14
    { 0, 0, 1,-1, 0, 0, 0,-1, 0},//15

    { 0, 0, 1, 0, 1, 0,-1, 0, 0},//16
    { 0,-1, 0, 0, 0, 1,-1, 0, 0},//17
    { 0, 0,-1, 0,-1, 0,-1, 0, 0},//18
    { 0, 1, 0, 0, 0,-1,-1, 0, 0},//19

    { 0, 0, 1, 0,-1, 0, 1, 0, 0},//20
    { 0, 1, 0, 0, 0, 1, 1, 0, 0},//21
    { 0, 0,-1, 0, 1, 0, 1, 0, 0},//22
    { 0,-1, 0, 0, 0,-1, 1, 0, 0},//23
};

// Corner vector ortho base
enum {
    cvBackDownLeft=0,
    cvFrontDownLeft=1,
    cvFrontDownRight=2,
    cvBackDownRight=3,
    cvBackUpLeft=4,
    cvBackUpRight=7,
    cvFrontUpLeft=8,
    cvFrontUpRight=11,

    cvFrontUpRight2=14,
    cvFrontUpRight3=18,
    cvFrontUpLeft2=13,
    cvFrontUpLeft3=23,
    cvFrontDownRight2=15,
    cvFrontDownRight3=17,
    cvFrontDownLeft2=12,
    cvFrontDownLeft3=20,
    cvBackUpRight2=10,
    cvBackUpRight3=19,
    cvBackUpLeft2=9,
    cvBackUpLeft3=22,
    cvBackDownRight2=6,
    cvBackDownRight3=16,
    cvBackDownLeft2=5,
    cvBackDownLeft3=21,
};

/*
gm_vct3i vct = {1,1,1};
GM_VCT3_ROTATE(vct,i);

num=0   [x=1 y=1 z=1]
num=5   [x=1 y=1 z=1]
num=21  [x=1 y=1 z=1]

num=1   [x=1 y=-1 z=1]
num=12  [x=1 y=-1 z=1]
num=20  [x=1 y=-1 z=1]

num=2   [x=-1 y=-1 z=1]
num=15  [x=-1 y=-1 z=1]
num=17  [x=-1 y=-1 z=1]

num=3   [x=-1 y=1 z=1]
num=6   [x=-1 y=1 z=1]
num=16  [x=-1 y=1 z=1]

num=4   [x=1 y=1 z=-1]
num=9   [x=1 y=1 z=-1]
num=22  [x=1 y=1 z=-1]

num=7   [x=-1 y=1 z=-1]
num=10  [x=-1 y=1 z=-1]
num=19  [x=-1 y=1 z=-1]

num=8   [x=1 y=-1 z=-1]
num=13  [x=1 y=-1 z=-1]
num=23  [x=1 y=-1 z=-1]

num=11  [x=-1 y=-1 z=-1]
num=14  [x=-1 y=-1 z=-1]
num=18  [x=-1 y=-1 z=-1]

0 1 2 3 4  7 8  11
*/


#define GM_VCT3_DOT(a,b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)
#define GM_VCT3_LENGTH_SQUARED(v) GM_VCT3_DOT(v,v)
#define GM_VCT3_LENGTH(v) sqrt(GM_VCT3_LENGTH_SQUARED(v))
#define GM_VCT3_DISTANCE_SQUARED(v1,v2) ((((v1).x-(v2).x)*((v1).x-(v2).x))+(((v1).y-(v2).y)*((v1).y-(v2).y))+(((v1).z-(v2).z)*((v1).z-(v2).z))) 
#define GM_VCT3_DISTANCE(v1,v2) sqrt(GM_VCT3_DISTANCE_SQUARED(v1,v2)) 

#define GM_VCT3_ROTATE(v, num){                                 \
    const int8_t* matrix = (const int8_t*)&_ortho_bases[(num)]; \
    typeof(v) result;                                           \
                                                                \
    result.x = matrix[GM_EL(GM_X,GM_X)] * (v).x +               \
               matrix[GM_EL(GM_Y,GM_X)] * (v).y +               \
               matrix[GM_EL(GM_Z,GM_X)] * (v).z;                \
    result.y = matrix[GM_EL(GM_X,GM_Y)] * (v).x +               \
               matrix[GM_EL(GM_Y,GM_Y)] * (v).y +               \
               matrix[GM_EL(GM_Z,GM_Y)] * (v).z;                \
    result.z = matrix[GM_EL(GM_X,GM_Z)] * (v).x +               \
               matrix[GM_EL(GM_Y,GM_Z)] * (v).y +               \
               matrix[GM_EL(GM_Z,GM_Z)] * (v).z;                \
                                                                \
    memcpy(&(v),&result,sizeof(v));                             \
}   

#define GM_VCT3_ADD(result_vector, other_vector){         \
    (result_vector).data[0] += (other_vector).data[0];    \
    (result_vector).data[1] += (other_vector).data[1];    \
    (result_vector).data[2] += (other_vector).data[2];    \
}

#define GM_VCT3_SUBST(result_vector, other_vector){         \
    (result_vector).data[0] -= (other_vector).data[0];    \
    (result_vector).data[1] -= (other_vector).data[1];    \
    (result_vector).data[2] -= (other_vector).data[2];    \
}

#define GM_VCT3_MULT(result_vector, a){ \
    (result_vector).data[0] *= (a);     \
    (result_vector).data[1] *= (a);     \
    (result_vector).data[2] *= (a);     \
}

#define GM_VCT3_EQ(a,b) (memcmp(&(a),&(b),sizeof(a))==0)

#define GM_VCT3_CROSS(result_vector, b){               \
  typeof(result_vector) tmp = result_vector;           \
  (result_vector).x = tmp.y * (b).z - tmp.z * (b).y;   \
  (result_vector).y = tmp.z * (b).x - tmp.x * (b).z;   \
  (result_vector).z = tmp.x * (b).y - tmp.y * (b).x;   \
}

#define GM_VCT3_DISTANCE_TO_LINE(ret, err, point, line_point1, line_point2){ \
    gm_vct3i tmp_v3 = (line_point2);                                         \
    GM_VCT3_SUBST(tmp_v3, (line_point1));                                    \
    double v3_len = GM_VCT3_LENGTH(tmp_v3);                                  \
    if (v3_len == 0){                                                        \
        err = 1;                                                             \
    } else {                                                                 \
        gm_vct3i tmp_v1 = (point);                                           \
        gm_vct3i tmp_v2 = (point);                                           \
        GM_VCT3_SUBST(tmp_v1, (line_point1));                                \
        GM_VCT3_SUBST(tmp_v2, (line_point2));                                \
        GM_VCT3_CROSS(tmp_v1,tmp_v2);                                        \
        ret = GM_VCT3_LENGTH(tmp_v1) / v3_len;                               \
    }                                                                        \
}

#define GM_VCT3_DISTANCE_TO_LINE_SQUARED(ret, err, point, line_point1, line_point2){ \
    gm_vct3i tmp_v3 = (line_point2);                                         \
    GM_VCT3_SUBST(tmp_v3, (line_point1));                                    \
    GM_TYPE v3_len = GM_VCT3_LENGTH_SQUARED(tmp_v3);                         \
    if (v3_len == 0){                                                        \
        err = 1;                                                             \
    } else {                                                                 \
        gm_vct3i tmp_v1 = (point);                                           \
        gm_vct3i tmp_v2 = (point);                                           \
        GM_VCT3_SUBST(tmp_v1, (line_point1));                                \
        GM_VCT3_SUBST(tmp_v2, (line_point2));                                \
        GM_VCT3_CROSS(tmp_v1,tmp_v2);                                        \
        ret = GM_VCT3_LENGTH_SQUARED(tmp_v1) / v3_len;                       \
    }                                                                        \
}

#define GM_VCT3_NORMALIZE(v){        \
    double len = GM_VCT3_LENGTH(v);  \
    (v).x /= len;                    \
    (v).y /= len;                    \
    (v).z /= len;                    \
}

#define GM_TRIANGLEI_DIST(ret, tri, point){                 \
    typeof(point) tmp_v = (point);                          \
    GM_VCT3_SUBST(tmp_v,  (tri).p[0]);                      \
    gm_vct3f tmp_vf = {.x=tmp_v.x, .y=tmp_v.y, .z=tmp_v.z}; \
    (ret) = GM_VCT3_DOT((tri).normal, tmp_vf);              \
}

#define GM_TRIANGLEI_CALC_NORM(tri){    \
    gm_vct3i tmp_v1 = (tri).p[0];       \
    gm_vct3i tmp_v2 = (tri).p[1];       \
    GM_VCT3_SUBST(tmp_v1, (tri).p[1]);  \
    GM_VCT3_SUBST(tmp_v2, (tri).p[2]);  \
    GM_VCT3_CROSS(tmp_v1, tmp_v2);      \
    (tri).normal = (gm_vct3f){          \
        .x=tmp_v1.x,                    \
        .y=tmp_v1.y,                    \
        .z=tmp_v1.z,                    \
    };                                  \
    GM_VCT3_NORMALIZE((tri).normal);    \
}

#define GM_RECT_EXTEND(rect, value){            \
    gm_vct3i vct = {(value),(value),(value)};   \
    GM_VCT3_SUBST((rect).min, vct);             \
    GM_VCT3_ADD((rect).max, vct);               \
}

void gm_update_convex_hull(struct gm_convex_hull* ch, const cvector_vector_type(gm_vct3i) points, const union gm_extremes* extremes);
bool gm_is_vct3i_inside_convex_hull(const struct gm_convex_hull* ch, const gm_vct3i* point);


#ifdef DEBUG
#include <stdio.h>

#define GM_TRACE printf("%s:%d ",__MAPYR__FILENAME__,__LINE__)

#define GM_PRINT_VCT3I(vector)          \
printf("[x=%d y=%d z=%d]",              \
vector.x,                               \
vector.y,                               \
vector.z);

#define GM_PRINT_VCT3F(vector)          \
printf("[x=%.2f y=%.2f z=%.2f]",        \
vector.x,                               \
vector.y,                               \
vector.z);

#define GM_PRINT_SEGMENTI(segment)      \
GM_PRINT_VCT3I(segment.start)           \
GM_PRINT_VCT3I(segment.end)

#define GM_PRINT_TRIANGLE(triangle)     \
for(int i =0; i<3; i++){                \
    GM_PRINT_VCT3I(triangle.p[i])       \
}                                       \
                                        \
printf(" normal=");                     \
GM_PRINT_VCT3F(triangle.normal);

#define GM_PRINT_CONVEX_PLANE(plane)                \
GM_PRINT_TRIANGLE(plane.triangle)                   \
printf(" outside_points=");                         \
cvector_iterator(gm_vct3i) pnt;                     \
cvector_for_each_in(pnt,(plane.outside_points)){    \
    GM_PRINT_VCT3I((*pnt));                         \
}

#endif

#endif