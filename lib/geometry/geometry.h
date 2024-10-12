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

union gm_fvec2 {
    struct {
        float x,y;
    };
    float data[2];
};

union gm_dvec2 {
    struct {
        double x,y;
    };
    double data[2];
};

union gm_ivec2 {
    struct {
        int32_t x,y;
    };
    int32_t data[2];
};

union gm_fvec3 {
    struct {
        float x,y,z;
    };
    float data[3];
};

union gm_dvec3 {
    struct {
        double x,y,z;
    };
    double data[3];
};

union gm_ivec3 {
    struct {
        int32_t x,y,z;
    };
    int32_t data[3];
};

union gm_fvec4 {
    struct {
        float x,y,z,w;
    };
    float data[3];
};

/*
Matrix 4×4 representation:
0/m11 4/m12  8/m13 12/m14
1/m21 5/m22  9/m23 13/m24
2/m31 6/m32 10/m33 14/m34
3/m41 7/m42 11/m43 15/m44
*/
union gm_dmat4 {
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

union gm_fmat4 {
    struct {
        float m11;
        float m21;
        float m31;
        float m41;
        float m12;
        float m22;
        float m32;
        float m42;
        float m13;
        float m23;
        float m33;
        float m43;
        float m14;
        float m24;
        float m34;
        float m44;
    };
    float data[16];
};


struct gm_irect3 {
    union gm_ivec3 min;
    union gm_ivec3 max;
};

struct gm_isegment3 {
    union gm_ivec3 start;
    union gm_ivec3 end;
};

struct gm_dsegment3 {
    union gm_dvec3 start;
    union gm_dvec3 end;
};

struct gm_itriangle3 {
    union gm_ivec3 p[3];
    union gm_dvec3 normal;
};

union gm_iextremes3 {
    struct {
        const union gm_ivec3* min_x;
        const union gm_ivec3* max_x;
        const union gm_ivec3* min_y;
        const union gm_ivec3* max_y;
        const union gm_ivec3* min_z;
        const union gm_ivec3* max_z;
    };
    const union gm_ivec3* data[6];
};

struct gm_iconvex_hull3 {
    cvector_vector_type(struct gm_itriangle3) tris;
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

#define GM_VEC2_DOT(a,b) ((a).x * (b).x + (a).y * (b).y)
#define GM_VEC2_LENGTH_SQUARED(v) GM_VEC2_DOT(v,v)
#define GM_VEC2_LENGTH(v) sqrt(GM_VEC2_LENGTH_SQUARED(v))
#define GM_VEC2_DISTANCE_SQUARED(v1,v2) ((((v1).x-(v2).x)*((v1).x-(v2).x))+(((v1).y-(v2).y)*((v1).y-(v2).y)))
#define GM_VEC2_DISTANCE(v1,v2) sqrt(GM_VEC2_DISTANCE_SQUARED(v1,v2))

#define GM_VEC3_DOT(a,b) (GM_VEC2_DOT(a,b) + (a).z * (b).z)
#define GM_VEC3_LENGTH_SQUARED(v) GM_VEC3_DOT(v,v)
#define GM_VEC3_LENGTH(v) sqrt(GM_VEC3_LENGTH_SQUARED(v))
#define GM_VEC3_DISTANCE_SQUARED(v1,v2) ((((v1).x-(v2).x)*((v1).x-(v2).x))+(((v1).y-(v2).y)*((v1).y-(v2).y))+(((v1).z-(v2).z)*((v1).z-(v2).z)))
#define GM_VEC3_DISTANCE(v1,v2) sqrt(GM_VEC3_DISTANCE_SQUARED(v1,v2))

#define GM_IVEC3_ROTATE(v, num){                                \
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

#define GM_VEC2_ADD(result_vector, other_vector){         \
    (result_vector).data[0] += (other_vector).data[0];    \
    (result_vector).data[1] += (other_vector).data[1];    \
}

#define GM_VEC2_SUBST(result_vector, other_vector){       \
    (result_vector).data[0] -= (other_vector).data[0];    \
    (result_vector).data[1] -= (other_vector).data[1];    \
}

