/*
    GeoMetry
    v.0.1
*/

#ifndef GM_H
#define GM_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "cvector.h"

#define GM_X 0
#define GM_Y 1
#define GM_Z 2
#define GM_W 3

#define GM_R 0
#define GM_G 1
#define GM_B 2
#define GM_A 3

#define GM_LEFT     0
#define GM_TOP      1
#define GM_WIDTH    2
#define GM_HEIGHT   3

union gm_vct2f {
    struct {
        double x,y;
    };
    double data[2];
};

union gm_vct2i {
    struct {
        int32_t x,y;
    };
    int32_t data[2];
};

union gm_vct3f {
    struct {
        double x,y,z;
    };
    double data[3];
};

union gm_vct3i {
    struct {
        int32_t x,y,z;
    };
    int32_t data[3];
};

/*
Matrix 4×4 representation:
0/m11 4/m12  8/m13 12/m14
1/m21 5/m22  9/m23 13/m24
2/m31 6/m32 10/m33 14/m34
3/m41 7/m42 11/m43 15/m44
*/
union gm_mat4f {
    struct {
        double m11;
        double m21;
        double m31;
        double m41;
        double m12;
        double m22;
        double m32;
        double m42;
        double m13;
        double m23;
        double m33;
        double m43;
        double m14;
        double m24;
        double m34;
        double m44;
    };
    double data[16];
};

struct gm_recti {
    union gm_vct3i min;
    union gm_vct3i max;
};

struct gm_segmenti {
    union gm_vct3i start;
    union gm_vct3i end;
};

struct gm_segmentf {
    union gm_vct3f start;
    union gm_vct3f end;
};

struct gm_trianglei {
    union gm_vct3i p[3];
    union gm_vct3f normal;
};

union gm_extremes {
    struct {
        const union gm_vct3i* min_x; 
        const union gm_vct3i* max_x; 
        const union gm_vct3i* min_y; 
        const union gm_vct3i* max_y; 
        const union gm_vct3i* min_z; 
        const union gm_vct3i* max_z;
    };
    const union gm_vct3i* data[6];
};

struct gm_convex_hull {
    cvector_vector_type(struct gm_trianglei) tris;
};

#define GM_EL(r,c) r*3+c
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

#define GM_VCT2_DOT(a,b) ((a).x * (b).x + (a).y * (b).y)
#define GM_VCT2_LENGTH_SQUARED(v) GM_VCT2_DOT(v,v)
#define GM_VCT2_LENGTH(v) sqrt(GM_VCT2_LENGTH_SQUARED(v))
#define GM_VCT2_DISTANCE_SQUARED(v1,v2) ((((v1).x-(v2).x)*((v1).x-(v2).x))+(((v1).y-(v2).y)*((v1).y-(v2).y))) 
#define GM_VCT2_DISTANCE(v1,v2) sqrt(GM_VCT2_DISTANCE_SQUARED(v1,v2)) 

#define GM_VCT3_DOT(a,b) (GM_VCT2_DOT(a,b) + (a).z * (b).z)
#define GM_VCT3_LENGTH_SQUARED(v) GM_VCT3_DOT(v,v)
#define GM_VCT3_LENGTH(v) sqrt(GM_VCT3_LENGTH_SQUARED(v))
#define GM_VCT3_DISTANCE_SQUARED(v1,v2) ((((v1).x-(v2).x)*((v1).x-(v2).x))+(((v1).y-(v2).y)*((v1).y-(v2).y))+(((v1).z-(v2).z)*((v1).z-(v2).z))) 
#define GM_VCT3_DISTANCE(v1,v2) sqrt(GM_VCT3_DISTANCE_SQUARED(v1,v2)) 

#define GM_VCT3I_ROTATE(v, num){                                \
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

#define GM_VCT2_ADD(result_vector, other_vector){         \
    (result_vector).data[0] += (other_vector).data[0];    \
    (result_vector).data[1] += (other_vector).data[1];    \
}

#define GM_VCT2_SUBST(result_vector, other_vector){       \
    (result_vector).data[0] -= (other_vector).data[0];    \
    (result_vector).data[1] -= (other_vector).data[1];    \
}

#define GM_VCT2_MULT(result_vector, a){ \
    (result_vector).data[0] *= (a);     \
    (result_vector).data[1] *= (a);     \
}

#define GM_VCT3_ADD(result_vector, other_vector){         \
    GM_VCT2_ADD(result_vector, other_vector);             \
    (result_vector).data[2] += (other_vector).data[2];    \
}

#define GM_VCT3_SUBST(result_vector, other_vector){       \
    GM_VCT2_SUBST(result_vector, other_vector);           \
    (result_vector).data[2] -= (other_vector).data[2];    \
}

