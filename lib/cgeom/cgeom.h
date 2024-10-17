#ifndef CGEOM_H
#define CGEOM_H

#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <float.h>

#define VEC2_SIZE 2
#define VEC3_SIZE 3
#define VEC4_SIZE 4
#define QUAT_SIZE 4
#define MAT2_SIZE 4
#define MAT3_SIZE 9
#define MAT4_SIZE 16

#define M_X 0
#define M_Y 1
#define M_Z 2
#define M_W 3

#define M_R 0
#define M_G 1
#define M_B 2
#define M_A 3

#define M_LEFT     0
#define M_TOP      1
#define M_WIDTH    2
#define M_HEIGHT   3

#if !defined(CGEOM_INT_TYPE)
typedef int32_t mint_t;
#else
typedef CGEOM_INT_TYPE mint_t;
#endif

#if !defined(CGEOM_FLOAT_TYPE)
typedef float mfloat_t;
#else
typedef CGEOM_FLOAT_TYPE mfloat_t;
#endif

#if !defined(CGEOM_DOUBLE_TYPE)
typedef double mdouble_t;
#else
typedef CGEOM_DOUBLE_TYPE mdouble_t;
#endif

#define MdPI    (mdouble_t)3.14159265358979323846
#define MdPI_2  (mdouble_t)1.57079632679489661923
#define MdPI_4  (mdouble_t)0.78539816339744830962
#define MfPI    (mfloat_t)3.1415926536
#define MfPI_2  (mfloat_t)1.5707963268
#define MfPI_4  (mfloat_t)0.7853981634

#define MfFABS  fabsf
#define MfFMIN  fminf
#define MfFMAX  fmaxf
#define MfSQRT  sqrtf
#define MfSIN   sinf
#define MfCOS   cosf
#define MfACOS  acosf
#define MfASIN  asinf
#define MfTAN   tanf
#define MfATAN2 atan2f
#define MfPOW   powf
#define MfFLOOR floorf
#define MfCEIL  ceilf
#define MfROUND roundf

#define MdFABS  fabs
#define MdFMIN  fmin
#define MdFMAX  fmax
#define MdSQRT  sqrt
#define MdSIN   sin
#define MdCOS   cos
#define MdACOS  acos
#define MdASIN  asin
#define MdTAN   tan
#define MdATAN2 atan2
#define MdPOW   pow
#define MdFLOOR floor
#define MdCEIL  ceil
#define MdROUND round

#define MfEPSILON FLT_EPSILON
#define MdEPSILON DBL_EPSILON

// VEC2

#define M_GEN_VEC2(_t,_char)      \
struct vec2##_char {        \
    union {                 \
        struct {            \
            _t x;           \
            _t y;           \
        };                  \
        struct {            \
            _t s;           \
            _t t;           \
        };                  \
        struct {            \
            _t start;       \
            _t end;         \
        };                  \
        _t v[VEC2_SIZE];    \
    };                      \
}

M_GEN_VEC2(mint_t,i);
M_GEN_VEC2(mfloat_t,f);
M_GEN_VEC2(mdouble_t,d);

// END VEC2

// VEC3

#define M_GEN_VEC3(_t,_char)      \
struct vec3##_char {        \
    union {                 \
        struct {            \
            _t x;           \
            _t y;           \
            _t z;           \
        };                  \
        struct {            \
            _t r;           \
            _t g;           \
            _t b;           \
        };                  \
        struct {            \
            _t red;         \
            _t green;       \
            _t blue;        \
        };                  \
        _t v[VEC3_SIZE];    \
    };                      \
}

M_GEN_VEC3(mint_t,i);
M_GEN_VEC3(mfloat_t,f);
M_GEN_VEC3(mdouble_t,d);

// END VEC3

// VEC4

#define M_GEN_VEC4(_t,_char)      \
struct vec4##_char {        \
    union {                 \
        struct {            \
            _t x;           \
            _t y;           \
            _t z;           \
            _t w;           \
        };                  \
        struct {            \
            _t left;        \
            _t top;         \
            _t width;       \
            _t height;      \
        };                  \
        struct {            \
            _t r;           \
            _t g;           \
            _t b;           \
            _t a;           \
        };                  \
        struct {            \
            _t red;         \
            _t green;       \
            _t blue;        \
            _t alpha;       \
        };                  \
        struct {            \
            _t vstart;      \
            _t vcount;      \
            _t istart;      \
            _t icount;      \
        };                  \
        _t v[VEC4_SIZE];    \
    };                      \
}

M_GEN_VEC4(mint_t,i);
M_GEN_VEC4(mfloat_t,f);
M_GEN_VEC4(mdouble_t,d);

// END VEC4

// QUAT

#define M_GEN_QUAT(_t,_char)      \
struct quat##_char {        \
    union {                 \
        struct {            \
            _t x;           \
            _t y;           \
            _t z;           \
            _t w;           \
        };                  \
        _t v[QUAT_SIZE];    \
    };                      \
}

M_GEN_QUAT(mint_t,i);
M_GEN_QUAT(mfloat_t,f);
M_GEN_QUAT(mdouble_t,d);

// END QUAT

// MAT2

/*
Matrix 2×2 representation:
0/m11 2/m12
1/m21 3/m22
*/
#define M_GEN_MAT2(_t,_char)      \
struct mat2##_char {        \
    union {                 \
        struct {            \
            _t m11;         \
            _t m21;         \
            _t m12;         \
            _t m22;         \
        };                  \
        _t v[MAT2_SIZE];    \
    };                      \
}

M_GEN_MAT2(mfloat_t,f);
M_GEN_MAT2(mdouble_t,d);

// END MAT2

// MAT 3

/*
Matrix 3×3 representation:
0/m11 3/m12 6/m13
1/m21 4/m22 7/m23
2/m31 5/m32 8/m33
*/
#define M_GEN_MAT3(_t,_char)      \
struct mat3##_char {        \
    union {                 \
        struct {            \
            _t m11;         \
            _t m21;         \
            _t m31;         \
            _t m12;         \
            _t m22;         \
            _t m32;         \
            _t m13;         \
            _t m23;         \
            _t m33;         \
        };                  \
        _t v[MAT3_SIZE];    \
    };                      \
}