#define GM_VEC2_MULT(result_vector, a){ \
    (result_vector).data[0] *= (a);     \
    (result_vector).data[1] *= (a);     \
}

#define GM_VEC2_DIV(result_vector, a){ \
    (result_vector).data[0] /= (a);     \
    (result_vector).data[1] /= (a);     \
}

#define GM_DVEC2_FROM_VEC2(vct2)    \
(union gm_dvec2){                   \
    .x = (vct2).x,                  \
    .y = (vct2).y                   \
}

#define GM_FVEC3_FROM_VEC2(vct2, _z)    \
(union gm_fvec3){                       \
    .x = (vct2).x,                      \
    .y = (vct2).y,                      \
    .z = _z                             \
}

#define GM_DVEC3_FROM_VEC2(vct2, _z)    \
(union gm_dvec3){                       \
    .x = (vct2).x,                      \
    .y = (vct2).y,                      \
    .z = _z                             \
}

#define GM_VEC3_ADD(result_vector, other_vector){         \
    GM_VEC2_ADD(result_vector, other_vector);             \
    (result_vector).data[2] += (other_vector).data[2];    \
}

#define GM_VEC3_SUBST(result_vector, other_vector){       \
    GM_VEC2_SUBST(result_vector, other_vector);           \
    (result_vector).data[2] -= (other_vector).data[2];    \
}

#define GM_VEC3_MULT(result_vector, a){ \
    GM_VEC2_MULT(result_vector,a);      \
    (result_vector).data[2] *= (a);     \
}

#define GM_VEC_EQ(a,b) (memcmp(&(a),&(b),sizeof(a))==0)

#define GM_VEC3_CROSS(result_vector, b){               \
  typeof(result_vector) tmp = result_vector;           \
  (result_vector).x = tmp.y * (b).z - tmp.z * (b).y;   \
  (result_vector).y = tmp.z * (b).x - tmp.x * (b).z;   \
  (result_vector).z = tmp.x * (b).y - tmp.y * (b).x;   \
}

#define GM_VEC3_DISTANCE_TO_LINE(ret, err, point, line_point1, line_point2){ \
    union gm_ivec3 tmp_v3 = (line_point2);                                   \
    GM_VEC3_SUBST(tmp_v3, (line_point1));                                    \
    double v3_len = GM_VEC3_LENGTH(tmp_v3);                                  \
    if (v3_len == 0){                                                        \
        err = 1;                                                             \
    } else {                                                                 \
        union gm_ivec3 tmp_v1 = (point);                                     \
        union gm_ivec3 tmp_v2 = (point);                                     \
        GM_VEC3_SUBST(tmp_v1, (line_point1));                                \
        GM_VEC3_SUBST(tmp_v2, (line_point2));                                \
        GM_VEC3_CROSS(tmp_v1,tmp_v2);                                        \
        ret = GM_VEC3_LENGTH(tmp_v1) / v3_len;                               \
    }                                                                        \
}

#define GM_VEC3_DISTANCE_TO_LINE_SQUARED(ret, err, point, line_point1, line_point2){ \
    union gm_ivec3 tmp_v3 = (line_point2);                                   \
    GM_VEC3_SUBST(tmp_v3, (line_point1));                                    \
    int32_t v3_len = GM_VEC3_LENGTH_SQUARED(tmp_v3);                         \
    if (v3_len == 0){                                                        \
        err = 1;                                                             \
    } else {                                                                 \
        union gm_ivec3 tmp_v1 = (point);                                     \
        union gm_ivec3 tmp_v2 = (point);                                     \
        GM_VEC3_SUBST(tmp_v1, (line_point1));                                \
        GM_VEC3_SUBST(tmp_v2, (line_point2));                                \
        GM_VEC3_CROSS(tmp_v1,tmp_v2);                                        \
        ret = GM_VEC3_LENGTH_SQUARED(tmp_v1) / v3_len;                       \
    }                                                                        \
}