#define GM_VCT3_MULT(result_vector, a){ \
    GM_VCT2_MULT(result_vector,a);      \
    (result_vector).data[2] *= (a);     \
}

#define GM_VCT_EQ(a,b) (memcmp(&(a),&(b),sizeof(a))==0)

#define GM_VCT3_CROSS(result_vector, b){               \
  typeof(result_vector) tmp = result_vector;           \
  (result_vector).x = tmp.y * (b).z - tmp.z * (b).y;   \
  (result_vector).y = tmp.z * (b).x - tmp.x * (b).z;   \
  (result_vector).z = tmp.x * (b).y - tmp.y * (b).x;   \
}

#define GM_VCT3_DISTANCE_TO_LINE(ret, err, point, line_point1, line_point2){ \
    union gm_vct3i tmp_v3 = (line_point2);                                   \
    GM_VCT3_SUBST(tmp_v3, (line_point1));                                    \
    double v3_len = GM_VCT3_LENGTH(tmp_v3);                                  \
    if (v3_len == 0){                                                        \
        err = 1;                                                             \
    } else {                                                                 \
        union gm_vct3i tmp_v1 = (point);                                     \
        union gm_vct3i tmp_v2 = (point);                                     \
        GM_VCT3_SUBST(tmp_v1, (line_point1));                                \
        GM_VCT3_SUBST(tmp_v2, (line_point2));                                \
        GM_VCT3_CROSS(tmp_v1,tmp_v2);                                        \
        ret = GM_VCT3_LENGTH(tmp_v1) / v3_len;                               \
    }                                                                        \
}

#define GM_VCT3_DISTANCE_TO_LINE_SQUARED(ret, err, point, line_point1, line_point2){ \
    union gm_vct3i tmp_v3 = (line_point2);                                   \
    GM_VCT3_SUBST(tmp_v3, (line_point1));                                    \
    int32_t v3_len = GM_VCT3_LENGTH_SQUARED(tmp_v3);                         \
    if (v3_len == 0){                                                        \
        err = 1;                                                             \
    } else {                                                                 \
        union gm_vct3i tmp_v1 = (point);                                     \
        union gm_vct3i tmp_v2 = (point);                                     \
        GM_VCT3_SUBST(tmp_v1, (line_point1));                                \
        GM_VCT3_SUBST(tmp_v2, (line_point2));                                \
        GM_VCT3_CROSS(tmp_v1,tmp_v2);                                        \
        ret = GM_VCT3_LENGTH_SQUARED(tmp_v1) / v3_len;                       \
    }                                                                        \
}

#define GM_VCT2_NORMALIZE(v){        \
    double len = GM_VCT2_LENGTH(v);  \
    (v).x /= len;                    \
    (v).y /= len;                    \
}

#define GM_VCT3_NORMALIZE(v){        \
    double len = GM_VCT3_LENGTH(v);  \
    (v).x /= len;                    \
    (v).y /= len;                    \
    (v).z /= len;                    \
}

#define GM_TRIANGLEI_DIST(ret, tri, point){                         \
    typeof(point) tmp_v = (point);                                  \
    GM_VCT3_SUBST(tmp_v,  (tri).p[0]);                              \
    union gm_vct3f tmp_vf = {.x=tmp_v.x, .y=tmp_v.y, .z=tmp_v.z};   \
    (ret) = GM_VCT3_DOT((tri).normal, tmp_vf);                      \
}

#define GM_TRIANGLEI_CALC_NORM(tri){    \
    union gm_vct3i tmp_v1 = (tri).p[0]; \
    union gm_vct3i tmp_v2 = (tri).p[1]; \
    GM_VCT3_SUBST(tmp_v1, (tri).p[1]);  \
    GM_VCT3_SUBST(tmp_v2, (tri).p[2]);  \
    GM_VCT3_CROSS(tmp_v1, tmp_v2);      \
    (tri).normal = (union gm_vct3f){    \
        .x=tmp_v1.x,                    \
        .y=tmp_v1.y,                    \
        .z=tmp_v1.z,                    \
    };                                  \
    GM_VCT3_NORMALIZE((tri).normal);    \
}

#define GM_RECT_EXTEND(rect, value){                \
    union gm_vct3i vct = {(value),(value),(value)}; \
    GM_VCT3_SUBST((rect).min, vct);                 \
    GM_VCT3_ADD((rect).max, vct);                   \
}

void gm_update_convex_hull(struct gm_convex_hull* ch, const cvector_vector_type(union gm_vct3i) points, const union gm_extremes* extremes);
bool gm_is_vct3i_inside_convex_hull(const struct gm_convex_hull* ch, const union gm_vct3i* point);

#endif