M_GEN_MAT3(mfloat_t,f);
M_GEN_MAT3(mdouble_t,d);

// END MAT3

// MAT 4

/*
Matrix 4×4 representation:
0/m11 4/m12  8/m13 12/m14
1/m21 5/m22  9/m23 13/m24
2/m31 6/m32 10/m33 14/m34
3/m41 7/m42 11/m43 15/m44
*/
#define M_GEN_MAT4(_t,_char)      \
struct mat4##_char {        \
    union {                 \
        struct {            \
            _t m11;         \
            _t m21;         \
            _t m31;         \
            _t m41;         \
            _t m12;         \
            _t m22;         \
            _t m32;         \
            _t m42;         \
            _t m13;         \
            _t m23;         \
            _t m33;         \
            _t m43;         \
            _t m14;         \
            _t m24;         \
            _t m34;         \
            _t m44;         \
        };                  \
        _t v[MAT4_SIZE];    \
    };                      \
}

M_GEN_MAT4(mfloat_t,f);
M_GEN_MAT4(mdouble_t,d);

// END MAT4

// Rotating vec3i
enum mcorner_vector {
    mcvBackDownLeft     =0,
    mcvFrontDownLeft    =1,
    mcvFrontDownRight   =2,
    mcvBackDownRight    =3,
    mcvBackUpLeft       =4,
    mcvBackUpRight      =7,
    mcvFrontUpLeft      =8,
    mcvFrontUpRight     =11,

    mcvFrontUpRight2    =14,
    mcvFrontUpRight3    =18,
    mcvFrontUpLeft2     =13,
    mcvFrontUpLeft3     =23,
    mcvFrontDownRight2  =15,
    mcvFrontDownRight3  =17,
    mcvFrontDownLeft2   =12,
    mcvFrontDownLeft3   =20,
    mcvBackUpRight2     =10,
    mcvBackUpRight3     =19,
    mcvBackUpLeft2      =9,
    mcvBackUpLeft3      =22,
    mcvBackDownRight2   =6,
    mcvBackDownRight3   =16,
    mcvBackDownLeft2    =5,
    mcvBackDownLeft3    =21,
};

mint_t *vec3i_rotate_ortho(mint_t *result, const mint_t *v, enum mcorner_vector cv);


#define M_GEN_nearly_equal(_t,_char) bool nearly_equal##_char(_t a,_t b, _t epsilon)
M_GEN_nearly_equal(mfloat_t,f);
M_GEN_nearly_equal(mdouble_t,d);

#define M_GEN_clamp(_t,_char) _t clamp##_char(_t value, _t min, _t max)
M_GEN_clamp(mint_t,i);
M_GEN_clamp(mfloat_t,f);
M_GEN_clamp(mdouble_t,d);

#define M_GEN_to_radians(_t,_char) _t to_radians##_char(_t degrees)
M_GEN_to_radians(mfloat_t,f);
M_GEN_to_radians(mdouble_t,d);

#define M_GEN_to_degrees(_t,_char) _t to_degrees##_char(_t radians)
M_GEN_to_degrees(mfloat_t,f);
M_GEN_to_degrees(mdouble_t,d);


#define M_GEN_is_zero(_t,_char) bool _t##_char##_is_zero(const struct _t##_char* v0)
M_GEN_is_zero(vec2,i);
M_GEN_is_zero(vec2,f);
M_GEN_is_zero(vec2,d);
M_GEN_is_zero(vec3,i);
M_GEN_is_zero(vec3,f);
M_GEN_is_zero(vec3,d);
M_GEN_is_zero(vec4,i);
M_GEN_is_zero(vec4,f);
M_GEN_is_zero(vec4,d);
M_GEN_is_zero(quat,i);
M_GEN_is_zero(quat,f);
M_GEN_is_zero(quat,d);

#define M_GEN_is_equal(_t,_char) bool _t##_char##_is_equal(const struct _t##_char* v0, const struct _t##_char* v1)
M_GEN_is_equal(vec2,i);
M_GEN_is_equal(vec2,f);
M_GEN_is_equal(vec2,d);
M_GEN_is_equal(vec3,i);
M_GEN_is_equal(vec3,f);
M_GEN_is_equal(vec3,d);
M_GEN_is_equal(vec4,i);
M_GEN_is_equal(vec4,f);
M_GEN_is_equal(vec4,d);
M_GEN_is_equal(quat,i);
M_GEN_is_equal(quat,f);
M_GEN_is_equal(quat,d);

#define M_GEN_one(_t,_char) struct _t##_char##* _t##_char##_one(struct _t##_char* result)
M_GEN_one(vec2,i);
M_GEN_one(vec2,f);
M_GEN_one(vec2,d);
M_GEN_one(vec3,i);
M_GEN_one(vec3,f);
M_GEN_one(vec3,d);
M_GEN_one(vec4,i);
M_GEN_one(vec4,f);
M_GEN_one(vec4,d);

#define M_GEN_sign(_t,_char) struct _t##_char##* _t##_char##_sign( struct _t##_char* result, const struct _t##_char* v0)
M_GEN_sign(vec2,i);
M_GEN_sign(vec2,f);
M_GEN_sign(vec2,d);
M_GEN_sign(vec3,i);
M_GEN_sign(vec3,f);
M_GEN_sign(vec3,d);
M_GEN_sign(vec4,i);
M_GEN_sign(vec4,f);
M_GEN_sign(vec4,d);


struct vec2i *psvec2i_add(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1);
struct vec2i *psvec2i_add_i(struct vec2i *result, const struct vec2i *v0, mint_t i);
struct vec2i *psvec2i_subtract(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1);
struct vec2i *psvec2i_subtract_i(struct vec2i *result, const struct vec2i *v0, mint_t i);
struct vec2i *psvec2i_multiply(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1);
struct vec2i *psvec2i_multiply_i(struct vec2i *result, const struct vec2i *v0, mint_t i);
struct vec2i *psvec2i_divide(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1);
struct vec2i *psvec2i_divide_i(struct vec2i *result, const struct vec2i *v0, mint_t i);
struct vec2i *psvec2i_snap(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1);
struct vec2i *psvec2i_snap_i(struct vec2i *result, const struct vec2i *v0, mint_t i);
struct vec2i *psvec2i_negative(struct vec2i *result, const struct vec2i *v0);
struct vec2i *psvec2i_abs(struct vec2i *result, const struct vec2i *v0);
struct vec2i *psvec2i_max(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1);
struct vec2i *psvec2i_min(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1);
struct vec2i *psvec2i_clamp(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1, struct vec2i *v2);
struct vec2i *psvec2i_tangent(struct vec2i *result, const struct vec2i *v0);