#define GM_VEC2_NORMALIZE(v){        \
    double len = GM_VEC2_LENGTH(v);  \
    (v).x /= len;                    \
    (v).y /= len;                    \
}

#define GM_VEC3_NORMALIZE(v){        \
    double len = GM_VEC3_LENGTH(v);  \
    (v).x /= len;                    \
    (v).y /= len;                    \
    (v).z /= len;                    \
}

#define GM_MAT4_ORTHO(result, l, r, b, t, n, f){    \
    result.data[0]  = 2.0f / (r - l);               \
    result.data[1]  = 0.0f;                         \
    result.data[2]  = 0.0f;                         \
    result.data[3]  = 0.0f;                         \
    result.data[4]  = 0.0f;                         \
    result.data[5]  = 2.0f / (t - b);               \
    result.data[6]  = 0.0f;                         \
    result.data[7]  = 0.0f;                         \
    result.data[8]  = 0.0f;                         \
    result.data[9]  = 0.0f;                         \
    result.data[10] = -2.0f / (f - n);              \
    result.data[11] = 0.0f;                         \
    result.data[12] = -((r + l) / (r - l));         \
    result.data[13] = -((t + b) / (t - b));         \
    result.data[14] = -((f + n) / (f - n));         \
    result.data[15] = 1.0f;                         \
}

#define GM_FMAT4_LOOK_AT(result, position, target, up){             \
    union gm_fvec3 tmp_forward = target;                            \
    GM_VEC3_SUBST(tmp_forward, position);                           \
    GM_VEC3_NORMALIZE(tmp_forward);                                 \
                                                                    \
    union gm_fvec3 tmp_side = tmp_forward;                          \
    GM_VEC3_CROSS(tmp_side, up);                                    \
    GM_VEC3_NORMALIZE(tmp_side);                                    \
                                                                    \
    union gm_fvec3 tmp_up = tmp_side;                               \
    GM_VEC3_CROSS(tmp_up, tmp_forward);                             \
                                                                    \
    result.data[0] = tmp_side.data[0];                              \
    result.data[1] = tmp_up.data[0];                                \
    result.data[2] = -tmp_forward.data[0];                          \
    result.data[3] = 0.0f;                                          \
    result.data[4] = tmp_side.data[1];                              \
    result.data[5] = tmp_up.data[1];                                \
    result.data[6] = -tmp_forward.data[1];                          \
    result.data[7] = 0.0f;                                          \
    result.data[8] = tmp_side.data[2];                              \
    result.data[9] = tmp_up.data[2];                                \
    result.data[10] = -tmp_forward.data[2];                         \
    result.data[11] = 0.0f;                                         \
    result.data[12] = -GM_VEC3_DOT(tmp_side, position);             \
    result.data[13] = -GM_VEC3_DOT(tmp_up, position);               \
    result.data[14] = GM_VEC3_DOT(tmp_forward, position);           \
    result.data[15] = 1.0f;                                         \
}

#define GM_MAT4_INVERSE(result) {                                                                               \
    typeof(result.data[0]) inverse[16];                                                                         \
    typeof(result.data[0]) inverted_determinant;                                                                \
    typeof(result.data[0]) m11 = result.data[0];                                                                \
    typeof(result.data[0]) m21 = result.data[1];                                                                \
    typeof(result.data[0]) m31 = result.data[2];                                                                \
    typeof(result.data[0]) m41 = result.data[3];                                                                \
    typeof(result.data[0]) m12 = result.data[4];                                                                \
    typeof(result.data[0]) m22 = result.data[5];                                                                \
    typeof(result.data[0]) m32 = result.data[6];                                                                \
    typeof(result.data[0]) m42 = result.data[7];                                                                \
    typeof(result.data[0]) m13 = result.data[8];                                                                \
    typeof(result.data[0]) m23 = result.data[9];                                                                \
    typeof(result.data[0]) m33 = result.data[10];                                                               \
    typeof(result.data[0]) m43 = result.data[11];                                                               \
    typeof(result.data[0]) m14 = result.data[12];                                                               \
    typeof(result.data[0]) m24 = result.data[13];                                                               \
    typeof(result.data[0]) m34 = result.data[14];                                                               \
    typeof(result.data[0]) m44 = result.data[15];                                                               \
    inverse[0] = m22 * m33 * m44                                                                                \
        - m22 * m43 * m34                                                                                       \
        - m23 * m32 * m44                                                                                       \
        + m23 * m42 * m34                                                                                       \
        + m24 * m32 * m43                                                                                       \
        - m24 * m42 * m33;                                                                                      \
    inverse[4] = -m12 * m33 * m44                                                                               \
        + m12 * m43 * m34                                                                                       \
        + m13 * m32 * m44                                                                                       \
        - m13 * m42 * m34                                                                                       \
        - m14 * m32 * m43                                                                                       \
        + m14 * m42 * m33;                                                                                      \
    inverse[8] = m12 * m23 * m44                                                                                \
        - m12 * m43 * m24                                                                                       \
        - m13 * m22 * m44                                                                                       \
        + m13 * m42 * m24                                                                                       \
        + m14 * m22 * m43                                                                                       \
        - m14 * m42 * m23;                                                                                      \
    inverse[12] = -m12 * m23 * m34                                                                              \
        + m12 * m33 * m24                                                                                       \
        + m13 * m22 * m34                                                                                       \
        - m13 * m32 * m24                                                                                       \
        - m14 * m22 * m33                                                                                       \
        + m14 * m32 * m23;                                                                                      \
    inverse[1] = -m21 * m33 * m44                                                                               \
        + m21 * m43 * m34                                                                                       \
        + m23 * m31 * m44                                                                                       \
        - m23 * m41 * m34                                                                                       \
        - m24 * m31 * m43                                                                                       \
        + m24 * m41 * m33;                                                                                      \
    inverse[5] =m11 * m33 * m44                                                                                 \
        -m11 * m43 * m34                                                                                        \
        - m13 * m31 * m44                                                                                       \
        + m13 * m41 * m34                                                                                       \
        + m14 * m31 * m43                                                                                       \
        - m14 * m41 * m33;                                                                                      \
    inverse[9] = -m11 * m23 * m44                                                                               \
        +m11 * m43 * m24                                                                                        \
        + m13 * m21 * m44                                                                                       \
        - m13 * m41 * m24                                                                                       \
        - m14 * m21 * m43                                                                                       \
        + m14 * m41 * m23;                                                                                      \
    inverse[13] =m11 * m23 * m34                                                                                \
        -m11 * m33 * m24                                                                                        \
        - m13 * m21 * m34                                                                                       \
        + m13 * m31 * m24                                                                                       \
        + m14 * m21 * m33                                                                                       \
        - m14 * m31 * m23;                                                                                      \
    inverse[2] = m21 * m32 * m44                                                                                \
        - m21 * m42 * m34                                                                                       \
        - m22 * m31 * m44                                                                                       \
        + m22 * m41 * m34                                                                                       \
        + m24 * m31 * m42                                                                                       \
        - m24 * m41 * m32;                                                                                      \
    inverse[6] = -m11 * m32 * m44                                                                               \
        +m11 * m42 * m34                                                                                        \
        + m12 * m31 * m44                                                                                       \
        - m12 * m41 * m34                                                                                       \
        - m14 * m31 * m42                                                                                       \
        + m14 * m41 * m32;                                                                                      \
    inverse[10] =m11 * m22 * m44                                                                                \
        -m11 * m42 * m24                                                                                        \
        - m12 * m21 * m44                                                                                       \
        + m12 * m41 * m24                                                                                       \
        + m14 * m21 * m42                                                                                       \
        - m14 * m41 * m22;                                                                                      \
    inverse[14] = -m11 * m22 * m34                                                                              \
        +m11 * m32 * m24                                                                                        \
        + m12 * m21 * m34                                                                                       \
        - m12 * m31 * m24                                                                                       \
        - m14 * m21 * m32                                                                                       \
        + m14 * m31 * m22;                                                                                      \
    inverse[3] = -m21 * m32 * m43                                                                               \
        + m21 * m42 * m33                                                                                       \
        + m22 * m31 * m43                                                                                       \
        - m22 * m41 * m33                                                                                       \
        - m23 * m31 * m42                                                                                       \
        + m23 * m41 * m32;                                                                                      \
    inverse[7] =m11 * m32 * m43                                                                                 \
        -m11 * m42 * m33                                                                                        \
        - m12 * m31 * m43                                                                                       \
        + m12 * m41 * m33                                                                                       \
        + m13 * m31 * m42                                                                                       \
        - m13 * m41 * m32;                                                                                      \
    inverse[11] = -m11 * m22 * m43                                                                              \
        +m11 * m42 * m23                                                                                        \
        + m12 * m21 * m43                                                                                       \
        - m12 * m41 * m23                                                                                       \
        - m13 * m21 * m42                                                                                       \
        + m13 * m41 * m22;                                                                                      \
    inverse[15] =m11 * m22 * m33                                                                                \
        -m11 * m32 * m23                                                                                        \
        - m12 * m21 * m33                                                                                       \
        + m12 * m31 * m23                                                                                       \
        + m13 * m21 * m32                                                                                       \
        - m13 * m31 * m22;                                                                                      \
    inverted_determinant = 1.0f / (m11 * inverse[0] + m21 * inverse[4] + m31 * inverse[8] + m41 * inverse[12]); \
    result.data[0] = inverse[0] * inverted_determinant;                                                         \
    result.data[1] = inverse[1] * inverted_determinant;                                                         \
    result.data[2] = inverse[2] * inverted_determinant;                                                         \
    result.data[3] = inverse[3] * inverted_determinant;                                                         \
    result.data[4] = inverse[4] * inverted_determinant;                                                         \
    result.data[5] = inverse[5] * inverted_determinant;                                                         \
    result.data[6] = inverse[6] * inverted_determinant;                                                         \
    result.data[7] = inverse[7] * inverted_determinant;                                                         \
    result.data[8] = inverse[8] * inverted_determinant;                                                         \
    result.data[9] = inverse[9] * inverted_determinant;                                                         \
    result.data[10] = inverse[10] * inverted_determinant;                                                       \
    result.data[11] = inverse[11] * inverted_determinant;                                                       \
    result.data[12] = inverse[12] * inverted_determinant;                                                       \
    result.data[13] = inverse[13] * inverted_determinant;                                                       \
    result.data[14] = inverse[14] * inverted_determinant;                                                       \
    result.data[15] = inverse[15] * inverted_determinant;                                                       \
}