struct vec3i *psvec3i_sign(struct vec3i *result, const struct vec3i *v0);
struct vec3i *psvec3i_add(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1);
struct vec3i *psvec3i_add_i(struct vec3i *result, const struct vec3i *v0, mint_t i);
struct vec3i *psvec3i_subtract(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1);
struct vec3i *psvec3i_subtract_i(struct vec3i *result, const struct vec3i *v0, mint_t i);
struct vec3i *psvec3i_multiply(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1);
struct vec3i *psvec3i_multiply_i(struct vec3i *result, const struct vec3i *v0, mint_t i);
struct vec3i *psvec3i_divide(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1);
struct vec3i *psvec3i_divide_i(struct vec3i *result, const struct vec3i *v0, mint_t i);
struct vec3i *psvec3i_snap(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1);
struct vec3i *psvec3i_snap_i(struct vec3i *result, const struct vec3i *v0, mint_t i);
struct vec3i *psvec3i_cross(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1);
struct vec3i *psvec3i_negative(struct vec3i *result, const struct vec3i *v0);
struct vec3i *psvec3i_abs(struct vec3i *result, const struct vec3i *v0);
struct vec3i *psvec3i_max(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1);
struct vec3i *psvec3i_min(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1);
struct vec3i *psvec3i_clamp(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1, const struct vec3i *v2);

struct vec4i *psvec4i_one(struct vec4i *result);
struct vec4i *psvec4i_sign(struct vec4i *result, struct vec4i *v0);
struct vec4i *psvec4i_add(struct vec4i *result, struct vec4i *v0, struct vec4i *v1);
struct vec4i *psvec4i_add_i(struct vec4i *result, struct vec4i *v0, mint_t i);
struct vec4i *psvec4i_subtract(struct vec4i *result, struct vec4i *v0, struct vec4i *v1);
struct vec4i *psvec4i_subtract_i(struct vec4i *result, struct vec4i *v0, mint_t i);
struct vec4i *psvec4i_multiply(struct vec4i *result, struct vec4i *v0, struct vec4i *v1);
struct vec4i *psvec4i_multiply_i(struct vec4i *result, struct vec4i *v0, mint_t i);
struct vec4i *psvec4i_divide(struct vec4i *result, struct vec4i *v0, struct vec4i *v1);
struct vec4i *psvec4i_divide_i(struct vec4i *result, struct vec4i *v0, mint_t i);
struct vec4i *psvec4i_snap(struct vec4i *result, struct vec4i *v0, struct vec4i *v1);
struct vec4i *psvec4i_snap_i(struct vec4i *result, struct vec4i *v0, mint_t i);
struct vec4i *psvec4i_negative(struct vec4i *result, struct vec4i *v0);
struct vec4i *psvec4i_abs(struct vec4i *result, struct vec4i *v0);
struct vec4i *psvec4i_max(struct vec4i *result, struct vec4i *v0, struct vec4i *v1);
struct vec4i *psvec4i_min(struct vec4i *result, struct vec4i *v0, struct vec4i *v1);
struct vec4i *psvec4i_clamp(struct vec4i *result, struct vec4i *v0, struct vec4i *v1, struct vec4i *v2);



struct vec2 *psvec2_one(struct vec2 *result);
struct vec2 *psvec2_sign(struct vec2 *result, struct vec2 *v0);
struct vec2 *psvec2_add(struct vec2 *result, struct vec2 *v0, struct vec2 *v1);
struct vec2 *psvec2_add_f(struct vec2 *result, struct vec2 *v0, mreal_t f);
struct vec2 *psvec2_subtract(struct vec2 *result, struct vec2 *v0, struct vec2 *v1);
struct vec2 *psvec2_subtract_f(struct vec2 *result, struct vec2 *v0, mreal_t f);
struct vec2 *psvec2_multiply(struct vec2 *result, struct vec2 *v0, struct vec2 *v1);
struct vec2 *psvec2_multiply_f(struct vec2 *result, struct vec2 *v0, mreal_t f);
struct vec2 *psvec2_multiply_mat2(struct vec2 *result, struct vec2 *v0, const struct mat2 *m0);
struct vec2 *psvec2_divide(struct vec2 *result, struct vec2 *v0, struct vec2 *v1);
struct vec2 *psvec2_divide_f(struct vec2 *result, struct vec2 *v0, mreal_t f);
struct vec2 *psvec2_snap(struct vec2 *result, struct vec2 *v0, struct vec2 *v1);
struct vec2 *psvec2_snap_f(struct vec2 *result, struct vec2 *v0, mreal_t f);
struct vec2 *psvec2_negative(struct vec2 *result, struct vec2 *v0);
struct vec2 *psvec2_abs(struct vec2 *result, struct vec2 *v0);
struct vec2 *psvec2_floor(struct vec2 *result, struct vec2 *v0);
struct vec2 *psvec2_ceil(struct vec2 *result, struct vec2 *v0);
struct vec2 *psvec2_round(struct vec2 *result, struct vec2 *v0);
struct vec2 *psvec2_max(struct vec2 *result, struct vec2 *v0, struct vec2 *v1);
struct vec2 *psvec2_min(struct vec2 *result, struct vec2 *v0, struct vec2 *v1);
struct vec2 *psvec2_clamp(struct vec2 *result, struct vec2 *v0, struct vec2 *v1, struct vec2 *v2);
struct vec2 *psvec2_normalize(struct vec2 *result, struct vec2 *v0);
mreal_t psvec2_dot(struct vec2 *v0, struct vec2 *v1);
struct vec2 *psvec2_project(struct vec2 *result, struct vec2 *v0, struct vec2 *v1);
struct vec2 *psvec2_slide(struct vec2 *result, struct vec2 *v0, struct vec2 *normal);
struct vec2 *psvec2_reflect(struct vec2 *result, struct vec2 *v0, struct vec2 *normal);
struct vec2 *psvec2_tangent(struct vec2 *result, struct vec2 *v0);
struct vec2 *psvec2_rotate(struct vec2 *result, struct vec2 *v0, mreal_t f);
struct vec2 *psvec2_lerp(struct vec2 *result, struct vec2 *v0, struct vec2 *v1, mreal_t f);
struct vec2 *psvec2_bezier3(struct vec2 *result, struct vec2 *v0, struct vec2 *v1, struct vec2 *v2, mreal_t f);
struct vec2 *psvec2_bezier4(struct vec2 *result, struct vec2 *v0, struct vec2 *v1, struct vec2 *v2, struct vec2 *v3, mreal_t f);
mreal_t psvec2_angle(struct vec2 *v0);
mreal_t psvec2_length(struct vec2 *v0);
mreal_t psvec2_length_squared(struct vec2 *v0);
mreal_t psvec2_distance(struct vec2 *v0, struct vec2 *v1);
mreal_t psvec2_distance_squared(struct vec2 *v0, struct vec2 *v1);