#define GM_MAT4_MULT(result, m1){                                                                                                             \
    typeof(result.data[0]) multiplied[16];                                                                                                                         \
    multiplied[0] = result.data[0] * m1.data[0] + result.data[4] * m1.data[1] + result.data[8] * m1.data[2] + result.data[12] * m1.data[3];         \
    multiplied[1] = result.data[1] * m1.data[0] + result.data[5] * m1.data[1] + result.data[9] * m1.data[2] + result.data[13] * m1.data[3];         \
    multiplied[2] = result.data[2] * m1.data[0] + result.data[6] * m1.data[1] + result.data[10] * m1.data[2] + result.data[14] * m1.data[3];        \
    multiplied[3] = result.data[3] * m1.data[0] + result.data[7] * m1.data[1] + result.data[11] * m1.data[2] + result.data[15] * m1.data[3];        \
    multiplied[4] = result.data[0] * m1.data[4] + result.data[4] * m1.data[5] + result.data[8] * m1.data[6] + result.data[12] * m1.data[7];         \
    multiplied[5] = result.data[1] * m1.data[4] + result.data[5] * m1.data[5] + result.data[9] * m1.data[6] + result.data[13] * m1.data[7];         \
    multiplied[6] = result.data[2] * m1.data[4] + result.data[6] * m1.data[5] + result.data[10] * m1.data[6] + result.data[14] * m1.data[7];        \
    multiplied[7] = result.data[3] * m1.data[4] + result.data[7] * m1.data[5] + result.data[11] * m1.data[6] + result.data[15] * m1.data[7];        \
    multiplied[8] = result.data[0] * m1.data[8] + result.data[4] * m1.data[9] + result.data[8] * m1.data[10] + result.data[12] * m1.data[11];       \
    multiplied[9] = result.data[1] * m1.data[8] + result.data[5] * m1.data[9] + result.data[9] * m1.data[10] + result.data[13] * m1.data[11];       \
    multiplied[10] = result.data[2] * m1.data[8] + result.data[6] * m1.data[9] + result.data[10] * m1.data[10] + result.data[14] * m1.data[11];     \
    multiplied[11] = result.data[3] * m1.data[8] + result.data[7] * m1.data[9] + result.data[11] * m1.data[10] + result.data[15] * m1.data[11];     \
    multiplied[12] = result.data[0] * m1.data[12] + result.data[4] * m1.data[13] + result.data[8] * m1.data[14] + result.data[12] * m1.data[15];    \
    multiplied[13] = result.data[1] * m1.data[12] + result.data[5] * m1.data[13] + result.data[9] * m1.data[14] + result.data[13] * m1.data[15];    \
    multiplied[14] = result.data[2] * m1.data[12] + result.data[6] * m1.data[13] + result.data[10] * m1.data[14] + result.data[14] * m1.data[15];   \
    multiplied[15] = result.data[3] * m1.data[12] + result.data[7] * m1.data[13] + result.data[11] * m1.data[14] + result.data[15] * m1.data[15];   \
    result.data[0] = multiplied[0];                                                                                                                 \
    result.data[1] = multiplied[1];                                                                                                                 \
    result.data[2] = multiplied[2];                                                                                                                 \
    result.data[3] = multiplied[3];                                                                                                                 \
    result.data[4] = multiplied[4];                                                                                                                 \
    result.data[5] = multiplied[5];                                                                                                                 \
    result.data[6] = multiplied[6];                                                                                                                 \
    result.data[7] = multiplied[7];                                                                                                                 \
    result.data[8] = multiplied[8];                                                                                                                 \
    result.data[9] = multiplied[9];                                                                                                                 \
    result.data[10] = multiplied[10];                                                                                                               \
    result.data[11] = multiplied[11];                                                                                                               \
    result.data[12] = multiplied[12];                                                                                                               \
    result.data[13] = multiplied[13];                                                                                                               \
    result.data[14] = multiplied[14];                                                                                                               \
    result.data[15] = multiplied[15];                                                                                                               \
}

#define GM_TRIANGLE3_DIST(ret, tri, point){                        \
    typeof(point) tmp_v = (point);                                  \
    GM_VEC3_SUBST(tmp_v,  (tri).p[0]);                              \
    union gm_dvec3 tmp_vf = {.x=tmp_v.x, .y=tmp_v.y, .z=tmp_v.z};   \
    (ret) = GM_VEC3_DOT((tri).normal, tmp_vf);                      \
}

#define GM_TRIANGLE3_CALC_NORM(tri){        \
    typeof((tri).p[0]) tmp_v1 = (tri).p[0]; \
    typeof((tri).p[0]) tmp_v2 = (tri).p[1]; \
    GM_VEC3_SUBST(tmp_v1, (tri).p[1]);      \
    GM_VEC3_SUBST(tmp_v2, (tri).p[2]);      \
    GM_VEC3_CROSS(tmp_v1, tmp_v2);          \
    (tri).normal = (union gm_dvec3){        \
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

void gm_update_iconvex_hull3(struct gm_iconvex_hull3* ch, const cvector_vector_type(union gm_ivec3) points, const union gm_iextremes3* extremes);
bool gm_is_ivec3_inside_convex_hull(const struct gm_iconvex_hull3* ch, const union gm_ivec3* point);

#endif