bool psvec3_is_equal(const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_one(struct vec3 *result);
struct vec3 *psvec3_sign(struct vec3 *result, const struct vec3 *v0);
struct vec3 *psvec3_add(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_add_f(struct vec3 *result, const struct vec3 *v0, mreal_t f);
struct vec3 *psvec3_subtract(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_subtract_f(struct vec3 *result, const struct vec3 *v0, mreal_t f);
struct vec3 *psvec3_multiply(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_multiply_f(struct vec3 *result, const struct vec3 *v0, mreal_t f);
struct vec3 *psvec3_multiply_mat3(struct vec3 *result, const struct vec3 *v0, const struct mat3 *m0);
struct vec3 *psvec3_divide(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_divide_f(struct vec3 *result, const struct vec3 *v0, mreal_t f);
struct vec3 *psvec3_snap(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_snap_f(struct vec3 *result, const struct vec3 *v0, mreal_t f);
struct vec3 *psvec3_negative(struct vec3 *result, const struct vec3 *v0);
struct vec3 *psvec3_abs(struct vec3 *result, const struct vec3 *v0);
struct vec3 *psvec3_floor(struct vec3 *result, const struct vec3 *v0);
struct vec3 *psvec3_ceil(struct vec3 *result, const struct vec3 *v0);
struct vec3 *psvec3_round(struct vec3 *result, const struct vec3 *v0);
struct vec3 *psvec3_max(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_min(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_clamp(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1, struct vec3 *v2);
struct vec3 *psvec3_cross(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_normalize(struct vec3 *result, const struct vec3 *v0);
mreal_t psvec3_dot(const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_project(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1);
struct vec3 *psvec3_slide(struct vec3 *result, const struct vec3 *v0, struct vec3 *normal);
struct vec3 *psvec3_reflect(struct vec3 *result, const struct vec3 *v0, struct vec3 *normal);
struct vec3 *psvec3_rotate(struct vec3 *result, const struct vec3 *v0, struct vec3 *ra, mreal_t f);
struct vec3 *psvec3_lerp(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1, mreal_t f);
struct vec3 *psvec3_bezier3(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1, struct vec3 *v2, mreal_t f);
struct vec3 *psvec3_bezier4(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1, struct vec3 *v2, struct vec3 *v3, mreal_t f);
mreal_t psvec3_length(const struct vec3 *v0);
mreal_t psvec3_length_squared(const struct vec3 *v0);
mreal_t psvec3_distance(const struct vec3 *v0, struct vec3 *v1);
mreal_t psvec3_distance_squared(const struct vec3 *v0, struct vec3 *v1);

bool psvec4_is_equal(struct vec4 *v0, struct vec4 *v1);
struct vec4 *psvec4_one(struct vec4 *result);
struct vec4 *psvec4_sign(struct vec4 *result, struct vec4 *v0);
struct vec4 *psvec4_add(struct vec4 *result, struct vec4 *v0, struct vec4 *v1);
struct vec4 *psvec4_add_f(struct vec4 *result, struct vec4 *v0, mreal_t f);
struct vec4 *psvec4_subtract(struct vec4 *result, struct vec4 *v0, struct vec4 *v1);
struct vec4 *psvec4_subtract_f(struct vec4 *result, struct vec4 *v0, mreal_t f);
struct vec4 *psvec4_multiply(struct vec4 *result, struct vec4 *v0, struct vec4 *v1);
struct vec4 *psvec4_multiply_f(struct vec4 *result, struct vec4 *v0, mreal_t f);
struct vec4 *psvec4_multiply_mat4(struct vec4 *result, struct vec4 *v0, const struct mat4 *m0);
struct vec4 *psvec4_divide(struct vec4 *result, struct vec4 *v0, struct vec4 *v1);
struct vec4 *psvec4_divide_f(struct vec4 *result, struct vec4 *v0, mreal_t f);
struct vec4 *psvec4_snap(struct vec4 *result, struct vec4 *v0, struct vec4 *v1);
struct vec4 *psvec4_snap_f(struct vec4 *result, struct vec4 *v0, mreal_t f);
struct vec4 *psvec4_negative(struct vec4 *result, struct vec4 *v0);
struct vec4 *psvec4_abs(struct vec4 *result, struct vec4 *v0);
struct vec4 *psvec4_floor(struct vec4 *result, struct vec4 *v0);
struct vec4 *psvec4_ceil(struct vec4 *result, struct vec4 *v0);
struct vec4 *psvec4_round(struct vec4 *result, struct vec4 *v0);
struct vec4 *psvec4_max(struct vec4 *result, struct vec4 *v0, struct vec4 *v1);
struct vec4 *psvec4_min(struct vec4 *result, struct vec4 *v0, struct vec4 *v1);
struct vec4 *psvec4_clamp(struct vec4 *result, struct vec4 *v0, struct vec4 *v1, struct vec4 *v2);
struct vec4 *psvec4_normalize(struct vec4 *result, struct vec4 *v0);
struct vec4 *psvec4_lerp(struct vec4 *result, struct vec4 *v0, struct vec4 *v1, mreal_t f);

bool psquat_is_equal(const struct quat *q0, const struct quat *q1);
struct quat *psquat_null(struct quat *result);
struct quat *psquat_multiply(struct quat *result, const struct quat *q0, const struct quat *q1);
struct quat *psquat_multiply_f(struct quat *result, const struct quat *q0, mreal_t f);
struct quat *psquat_divide(struct quat *result, const struct quat *q0, const struct quat *q1);
struct quat *psquat_divide_f(struct quat *result, const struct quat *q0, mreal_t f);
struct quat *psquat_negative(struct quat *result, const struct quat *q0);
struct quat *psquat_conjugate(struct quat *result, const struct quat *q0);
struct quat *psquat_inverse(struct quat *result, const struct quat *q0);
struct quat *psquat_normalize(struct quat *result, const struct quat *q0);
mreal_t psquat_dot(const struct quat *q0, const struct quat *q1);
struct quat *psquat_power(struct quat *result, const struct quat *q0, mreal_t exponent);
struct quat *psquat_from_axis_angle(struct quat *result, const struct vec3 *v0, mreal_t angle);
struct quat *psquat_from_vec3(struct quat *result, const struct vec3 *v0, struct vec3 *v1);
struct quat *psquat_from_mat4(struct quat *result, const struct mat4 *m0);
struct quat *psquat_lerp(struct quat *result, const struct quat *q0, const struct quat *q1, mreal_t f);
struct quat *psquat_slerp(struct quat *result, const struct quat *q0, const struct quat *q1, mreal_t f);
mreal_t psquat_length(const struct quat *q0);
mreal_t psquat_length_squared(const struct quat *q0);
mreal_t psquat_angle(const struct quat *q0, const struct quat *q1);

struct mat2 *psmat2_identity(struct mat2 *result);
mreal_t psmat2_determinant(const struct mat2 *m0);
struct mat2 *psmat2_assign(struct mat2 *result, const struct mat2 *m0);
struct mat2 *psmat2_negative(struct mat2 *result, const struct mat2 *m0);
struct mat2 *psmat2_transpose(struct mat2 *result, const struct mat2 *m0);
struct mat2 *psmat2_cofactor(struct mat2 *result, const struct mat2 *m0);
struct mat2 *psmat2_adjugate(struct mat2 *result, const struct mat2 *m0);
struct mat2 *psmat2_multiply(struct mat2 *result, const struct mat2 *m0, const struct mat2 *m1);
struct mat2 *psmat2_multiply_f(struct mat2 *result, const struct mat2 *m0, mreal_t f);
struct mat2 *psmat2_inverse(struct mat2 *result, const struct mat2 *m0);
struct mat2 *psmat2_scaling(struct mat2 *result, struct vec2 *v0);
struct mat2 *psmat2_scale(struct mat2 *result, const struct mat2 *m0, struct vec2 *v0);
struct mat2 *psmat2_rotation_z(struct mat2 *result, mreal_t f);
struct mat2 *psmat2_lerp(struct mat2 *result, const struct mat2 *m0, const struct mat2 *m1, mreal_t f);
struct mat3 *psmat3(struct mat3 *result, mreal_t m11, mreal_t m12, mreal_t m13, mreal_t m21, mreal_t m22, mreal_t m23, mreal_t m31, mreal_t m32, mreal_t m33);
struct mat3 *psmat3_zero(struct mat3 *result);
struct mat3 *psmat3_identity(struct mat3 *result);
mreal_t psmat3_determinant(const struct mat3 *m0);
struct mat3 *psmat3_assign(struct mat3 *result, const struct mat3 *m0);
struct mat3 *psmat3_negative(struct mat3 *result, const struct mat3 *m0);
struct mat3 *psmat3_transpose(struct mat3 *result, const struct mat3 *m0);
struct mat3 *psmat3_cofactor(struct mat3 *result, const struct mat3 *m0);
struct mat3 *psmat3_multiply(struct mat3 *result, const struct mat3 *m0, const struct mat3 *m1);
struct mat3 *psmat3_multiply_f(struct mat3 *result, const struct mat3 *m0, mreal_t f);
struct mat3 *psmat3_inverse(struct mat3 *result, const struct mat3 *m0);
struct mat3 *psmat3_scaling(struct mat3 *result, const struct vec3 *v0);
struct mat3 *psmat3_scale(struct mat3 *result, const struct mat3 *m0, const struct vec3 *v0);
struct mat3 *psmat3_rotation_x(struct mat3 *result, mreal_t f);
struct mat3 *psmat3_rotation_y(struct mat3 *result, mreal_t f);
struct mat3 *psmat3_rotation_z(struct mat3 *result, mreal_t f);
struct mat3 *psmat3_rotation_axis(struct mat3 *result, const struct vec3 *v0, mreal_t f);
struct mat3 *psmat3_rotation_quat(struct mat3 *result, const struct quat *q0);
struct mat3 *psmat3_lerp(struct mat3 *result, const struct mat3 *m0, const struct mat3 *m1, mreal_t f);
struct mat4 *psmat4(struct mat4 *result, mreal_t m11, mreal_t m12, mreal_t m13, mreal_t m14, mreal_t m21, mreal_t m22, mreal_t m23, mreal_t m24, mreal_t m31, mreal_t m32, mreal_t m33, mreal_t m34, mreal_t m41, mreal_t m42, mreal_t m43, mreal_t m44);
struct mat4 *psmat4_zero(struct mat4 *result);
struct mat4 *psmat4_identity(struct mat4 *result);
mreal_t psmat4_determinant(const struct mat4 *m0);
struct mat4 *psmat4_assign(struct mat4 *result, const struct mat4 *m0);
struct mat4 *psmat4_negative(struct mat4 *result, const struct mat4 *m0);
struct mat4 *psmat4_transpose(struct mat4 *result, const struct mat4 *m0);
struct mat4 *psmat4_cofactor(struct mat4 *result, const struct mat4 *m0);
struct mat4 *psmat4_rotation_x(struct mat4 *result, mreal_t f);
struct mat4 *psmat4_rotation_y(struct mat4 *result, mreal_t f);
struct mat4 *psmat4_rotation_z(struct mat4 *result, mreal_t f);
struct mat4 *psmat4_rotation_axis(struct mat4 *result, const struct vec3 *v0, mreal_t f);
struct mat4 *psmat4_rotation_quat(struct mat4 *result, const struct quat *q0);
struct mat4 *psmat4_translation(struct mat4 *result, const struct mat4 *m0, const struct vec3 *v0);
struct mat4 *psmat4_translate(struct mat4 *result, const struct mat4 *m0, const struct vec3 *v0);
struct mat4 *psmat4_scaling(struct mat4 *result, const struct mat4 *m0, const struct vec3 *v0);
struct mat4 *psmat4_scale(struct mat4 *result, const struct mat4 *m0, const struct vec3 *v0);
struct mat4 *psmat4_multiply(struct mat4 *result, const struct mat4 *m0, const struct mat4 *m1);
struct mat4 *psmat4_multiply_f(struct mat4 *result, const struct mat4 *m0, mreal_t f);
struct mat4 *psmat4_inverse(struct mat4 *result, const struct mat4 *m0);
struct mat4 *psmat4_lerp(struct mat4 *result, const struct mat4 *m0, const struct mat4 *m1, mreal_t f);
struct mat4 *psmat4_look_at(struct mat4 *result, const struct vec3 *position, const struct vec3 *target, const struct vec3 *up);
struct mat4 *psmat4_ortho(struct mat4 *result, mreal_t l, mreal_t r, mreal_t b, mreal_t t, mreal_t n, mreal_t f);
struct mat4 *psmat4_perspective(struct mat4 *result, mreal_t fov_y, mreal_t aspect, mreal_t n, mreal_t f);
struct mat4 *psmat4_perspective_fov(struct mat4 *result, mreal_t fov, mreal_t w, mreal_t h, mreal_t n, mreal_t f);
struct mat4 *psmat4_perspective_infinite(struct mat4 *result, mreal_t fov_y, mreal_t aspect, mreal_t n);




mint_t *vec2i_one(mint_t *result);
mint_t *vec2i_sign(mint_t *result, const mint_t *v0);
mint_t *vec2i_add(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec2i_add_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec2i_subtract(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec2i_subtract_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec2i_multiply(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec2i_multiply_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec2i_divide(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec2i_divide_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec2i_snap(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec2i_snap_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec2i_negative(mint_t *result, const mint_t *v0);
mint_t *vec2i_abs(mint_t *result, const mint_t *v0);
mint_t *vec2i_max(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec2i_min(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec2i_clamp(mint_t *result, const mint_t *v0, const mint_t *v1, const mint_t *v2);
mint_t *vec2i_tangent(mint_t *result, const mint_t *v0);


mint_t *vec3i_one(mint_t *result);
mint_t *vec3i_sign(mint_t *result, const mint_t *v0);
mint_t *vec3i_add(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec3i_add_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec3i_subtract(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec3i_subtract_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec3i_multiply(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec3i_multiply_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec3i_divide(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec3i_divide_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec3i_snap(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec3i_snap_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec3i_cross(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec3i_negative(mint_t *result, const mint_t *v0);
mint_t *vec3i_abs(mint_t *result, const mint_t *v0);
mint_t *vec3i_max(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec3i_min(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec3i_clamp(mint_t *result, const mint_t *v0, const mint_t *v1, const mint_t *v2);
bool vec4i_is_equal(const mint_t *v0, const mint_t *v1);
mint_t *vec4i(mint_t *result, mint_t x, mint_t y, mint_t z, mint_t w);
mint_t *vec4i_assign(mint_t *result, const mint_t *v0);
mint_t *vec4i_assign_vec4(mint_t *result, const mreal_t *v0);
mint_t *vec4i_zero(mint_t *result);
mint_t *vec4i_one(mint_t *result);
mint_t *vec4i_sign(mint_t *result, const mint_t *v0);
mint_t *vec4i_add(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec4i_add_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec4i_subtract(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec4i_subtract_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec4i_multiply(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec4i_multiply_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec4i_divide(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec4i_divide_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec4i_snap(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec4i_snap_i(mint_t *result, const mint_t *v0, mint_t i);
mint_t *vec4i_negative(mint_t *result, const mint_t *v0);
mint_t *vec4i_abs(mint_t *result, const mint_t *v0);
mint_t *vec4i_max(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec4i_min(mint_t *result, const mint_t *v0, const mint_t *v1);
mint_t *vec4i_clamp(mint_t *result, const mint_t *v0, const mint_t *v1, const mint_t *v2);


mreal_t *vec2_one(mreal_t *result);
mreal_t *vec2_sign(mreal_t *result, const mreal_t *v0);
mreal_t *vec2_add(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec2_add_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec2_subtract(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec2_subtract_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec2_multiply(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec2_multiply_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec2_multiply_mat2(mreal_t *result, const mreal_t *v0, const mreal_t *m0);
mreal_t *vec2_divide(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec2_divide_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec2_snap(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec2_snap_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec2_negative(mreal_t *result, const mreal_t *v0);
mreal_t *vec2_abs(mreal_t *result, const mreal_t *v0);
mreal_t *vec2_floor(mreal_t *result, const mreal_t *v0);
mreal_t *vec2_ceil(mreal_t *result, const mreal_t *v0);
mreal_t *vec2_round(mreal_t *result, const mreal_t *v0);
mreal_t *vec2_max(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec2_min(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec2_clamp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2);
mreal_t *vec2_normalize(mreal_t *result, const mreal_t *v0);
mreal_t vec2_dot(const mreal_t *v0, const mreal_t *v1);
mreal_t *vec2_project(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec2_slide(mreal_t *result, const mreal_t *v0, const mreal_t *normal);
mreal_t *vec2_reflect(mreal_t *result, const mreal_t *v0, const mreal_t *normal);
mreal_t *vec2_tangent(mreal_t *result, const mreal_t *v0);
mreal_t *vec2_rotate(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec2_lerp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, mreal_t f);
mreal_t *vec2_bezier3(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2, mreal_t f);
mreal_t *vec2_bezier4(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2, const mreal_t *v3, mreal_t f);
mreal_t vec2_angle(const mreal_t *v0);
mreal_t vec2_length(const mreal_t *v0);
mreal_t vec2_length_squared(const mreal_t *v0);
mreal_t vec2_distance(const mreal_t *v0, const mreal_t *v1);
mreal_t vec2_distance_squared(const mreal_t *v0, const mreal_t *v1);
bool vec2_linear_independent(const mreal_t *v0, const mreal_t *v1);
mreal_t** vec2_orthonormalization(mreal_t result[2][2], mreal_t basis[2][2]);

mreal_t *vec3_one(mreal_t *result);
mreal_t *vec3_sign(mreal_t *result, const mreal_t *v0);
mreal_t *vec3_add(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec3_add_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec3_subtract(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec3_subtract_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec3_multiply(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec3_multiply_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec3_multiply_mat3(mreal_t *result, const mreal_t *v0, const mreal_t *m0);
mreal_t *vec3_divide(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec3_divide_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec3_snap(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec3_snap_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec3_negative(mreal_t *result, const mreal_t *v0);
mreal_t *vec3_abs(mreal_t *result, const mreal_t *v0);
mreal_t *vec3_floor(mreal_t *result, const mreal_t *v0);
mreal_t *vec3_ceil(mreal_t *result, const mreal_t *v0);
mreal_t *vec3_round(mreal_t *result, const mreal_t *v0);
mreal_t *vec3_max(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec3_min(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec3_clamp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2);
mreal_t *vec3_cross(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec3_normalize(mreal_t *result, const mreal_t *v0);
mreal_t vec3_dot(const mreal_t *v0, const mreal_t *v1);
mreal_t *vec3_project(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec3_slide(mreal_t *result, const mreal_t *v0, const mreal_t *normal);
mreal_t *vec3_reflect(mreal_t *result, const mreal_t *v0, const mreal_t *normal);
mreal_t *vec3_rotate(mreal_t *result, const mreal_t *v0, const mreal_t *ra, mreal_t f);
mreal_t *vec3_lerp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, mreal_t f);
mreal_t *vec3_bezier3(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2, mreal_t f);
mreal_t *vec3_bezier4(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2, const mreal_t *v3, mreal_t f);
mreal_t vec3_length(const mreal_t *v0);
mreal_t vec3_length_squared(const mreal_t *v0);
mreal_t vec3_distance(const mreal_t *v0, const mreal_t *v1);
mreal_t vec3_distance_squared(const mreal_t *v0, const mreal_t *v1);
bool vec3_linear_independent(const mreal_t *v0, const mreal_t *v1, const mreal_t *v2);
mreal_t** vec3_orthonormalization(mreal_t result[3][3], mreal_t basis[3][3]);

mreal_t *vec4_one(mreal_t *result);
mreal_t *vec4_sign(mreal_t *result, const mreal_t *v0);
mreal_t *vec4_add(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec4_add_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec4_subtract(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec4_subtract_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec4_multiply(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec4_multiply_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec4_multiply_mat4(mreal_t *result, const mreal_t *v0, const mreal_t *m0);
mreal_t *vec4_divide(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec4_divide_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec4_snap(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec4_snap_f(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *vec4_negative(mreal_t *result, const mreal_t *v0);
mreal_t *vec4_abs(mreal_t *result, const mreal_t *v0);
mreal_t *vec4_floor(mreal_t *result, const mreal_t *v0);
mreal_t *vec4_ceil(mreal_t *result, const mreal_t *v0);
mreal_t *vec4_round(mreal_t *result, const mreal_t *v0);
mreal_t *vec4_max(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec4_min(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *vec4_clamp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2);
mreal_t *vec4_normalize(mreal_t *result, const mreal_t *v0);
mreal_t *vec4_lerp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, mreal_t f);



bool quat_is_equal(const mreal_t *q0, const mreal_t *q1);
mreal_t *quat_null(mreal_t *result);
mreal_t *quat_multiply(mreal_t *result, const mreal_t *q0, const mreal_t *q1);
mreal_t *quat_multiply_f(mreal_t *result, const mreal_t *q0, mreal_t f);
mreal_t *quat_divide(mreal_t *result, const mreal_t *q0, const mreal_t *q1);
mreal_t *quat_divide_f(mreal_t *result, const mreal_t *q0, mreal_t f);
mreal_t *quat_negative(mreal_t *result, const mreal_t *q0);
mreal_t *quat_conjugate(mreal_t *result, const mreal_t *q0);
mreal_t *quat_inverse(mreal_t *result, const mreal_t *q0);
mreal_t *quat_normalize(mreal_t *result, const mreal_t *q0);
mreal_t quat_dot(const mreal_t *q0, const mreal_t *q1);
mreal_t *quat_power(mreal_t *result, const mreal_t *q0, mreal_t exponent);
mreal_t *quat_from_axis_angle(mreal_t *result, const mreal_t *v0, mreal_t angle);
mreal_t *quat_from_vec3(mreal_t *result, const mreal_t *v0, const mreal_t *v1);
mreal_t *quat_from_mat4(mreal_t *result, const mreal_t *m0);
mreal_t *quat_lerp(mreal_t *result, const mreal_t *q0, const mreal_t *q1, mreal_t f);
mreal_t *quat_slerp(mreal_t *result, const mreal_t *q0, const mreal_t *q1, mreal_t f);
mreal_t quat_length(const mreal_t *q0);
mreal_t quat_length_squared(const mreal_t *q0);
mreal_t quat_angle(const mreal_t *q0, const mreal_t *q1);

mreal_t *mat2_identity(mreal_t *result);
mreal_t mat2_determinant(const mreal_t *m0);
mreal_t *mat2_assign(mreal_t *result, const mreal_t *m0);
mreal_t *mat2_negative(mreal_t *result, const mreal_t *m0);
mreal_t *mat2_transpose(mreal_t *result, const mreal_t *m0);
mreal_t *mat2_cofactor(mreal_t *result, const mreal_t *m0);
mreal_t *mat2_adjugate(mreal_t *result, const mreal_t *m0);
mreal_t *mat2_multiply(mreal_t *result, const mreal_t *m0, const mreal_t *m1);
mreal_t *mat2_multiply_f(mreal_t *result, const mreal_t *m0, mreal_t f);
mreal_t *mat2_inverse(mreal_t *result, const mreal_t *m0);
mreal_t *mat2_scaling(mreal_t *result, const mreal_t *v0);
mreal_t *mat2_scale(mreal_t *result, const mreal_t *m0, const mreal_t *v0);
mreal_t *mat2_rotation_z(mreal_t *result, mreal_t f);
mreal_t *mat2_lerp(mreal_t *result, const mreal_t *m0, const mreal_t *m1, mreal_t f);


mreal_t *mat3_identity(mreal_t *result);
mreal_t mat3_determinant(const mreal_t *m0);
mreal_t *mat3_assign(mreal_t *result, const mreal_t *m0);
mreal_t *mat3_negative(mreal_t *result, const mreal_t *m0);
mreal_t *mat3_transpose(mreal_t *result, const mreal_t *m0);
mreal_t *mat3_cofactor(mreal_t *result, const mreal_t *m0);
mreal_t *mat3_multiply(mreal_t *result, const mreal_t *m0, const mreal_t *m1);
mreal_t *mat3_multiply_f(mreal_t *result, const mreal_t *m0, mreal_t f);
mreal_t *mat3_inverse(mreal_t *result, const mreal_t *m0);
mreal_t *mat3_scaling(mreal_t *result, const mreal_t *v0);
mreal_t *mat3_scale(mreal_t *result, const mreal_t *m0, const mreal_t *v0);
mreal_t *mat3_rotation_x(mreal_t *result, mreal_t f);
mreal_t *mat3_rotation_y(mreal_t *result, mreal_t f);
mreal_t *mat3_rotation_z(mreal_t *result, mreal_t f);
mreal_t *mat3_rotation_axis(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *mat3_rotation_quat(mreal_t *result, const mreal_t *q0);
mreal_t *mat3_lerp(mreal_t *result, const mreal_t *m0, const mreal_t *m1, mreal_t f);

mreal_t *mat4_identity(mreal_t *result);
mreal_t mat4_determinant(const mreal_t *m0);
mreal_t *mat4_assign(mreal_t *result, const mreal_t *m0);
mreal_t *mat4_negative(mreal_t *result, const mreal_t *m0);
mreal_t *mat4_transpose(mreal_t *result, const mreal_t *m0);
mreal_t *mat4_cofactor(mreal_t *result, const mreal_t *m0);
mreal_t *mat4_rotation_x(mreal_t *result, mreal_t f);
mreal_t *mat4_rotation_y(mreal_t *result, mreal_t f);
mreal_t *mat4_rotation_z(mreal_t *result, mreal_t f);
mreal_t *mat4_rotation_axis(mreal_t *result, const mreal_t *v0, mreal_t f);
mreal_t *mat4_rotation_quat(mreal_t *result, const mreal_t *q0);
mreal_t *mat4_translation(mreal_t *result, const mreal_t *m0, const mreal_t *v0);
mreal_t *mat4_translate(mreal_t *result, const mreal_t *m0, const mreal_t *v0);
mreal_t *mat4_scaling(mreal_t *result, const mreal_t *m0, const mreal_t *v0);
mreal_t *mat4_scale(mreal_t *result, const mreal_t *m0, const mreal_t *v0);
mreal_t *mat4_multiply(mreal_t *result, const mreal_t *m0, const mreal_t *m1);
mreal_t *mat4_multiply_f(mreal_t *result, const mreal_t *m0, mreal_t f);
mreal_t *mat4_inverse(mreal_t *result, const mreal_t *m0);
mreal_t *mat4_lerp(mreal_t *result, const mreal_t *m0, const mreal_t *m1, mreal_t f);
mreal_t *mat4_look_at(mreal_t *result, const mreal_t *position, const mreal_t *target, const mreal_t *up);
mreal_t *mat4_ortho(mreal_t *result, mreal_t l, mreal_t r, mreal_t b, mreal_t t, mreal_t n, mreal_t f);
mreal_t *mat4_perspective(mreal_t *result, mreal_t fov_y, mreal_t aspect, mreal_t n, mreal_t f);
mreal_t *mat4_perspective_fov(mreal_t *result, mreal_t fov, mreal_t w, mreal_t h, mreal_t n, mreal_t f);
mreal_t *mat4_perspective_infinite(mreal_t *result, mreal_t fov_y, mreal_t aspect, mreal_t n);



mreal_t quadratic_ease_out(mreal_t f);
mreal_t quadratic_ease_in(mreal_t f);
mreal_t quadratic_ease_in_out(mreal_t f);
mreal_t cubic_ease_out(mreal_t f);
mreal_t cubic_ease_in(mreal_t f);
mreal_t cubic_ease_in_out(mreal_t f);
mreal_t quartic_ease_out(mreal_t f);
mreal_t quartic_ease_in(mreal_t f);
mreal_t quartic_ease_in_out(mreal_t f);
mreal_t quintic_ease_out(mreal_t f);
mreal_t quintic_ease_in(mreal_t f);
mreal_t quintic_ease_in_out(mreal_t f);
mreal_t sine_ease_out(mreal_t f);
mreal_t sine_ease_in(mreal_t f);
mreal_t sine_ease_in_out(mreal_t f);
mreal_t circular_ease_out(mreal_t f);
mreal_t circular_ease_in(mreal_t f);
mreal_t circular_ease_in_out(mreal_t f);
mreal_t exponential_ease_out(mreal_t f);
mreal_t exponential_ease_in(mreal_t f);
mreal_t exponential_ease_in_out(mreal_t f);
mreal_t elastic_ease_out(mreal_t f);
mreal_t elastic_ease_in(mreal_t f);
mreal_t elastic_ease_in_out(mreal_t f);
mreal_t back_ease_out(mreal_t f);
mreal_t back_ease_in(mreal_t f);
mreal_t back_ease_in_out(mreal_t f);
mreal_t bounce_ease_out(mreal_t f);
mreal_t bounce_ease_in(mreal_t f);
mreal_t bounce_ease_in_out(mreal_t f);

#endif
