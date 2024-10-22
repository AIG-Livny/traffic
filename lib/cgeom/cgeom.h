#ifndef CGEOM_H
#define CGEOM_H

#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <float.h>

#include "cvector.h"
#include "cvector_utils.h"

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

// Error when one of intermediate value degenerates
#define M_ERROR_CALCULATION 1

#if !defined(CGEOM_INT_TYPE)
typedef int32_t mi_t;
#else
typedef CGEOM_INT_TYPE mint_t;
#endif

#if !defined(CGEOM_FLOAT_TYPE)
typedef float mf_t;
#else
typedef CGEOM_FLOAT_TYPE mf_t;
#endif

#if !defined(CGEOM_DOUBLE_TYPE)
typedef double md_t;
#else
typedef CGEOM_DOUBLE_TYPE md_t;
#endif

#define MdPI    (md_t)3.14159265358979323846
#define MdPI_2  (md_t)1.57079632679489661923
#define MdPI_4  (md_t)0.78539816339744830962
#define MfPI    (mf_t)3.1415926536
#define MfPI_2  (mf_t)1.5707963268
#define MfPI_4  (mf_t)0.7853981634

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

#define M_GEN_VEC2(_char) \
struct vec2##_char { \
    union { \
        struct { \
            m##_char##_t x; \
            m##_char##_t y; \
        }; \
        struct { \
            m##_char##_t s; \
            m##_char##_t t; \
        }; \
        struct { \
            m##_char##_t start; \
            m##_char##_t end; \
        }; \
        struct { \
            m##_char##_t min; \
            m##_char##_t max; \
        }; \
        m##_char##_t v[VEC2_SIZE]; \
    }; \
}

M_GEN_VEC2(i);
M_GEN_VEC2(f);
M_GEN_VEC2(d);

// END VEC2

// VEC3

#define M_GEN_VEC3(_char) \
typedef cvector(struct vec3##_char) cvector_vec3##_char; \
struct vec3##_char { \
    union { \
        struct { \
            m##_char##_t x; \
            m##_char##_t y; \
            m##_char##_t z; \
        }; \
        struct { \
            m##_char##_t r; \
            m##_char##_t g; \
            m##_char##_t b; \
        }; \
        struct { \
            m##_char##_t red; \
            m##_char##_t green; \
            m##_char##_t blue; \
        }; \
        m##_char##_t v[VEC3_SIZE]; \
    }; \
}

M_GEN_VEC3(i);
M_GEN_VEC3(f);
M_GEN_VEC3(d);

// END VEC3

// VEC4

#define M_GEN_VEC4(_char) \
struct vec4##_char { \
    union { \
        struct { \
            m##_char##_t x; \
            m##_char##_t y; \
            m##_char##_t z; \
            m##_char##_t w; \
        }; \
        struct { \
            m##_char##_t left; \
            m##_char##_t top; \
            m##_char##_t width; \
            m##_char##_t height; \
        }; \
        struct { \
            m##_char##_t r; \
            m##_char##_t g; \
            m##_char##_t b; \
            m##_char##_t a; \
        }; \
        struct { \
            m##_char##_t red; \
            m##_char##_t green; \
            m##_char##_t blue; \
            m##_char##_t alpha; \
        }; \
        struct { \
            m##_char##_t vstart; \
            m##_char##_t vcount; \
            m##_char##_t istart; \
            m##_char##_t icount; \
        }; \
        m##_char##_t v[VEC4_SIZE]; \
    }; \
}

M_GEN_VEC4(i);
M_GEN_VEC4(f);
M_GEN_VEC4(d);

// END VEC4

// QUAT

#define M_GEN_QUAT(_char) \
struct quat##_char { \
    union { \
        struct { \
            m##_char##_t x; \
            m##_char##_t y; \
            m##_char##_t z; \
            m##_char##_t w; \
        }; \
        m##_char##_t v[QUAT_SIZE]; \
    }; \
}

M_GEN_QUAT(f);
M_GEN_QUAT(d);

// END QUAT

// MAT2

/*
Matrix 2×2 representation:
0/m11 2/m12
1/m21 3/m22
*/
#define M_GEN_MAT2(_char) \
struct mat2##_char { \
    union { \
        struct { \
            m##_char##_t m11; \
            m##_char##_t m21; \
            m##_char##_t m12; \
            m##_char##_t m22; \
        }; \
        m##_char##_t v[MAT2_SIZE]; \
    }; \
}

M_GEN_MAT2(f);
M_GEN_MAT2(d);

// END MAT2

// MAT 3

/*
Matrix 3×3 representation:
0/m11 3/m12 6/m13
1/m21 4/m22 7/m23
2/m31 5/m32 8/m33
*/
#define M_GEN_MAT3(_char) \
struct mat3##_char { \
    union { \
        struct { \
            m##_char##_t m11; \
            m##_char##_t m21; \
            m##_char##_t m31; \
            m##_char##_t m12; \
            m##_char##_t m22; \
            m##_char##_t m32; \
            m##_char##_t m13; \
            m##_char##_t m23; \
            m##_char##_t m33; \
        }; \
        m##_char##_t v[MAT3_SIZE]; \
    }; \
}

M_GEN_MAT3(f);
M_GEN_MAT3(d);

// END MAT3

// MAT 4

/*
Matrix 4×4 representation:
0/m11 4/m12  8/m13 12/m14
1/m21 5/m22  9/m23 13/m24
2/m31 6/m32 10/m33 14/m34
3/m41 7/m42 11/m43 15/m44
*/
#define M_GEN_MAT4(_char) \
struct mat4##_char { \
    union { \
        struct { \
            m##_char##_t m11; \
            m##_char##_t m21; \
            m##_char##_t m31; \
            m##_char##_t m41; \
            m##_char##_t m12; \
            m##_char##_t m22; \
            m##_char##_t m32; \
            m##_char##_t m42; \
            m##_char##_t m13; \
            m##_char##_t m23; \
            m##_char##_t m33; \
            m##_char##_t m43; \
            m##_char##_t m14; \
            m##_char##_t m24; \
            m##_char##_t m34; \
            m##_char##_t m44; \
        }; \
        m##_char##_t v[MAT4_SIZE]; \
    }; \
}

M_GEN_MAT4(f);
M_GEN_MAT4(d);

// END MAT4

// RECT

#define M_GEN_RECT(_n,_char) \
struct rect##_n##_char { \
    struct vec##_n##_char min; \
    struct vec##_n##_char max; \
}

M_GEN_RECT(3,i);

// END RECT

// LINE

#define M_GEN_LINE(_n,_char) \
struct line##_n##_char { \
    struct vec##_n##_char point; \
    struct vec##_n##d direction; \
}

M_GEN_LINE(3,i);

// END LINE

// SEGMENT

#define M_GEN_SEGMENT(_n,_char) \
struct segment##_n##_char { \
    struct vec##_n##_char start; \
    struct vec##_n##_char end; \
}

M_GEN_SEGMENT(3,i);

// END SEGMENT

// TRIFACE

#define M_GEN_TRIFACE(_n,_char) \
struct triface##_n##_char { \
    struct vec##_n##_char p[3]; \
    struct vec3d normal; \
}

M_GEN_TRIFACE(3,i);

// END TRIFACE

// EXTREMES

#define M_GEN_EXTREMES(_n,_char) \
struct extremes##_n##_char { \
    union { \
        struct{ \
            struct vec3##_char* min_x; \
            struct vec3##_char* min_y; \
            struct vec3##_char* min_z; \
            struct vec3##_char* max_x; \
            struct vec3##_char* max_y; \
            struct vec3##_char* max_z; \
        }; \
        struct vec3##_char* v[6]; \
    }; \
}

M_GEN_EXTREMES(3,i);

// END EXTREMES

// CONVEX_HULL


#define M_GEN_CONVEX_HULL(_n,_char) \
typedef cvector(struct triface##_n##_char) cvector_triangle##_n##_char; \
struct convex_hull##_n##_char { \
    cvector_triangle##_n##_char tris; \
}

M_GEN_CONVEX_HULL(3,i);

// END CONVEX_HULL

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


#define M_GEN_nearly_equal(_char) bool nearly_equal##_char(m##_char##_t a, m##_char##_t b, m##_char##_t epsilon)
#ifdef M_GEN_nearly_equal
M_GEN_nearly_equal(f);
M_GEN_nearly_equal(d);
#endif

#define M_GEN_clamp(_char) m##_char##_t clamp##_char(m##_char##_t value, m##_char##_t min, m##_char##_t max)
#ifdef M_GEN_clamp
M_GEN_clamp(i);
M_GEN_clamp(f);
M_GEN_clamp(d);
#endif

#define M_GEN_to_radians(_char) m##_char##_t to_radians##_char(m##_char##_t degrees)
#ifdef M_GEN_to_radians
M_GEN_to_radians(f);
M_GEN_to_radians(d);
#endif

#define M_GEN_to_degrees(_char) m##_char##_t to_degrees##_char(m##_char##_t radians)
#ifdef M_GEN_to_degrees
M_GEN_to_degrees(f);
M_GEN_to_degrees(d);
#endif

#define VEC2_CAST(_char, vect) &(struct vec2##_char){.x=(vect)->x, .y=(vect)->y}
#define VEC3_CAST(_char, vect) &(struct vec3##_char){.x=(vect)->x, .y=(vect)->y, .z=(vect)->z}

#define M_GEN_is_zero(_s,_char) bool _s##_char##_is_zero(const struct _s##_char* v0)
#ifdef M_GEN_is_zero
M_GEN_is_zero(vec2,i);
M_GEN_is_zero(vec2,f);
M_GEN_is_zero(vec2,d);
M_GEN_is_zero(vec3,i);
M_GEN_is_zero(vec3,f);
M_GEN_is_zero(vec3,d);
M_GEN_is_zero(vec4,i);
M_GEN_is_zero(vec4,f);
M_GEN_is_zero(vec4,d);
M_GEN_is_zero(quat,f);
M_GEN_is_zero(quat,d);
#endif

#define M_GEN_is_equal(_s,_char) bool _s##_char##_is_equal(const struct _s##_char* v0, const struct _s##_char* v1)
#ifdef M_GEN_is_equal
M_GEN_is_equal(vec2,i);
M_GEN_is_equal(vec2,f);
M_GEN_is_equal(vec2,d);
M_GEN_is_equal(vec3,i);
M_GEN_is_equal(vec3,f);
M_GEN_is_equal(vec3,d);
M_GEN_is_equal(vec4,i);
M_GEN_is_equal(vec4,f);
M_GEN_is_equal(vec4,d);
M_GEN_is_equal(quat,f);
M_GEN_is_equal(quat,d);
#endif

#define M_GEN_one(_s,_char) struct _s##_char* _s##_char##_one(struct _s##_char* result)
#ifdef M_GEN_one
M_GEN_one(vec2,i);
M_GEN_one(vec2,f);
M_GEN_one(vec2,d);
M_GEN_one(vec3,i);
M_GEN_one(vec3,f);
M_GEN_one(vec3,d);
M_GEN_one(vec4,i);
M_GEN_one(vec4,f);
M_GEN_one(vec4,d);
#endif

#define M_GEN_sign(_s,_char) struct _s##_char* _s##_char##_sign( struct _s##_char* result, const struct _s##_char* v0)
#ifdef M_GEN_sign
M_GEN_sign(vec2,i);
M_GEN_sign(vec2,f);
M_GEN_sign(vec2,d);
M_GEN_sign(vec3,i);
M_GEN_sign(vec3,f);
M_GEN_sign(vec3,d);
M_GEN_sign(vec4,i);
M_GEN_sign(vec4,f);
M_GEN_sign(vec4,d);
#endif

#define M_GEN_add(_s,_char) struct _s##_char* _s##_char##_add( struct _s##_char* result, const struct _s##_char* v0)
#ifdef M_GEN_add
M_GEN_add(vec2,i);
M_GEN_add(vec2,f);
M_GEN_add(vec2,d);
M_GEN_add(vec3,i);
M_GEN_add(vec3,f);
M_GEN_add(vec3,d);
M_GEN_add(vec4,i);
M_GEN_add(vec4,f);
M_GEN_add(vec4,d);
#endif

#define M_GEN_add_num(_s,_char) struct _s##_char* _s##_char##_add_num( struct _s##_char* result, const struct _s##_char* v0, m##_char##_t num )
#ifdef M_GEN_add_num
M_GEN_add_num(vec2,i);
M_GEN_add_num(vec2,f);
M_GEN_add_num(vec2,d);
M_GEN_add_num(vec3,i);
M_GEN_add_num(vec3,f);
M_GEN_add_num(vec3,d);
M_GEN_add_num(vec4,i);
M_GEN_add_num(vec4,f);
M_GEN_add_num(vec4,d);
#endif

#define M_GEN_subtract(_s,_char) struct _s##_char* _s##_char##_subtract( struct _s##_char* result, const struct _s##_char* v0)
#ifdef M_GEN_subtract
M_GEN_subtract(vec2,i);
M_GEN_subtract(vec2,f);
M_GEN_subtract(vec2,d);
M_GEN_subtract(vec3,i);
M_GEN_subtract(vec3,f);
M_GEN_subtract(vec3,d);
M_GEN_subtract(vec4,i);
M_GEN_subtract(vec4,f);
M_GEN_subtract(vec4,d);
#endif

#define M_GEN_subtract_num(_s,_char) struct _s##_char* _s##_char##_subtract_num( struct _s##_char* result, const struct _s##_char* v0, m##_char##_t num)
#ifdef M_GEN_subtract_num
M_GEN_subtract_num(vec2,i);
M_GEN_subtract_num(vec2,f);
M_GEN_subtract_num(vec2,d);
M_GEN_subtract_num(vec3,i);
M_GEN_subtract_num(vec3,f);
M_GEN_subtract_num(vec3,d);
M_GEN_subtract_num(vec4,i);
M_GEN_subtract_num(vec4,f);
M_GEN_subtract_num(vec4,d);
#endif

#define M_GEN_multiply(_s,_char) struct _s##_char* _s##_char##_multiply_##_s##_char( struct _s##_char* result, const struct _s##_char* a0, const struct _s##_char* a1)
#ifdef M_GEN_multiply
M_GEN_multiply(vec2,i);
M_GEN_multiply(vec2,f);
M_GEN_multiply(vec2,d);
M_GEN_multiply(vec3,i);
M_GEN_multiply(vec3,f);
M_GEN_multiply(vec3,d);
M_GEN_multiply(vec4,i);
M_GEN_multiply(vec4,f);
M_GEN_multiply(vec4,d);
M_GEN_multiply(mat2,f);
M_GEN_multiply(mat2,d);
M_GEN_multiply(mat3,f);
M_GEN_multiply(mat3,d);
M_GEN_multiply(mat4,f);
M_GEN_multiply(mat4,d);
#endif

#define M_GEN_multiply_num(_s,_char) struct _s##_char* _s##_char##_multiply_num( struct _s##_char* result, const struct _s##_char* v0, m##_char##_t num)
#ifdef M_GEN_multiply_num
M_GEN_multiply_num(vec2,i);
M_GEN_multiply_num(vec2,f);
M_GEN_multiply_num(vec2,d);
M_GEN_multiply_num(vec3,i);
M_GEN_multiply_num(vec3,f);
M_GEN_multiply_num(vec3,d);
M_GEN_multiply_num(vec4,i);
M_GEN_multiply_num(vec4,f);
M_GEN_multiply_num(vec4,d);
M_GEN_multiply_num(mat2,f);
M_GEN_multiply_num(mat2,d);
M_GEN_multiply_num(mat3,f);
M_GEN_multiply_num(mat3,d);
M_GEN_multiply_num(mat4,f);
M_GEN_multiply_num(mat4,d);
M_GEN_multiply_num(quat,f);
M_GEN_multiply_num(quat,d);
#endif

#define M_GEN_divide(_s,_char) struct _s##_char* _s##_char##_divide( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* v1)
#ifdef M_GEN_divide
M_GEN_divide(vec2,i);
M_GEN_divide(vec2,f);
M_GEN_divide(vec2,d);
M_GEN_divide(vec3,i);
M_GEN_divide(vec3,f);
M_GEN_divide(vec3,d);
M_GEN_divide(vec4,i);
M_GEN_divide(vec4,f);
M_GEN_divide(vec4,d);
#endif

#define M_GEN_divide_num(_s,_char) struct _s##_char* _s##_char##_divide_num( struct _s##_char* result, const struct _s##_char* v0, m##_char##_t num)
#ifdef M_GEN_divide_num
M_GEN_divide_num(vec2,i);
M_GEN_divide_num(vec2,f);
M_GEN_divide_num(vec2,d);
M_GEN_divide_num(vec3,i);
M_GEN_divide_num(vec3,f);
M_GEN_divide_num(vec3,d);
M_GEN_divide_num(vec4,i);
M_GEN_divide_num(vec4,f);
M_GEN_divide_num(vec4,d);
M_GEN_divide_num(quat,f);
M_GEN_divide_num(quat,d);
#endif

#define M_GEN_snap(_s,_char) struct _s##_char* _s##_char##_snap( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* v1)
#ifdef M_GEN_snap
M_GEN_snap(vec2,f);
M_GEN_snap(vec2,d);
M_GEN_snap(vec3,f);
M_GEN_snap(vec3,d);
M_GEN_snap(vec4,f);
M_GEN_snap(vec4,d);
#endif

#define M_GEN_snap_num(_s,_char) struct _s##_char* _s##_char##_snap_num( struct _s##_char* result, const struct _s##_char* v0, m##_char##_t num)
#ifdef M_GEN_snap_num
M_GEN_snap_num(vec2,f);
M_GEN_snap_num(vec2,d);
M_GEN_snap_num(vec3,f);
M_GEN_snap_num(vec3,d);
M_GEN_snap_num(vec4,f);
M_GEN_snap_num(vec4,d);
#endif

#define M_GEN_negative(_s,_char) struct _s##_char* _s##_char##_negative( struct _s##_char* result, const struct _s##_char* v0)
#ifdef M_GEN_negative
M_GEN_negative(vec2,f);
M_GEN_negative(vec2,d);
M_GEN_negative(vec2,i);
M_GEN_negative(vec3,f);
M_GEN_negative(vec3,d);
M_GEN_negative(vec3,i);
M_GEN_negative(vec4,f);
M_GEN_negative(vec4,d);
M_GEN_negative(vec4,i);
M_GEN_negative(quat,f);
M_GEN_negative(quat,d);
M_GEN_negative(mat2,f);
M_GEN_negative(mat2,d);
M_GEN_negative(mat3,f);
M_GEN_negative(mat3,d);
M_GEN_negative(mat4,f);
M_GEN_negative(mat4,d);
#endif

#define M_GEN_abs(_s,_char) struct _s##_char* _s##_char##_abs( struct _s##_char* result, const struct _s##_char* v0)
#ifdef M_GEN_abs
M_GEN_abs(vec2,f);
M_GEN_abs(vec2,d);
M_GEN_abs(vec2,i);
M_GEN_abs(vec3,f);
M_GEN_abs(vec3,d);
M_GEN_abs(vec3,i);
M_GEN_abs(vec4,f);
M_GEN_abs(vec4,d);
M_GEN_abs(vec4,i);
#endif

#define M_GEN_max(_s,_char) struct _s##_char* _s##_char##_max( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* v1)
#ifdef M_GEN_max
M_GEN_max(vec2,f);
M_GEN_max(vec2,d);
M_GEN_max(vec2,i);
M_GEN_max(vec3,f);
M_GEN_max(vec3,d);
M_GEN_max(vec3,i);
M_GEN_max(vec4,f);
M_GEN_max(vec4,d);
M_GEN_max(vec4,i);
#endif

#define M_GEN_min(_s,_char) struct _s##_char* _s##_char##_min( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* v1)
#ifdef M_GEN_min
M_GEN_min(vec2,f);
M_GEN_min(vec2,d);
M_GEN_min(vec2,i);
M_GEN_min(vec3,f);
M_GEN_min(vec3,d);
M_GEN_min(vec3,i);
M_GEN_min(vec4,f);
M_GEN_min(vec4,d);
M_GEN_min(vec4,i);
#endif

#define M_GEN_vec_clamp(_s,_char) struct _s##_char* _s##_char##_clamp( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* v1, const struct _s##_char* v2)
#ifdef M_GEN_vec_clamp
M_GEN_vec_clamp(vec2,f);
M_GEN_vec_clamp(vec2,d);
M_GEN_vec_clamp(vec2,i);
M_GEN_vec_clamp(vec3,f);
M_GEN_vec_clamp(vec3,d);
M_GEN_vec_clamp(vec3,i);
M_GEN_vec_clamp(vec4,f);
M_GEN_vec_clamp(vec4,d);
M_GEN_vec_clamp(vec4,i);
#endif

#define M_GEN_tangent(_s,_char) struct _s##_char* _s##_char##_tangent( struct _s##_char* result, const struct _s##_char* v0)
#ifdef M_GEN_tangent
M_GEN_tangent(vec2,i);
M_GEN_tangent(vec2,f);
M_GEN_tangent(vec2,d);
#endif

#define M_GEN_cross(_s,_char) struct _s##_char* _s##_char##_cross( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* v1)
#ifdef M_GEN_cross
M_GEN_cross(vec3,i);
M_GEN_cross(vec3,f);
M_GEN_cross(vec3,d);
#endif

#define M_GEN_multiply_mat2(_s,_char) struct _s##_char* _s##_char##_multiply_mat2##_char( struct _s##_char* result, const struct _s##_char* v0, const struct mat2##_char* m0)
#ifdef M_GEN_multiply_mat2
M_GEN_multiply_mat2(vec2,f);
M_GEN_multiply_mat2(vec2,d);
#endif

#define M_GEN_floor(_s,_char) struct _s##_char* _s##_char##_floor( struct _s##_char* result, const struct _s##_char* v0)
#ifdef M_GEN_floor
M_GEN_floor(vec2,f);
M_GEN_floor(vec2,d);
M_GEN_floor(vec3,f);
M_GEN_floor(vec3,d);
M_GEN_floor(vec4,f);
M_GEN_floor(vec4,d);
#endif

#define M_GEN_ceil(_s,_char) struct _s##_char* _s##_char##_ceil( struct _s##_char* result, const struct _s##_char* v0)
#ifdef M_GEN_ceil
M_GEN_ceil(vec2,f);
M_GEN_ceil(vec2,d);
M_GEN_ceil(vec3,f);
M_GEN_ceil(vec3,d);
M_GEN_ceil(vec4,f);
M_GEN_ceil(vec4,d);
#endif

#define M_GEN_round(_s,_char) struct _s##_char* _s##_char##_round( struct _s##_char* result, const struct _s##_char* v0)
#ifdef M_GEN_round
M_GEN_round(vec2,f);
M_GEN_round(vec2,d);
M_GEN_round(vec3,f);
M_GEN_round(vec3,d);
M_GEN_round(vec4,f);
M_GEN_round(vec4,d);
#endif

#define M_GEN_length_squared(_s,_char) m##_char##_t _s##_char##_length_squared( const struct _s##_char* v0 )
#ifdef M_GEN_length_squared
M_GEN_length_squared(vec2,f);
M_GEN_length_squared(vec2,d);
M_GEN_length_squared(vec3,f);
M_GEN_length_squared(vec3,d);
M_GEN_length_squared(vec4,f);
M_GEN_length_squared(vec4,d);
M_GEN_length_squared(quat,f);
M_GEN_length_squared(quat,d);
#endif

#define M_GEN_length(_s,_char) m##_char##_t _s##_char##_length( const struct _s##_char* v0 )
#ifdef M_GEN_length
M_GEN_length(vec2,f);
M_GEN_length(vec2,d);
M_GEN_length(vec3,f);
M_GEN_length(vec3,d);
M_GEN_length(vec4,f);
M_GEN_length(vec4,d);
M_GEN_length(quat,f);
M_GEN_length(quat,d);
#endif

#define M_GEN_normalize(_s,_char) struct _s##_char* _s##_char##_normalize( struct _s##_char* result, const struct _s##_char* v0)
#ifdef M_GEN_normalize
M_GEN_normalize(vec2,f);
M_GEN_normalize(vec2,d);
M_GEN_normalize(vec3,f);
M_GEN_normalize(vec3,d);
M_GEN_normalize(vec4,f);
M_GEN_normalize(vec4,d);
#endif

#define M_GEN_dot(_s,_char) m##_char##_t _s##_char##_dot( const struct _s##_char* v0, const struct _s##_char* v1 )
#ifdef M_GEN_dot
M_GEN_dot(vec2,f);
M_GEN_dot(vec2,d);
M_GEN_dot(vec3,f);
M_GEN_dot(vec3,d);
M_GEN_dot(vec4,f);
M_GEN_dot(vec4,d);
M_GEN_dot(quat,f);
M_GEN_dot(quat,d);
#endif

#define M_GEN_project(_s,_char) struct _s##_char* _s##_char##_project( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* v1 )
#ifdef M_GEN_project
M_GEN_project(vec2,f);
M_GEN_project(vec2,d);
M_GEN_project(vec3,f);
M_GEN_project(vec3,d);
M_GEN_project(vec4,f);
M_GEN_project(vec4,d);
#endif

#define M_GEN_slide(_s,_char) struct _s##_char* _s##_char##_slide( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* normal )
#ifdef M_GEN_slide
M_GEN_slide(vec2,f);
M_GEN_slide(vec2,d);
M_GEN_slide(vec3,f);
M_GEN_slide(vec3,d);
M_GEN_slide(vec4,f);
M_GEN_slide(vec4,d);
#endif

#define M_GEN_reflect(_s,_char) struct _s##_char* _s##_char##_reflect( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* normal )
#ifdef M_GEN_reflect
M_GEN_reflect(vec2,f);
M_GEN_reflect(vec2,d);
M_GEN_reflect(vec3,f);
M_GEN_reflect(vec3,d);
M_GEN_reflect(vec4,f);
M_GEN_reflect(vec4,d);
#endif

#define M_GEN_vec2_rotate(_char) struct vec2##_char* vec2##_char##_rotate( struct vec2##_char* result, const struct vec2##_char* v0, m##_char##_t f )
#ifdef M_GEN_vec2_rotate
M_GEN_vec2_rotate(f);
M_GEN_vec2_rotate(d);
#endif

#define M_GEN_vec3_rotate(_char) struct vec3##_char* vec3##_char##_rotate( struct vec3##_char* result, const struct vec3##_char* v0, const struct vec3##_char* ra, m##_char##_t f )
#ifdef M_GEN_vec3_rotate
M_GEN_vec3_rotate(f);
M_GEN_vec3_rotate(d);
#endif

#define M_GEN_vec3i_rotate struct vec3i* vec3i_rotate(struct vec3i* result, const struct vec3i* v, enum mcorner_vector cv)
#ifdef M_GEN_vec3i_rotate
M_GEN_vec3i_rotate;
#endif

#define M_GEN_lerp(_s,_char) struct _s##_char* _s##_char##_lerp( struct _s##_char* result, const struct _s##_char* a0, const struct _s##_char* a1, m##_char##_t f )
#ifdef M_GEN_lerp
M_GEN_lerp(vec2,f);
M_GEN_lerp(vec2,d);
M_GEN_lerp(vec3,f);
M_GEN_lerp(vec3,d);
M_GEN_lerp(vec4,f);
M_GEN_lerp(vec4,d);
M_GEN_lerp(mat2,f);
M_GEN_lerp(mat2,d);
M_GEN_lerp(mat3,f);
M_GEN_lerp(mat3,d);
M_GEN_lerp(mat4,f);
M_GEN_lerp(mat4,d);
M_GEN_lerp(quat,f);
M_GEN_lerp(quat,d);
#endif

#define M_GEN_bezier3(_s,_char) struct _s##_char* _s##_char##_bezier3( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* v1, const struct _s##_char* v2, m##_char##_t f )
#ifdef M_GEN_bezier3
M_GEN_bezier3(vec2,f);
M_GEN_bezier3(vec2,d);
M_GEN_bezier3(vec3,f);
M_GEN_bezier3(vec3,d);
#endif

#define M_GEN_bezier4(_s,_char) struct _s##_char* _s##_char##_bezier4( struct _s##_char* result, const struct _s##_char* v0, const struct _s##_char* v1, const struct _s##_char* v2, const struct _s##_char* v3, m##_char##_t f )
#ifdef M_GEN_bezier4
M_GEN_bezier4(vec2,f);
M_GEN_bezier4(vec2,d);
M_GEN_bezier4(vec3,f);
M_GEN_bezier4(vec3,d);
#endif

#define M_GEN_vec2_angle(_char) m##_char##_t vec2##_char##_angle( const struct vec2##_char* v0 )
#ifdef M_GEN_vec2_angle
M_GEN_vec2_angle(f);
M_GEN_vec2_angle(d);
#endif

#define M_GEN_distance_squared(_reschar,_s,_char) m##_reschar##_t _s##_char##_distance_squared_##_s##_char( const struct _s##_char* v0, const struct _s##_char* v1 )
#ifdef M_GEN_distance_squared
M_GEN_distance_squared(d,vec2,i);
M_GEN_distance_squared(f,vec2,f);
M_GEN_distance_squared(d,vec2,d);
M_GEN_distance_squared(d,vec3,i);
M_GEN_distance_squared(f,vec3,f);
M_GEN_distance_squared(d,vec3,d);
M_GEN_distance_squared(d,vec4,i);
M_GEN_distance_squared(f,vec4,f);
M_GEN_distance_squared(d,vec4,d);
#endif

#define M_GEN_distance(_reschar,_s,_char) m##_reschar##_t _s##_char##_distance_##_s##_char( const struct _s##_char* v0, const struct _s##_char* v1 )
#ifdef M_GEN_distance
M_GEN_distance(d,vec2,i);
M_GEN_distance(f,vec2,f);
M_GEN_distance(d,vec2,d);
M_GEN_distance(d,vec3,i);
M_GEN_distance(f,vec3,f);
M_GEN_distance(d,vec3,d);
M_GEN_distance(d,vec4,i);
M_GEN_distance(f,vec4,f);
M_GEN_distance(d,vec4,d);
#endif

#define M_GEN_triface_distance(_trichar,_s,_n,_char) m##_char##_t triface##_n##_trichar##_distance_##_s##_n##_char( const struct triface##_n##_trichar* t0, const struct _s##_n##_char* v0 )
#ifdef M_GEN_triface_distance
M_GEN_triface_distance(i,vec,3,i);
M_GEN_triface_distance(i,vec,3,f);
M_GEN_triface_distance(i,vec,3,d);
#endif

#define M_GEN_line_distance_squared md_t line3i_distance_squared_vec3i(const struct line3i* l0, const struct vec3i* v0)
#ifdef M_GEN_line_distance_squared
M_GEN_line_distance_squared;
#endif

#define M_GEN_line_distance md_t line3i_distance_vec3i(const struct line3i* l0, const struct vec3i* v0)
#ifdef M_GEN_line_distance
M_GEN_line_distance;
#endif

#define M_GEN_vec2_linear_independent(_char) bool vec2##_char##_linear_independent(const struct vec2##_char* v0, struct vec2##_char* v1)
#ifdef M_GEN_vec2_linear_independent
M_GEN_vec2_linear_independent(f);
M_GEN_vec2_linear_independent(d);
#endif

#define M_GEN_vec3_linear_independent(_char) bool vec3##_char##_linear_independent(const struct vec3##_char* v0, struct vec3##_char* v1, struct vec3##_char* v2 )
#ifdef M_GEN_vec3_linear_independent
M_GEN_vec3_linear_independent(f);
M_GEN_vec3_linear_independent(d);
#endif

#define M_GEN_multiply_mat3(_s,_char) struct _s##_char* _s##_char##_multiply_mat3##_char(struct _s##_char* result, const struct _s##_char* v0, const struct mat3##_char* m0)
#ifdef M_GEN_multiply_mat3
M_GEN_multiply_mat3(vec3,f);
M_GEN_multiply_mat3(vec3,d);
#endif

#define M_GEN_multiply_mat4(_s,_char) struct _s##_char* _s##_char##_multiply_mat4##_char(struct _s##_char* result, struct _s##_char* v0, const struct mat4##_char* m0)
#ifdef M_GEN_multiply_mat4
M_GEN_multiply_mat4(vec4,f);
M_GEN_multiply_mat4(vec4,d);
#endif

#define M_GEN_null(_s,_char) struct _s##_char* _s##_char##_null(struct _s##_char* result)
#ifdef M_GEN_null
M_GEN_null(quat,f);
M_GEN_null(quat,d);
#endif

#define M_GEN_quat_divide(_s,_char) struct _s##_char* _s##_char##_divide(struct _s##_char* result, const struct _s##_char* q0, const struct _s##_char* q1)
#ifdef M_GEN_quat_divide
M_GEN_quat_divide(quat,f);
M_GEN_quat_divide(quat,d);
#endif

#define M_GEN_quat_conjugate(_s,_char) struct _s##_char* _s##_char##_conjugate(struct _s##_char* result, const struct _s##_char* q0)
#ifdef M_GEN_quat_conjugate
M_GEN_quat_conjugate(quat,f);
M_GEN_quat_conjugate(quat,d);
#endif

#define M_GEN_inverse(_s,_char) struct _s##_char* _s##_char##_inverse(struct _s##_char* result, const struct _s##_char* a0)
#ifdef M_GEN_inverse
M_GEN_inverse(mat2,f);
M_GEN_inverse(mat2,d);
M_GEN_inverse(mat3,f);
M_GEN_inverse(mat3,d);
M_GEN_inverse(mat4,f);
M_GEN_inverse(mat4,d);
M_GEN_inverse(quat,f);
M_GEN_inverse(quat,d);
#endif

#define M_GEN_quat_normalize(_s,_char) struct _s##_char* _s##_char##_normalize(struct _s##_char* result, const struct _s##_char* q0)
#ifdef M_GEN_quat_normalize
M_GEN_quat_normalize(quat,f);
M_GEN_quat_normalize(quat,d);
#endif

#define M_GEN_quat_power(_s,_char) struct _s##_char* _s##_char##_power(struct _s##_char* result, const struct _s##_char* q0, m##_char##_t exponent)
#ifdef M_GEN_quat_power
M_GEN_quat_power(quat,f);
M_GEN_quat_power(quat,d);
#endif

#define M_GEN_quat_from_axis_angle(_s,_char) struct _s##_char* _s##_char##_from_axis_angle(struct _s##_char* result, const struct _s##_char* v0, m##_char##_t angle)
#ifdef M_GEN_quat_from_axis_angle
M_GEN_quat_from_axis_angle(quat,f);
M_GEN_quat_from_axis_angle(quat,d);
#endif

#define M_GEN_quat_from_vec3(_char) struct quat##_char* quat##_char##_from_vec3(struct quat##_char* result, const struct vec3##_char* v0, struct vec3##_char* v1)
#ifdef M_GEN_quat_from_vec3
M_GEN_quat_from_vec3(f);
M_GEN_quat_from_vec3(d);
#endif

#define M_GEN_quat_from_mat4(_s,_char) struct _s##_char* _s##_char##_from_mat4(struct _s##_char* result, const struct mat4##_char* m0)
#ifdef M_GEN_quat_from_mat4
M_GEN_quat_from_mat4(quat,f);
M_GEN_quat_from_mat4(quat,d);
#endif

#define M_GEN_quat_slerp(_s,_char) struct _s##_char* _s##_char##_slerp(struct _s##_char* result, const struct _s##_char* q0, const struct _s##_char* q1, m##_char##_t f)
#ifdef M_GEN_quat_slerp
M_GEN_quat_slerp(quat,f);
M_GEN_quat_slerp(quat,d);
#endif

#define M_GEN_quat_angle(_s,_char) m##_char##_t _s##_char##_angle(const struct _s##_char* q0, const struct _s##_char* q1)
#ifdef M_GEN_quat_angle
M_GEN_quat_angle(quat,f);
M_GEN_quat_angle(quat,d);
#endif

#define M_GEN_identity(_s,_char) struct _s##_char* _s##_char##_identity(struct _s##_char* result)
#ifdef M_GEN_identity
M_GEN_identity(mat2,f);
M_GEN_identity(mat2,d);
M_GEN_identity(mat3,f);
M_GEN_identity(mat3,d);
M_GEN_identity(mat4,f);
M_GEN_identity(mat4,d);
#endif

#define M_GEN_determinant(_s,_char) m##_char##_t _s##_char##_determinant(const struct _s##_char* m0)
#ifdef M_GEN_determinant
M_GEN_determinant(mat2,f);
M_GEN_determinant(mat2,d);
M_GEN_determinant(mat3,f);
M_GEN_determinant(mat3,d);
M_GEN_determinant(mat4,f);
M_GEN_determinant(mat4,d);
#endif

#define M_GEN_transpose(_s,_char) struct _s##_char* _s##_char##_transpose(struct _s##_char* result, const struct _s##_char* m0)
#ifdef M_GEN_transpose
M_GEN_transpose(mat2,f);
M_GEN_transpose(mat2,d);
M_GEN_transpose(mat3,f);
M_GEN_transpose(mat3,d);
M_GEN_transpose(mat4,f);
M_GEN_transpose(mat4,d);
#endif

#define M_GEN_cofactor(_s,_char) struct _s##_char* _s##_char##_cofactor(struct _s##_char* result, const struct _s##_char* m0)
#ifdef M_GEN_cofactor
M_GEN_cofactor(mat2,f);
M_GEN_cofactor(mat2,d);
M_GEN_cofactor(mat3,f);
M_GEN_cofactor(mat3,d);
M_GEN_cofactor(mat4,f);
M_GEN_cofactor(mat4,d);
#endif

#define M_GEN_adjugate(_s,_char) struct _s##_char* _s##_char##_adjugate(struct _s##_char* result, const struct _s##_char* m0)
#ifdef M_GEN_adjugate
M_GEN_cofactor(mat2,f);
M_GEN_cofactor(mat2,d);
#endif

#define M_GEN_scaling(_s,_n,_char) struct _s##_n##_char* _s##_n##_char##_scaling(struct _s##_n##_char* result, struct vec##_n##_char* v0)
#ifdef M_GEN_scaling
M_GEN_scaling(mat,2,f);
M_GEN_scaling(mat,2,d);
M_GEN_scaling(mat,3,f);
M_GEN_scaling(mat,3,d);
M_GEN_scaling(mat,4,f);
M_GEN_scaling(mat,4,d);
#endif

#define M_GEN_scale(_s,_n,_char) struct _s##_n##_char* _s##_n##_char##_scale(struct _s##_n##_char* result, const struct vec##_n##_char* v0)
#ifdef M_GEN_scale
M_GEN_scale(mat,2,f);
M_GEN_scale(mat,2,d);
M_GEN_scale(mat,3,f);
M_GEN_scale(mat,3,d);
M_GEN_scale(mat,4,f);
M_GEN_scale(mat,4,d);
#endif

#define M_GEN_rotation_x(_s,_char) struct _s##_char* _s##_char##_rotation_x(struct _s##_char* result, m##_char##_t f)
#ifdef M_GEN_rotation_x
M_GEN_rotation_x(mat3,f);
M_GEN_rotation_x(mat3,d);
M_GEN_rotation_x(mat4,f);
M_GEN_rotation_x(mat4,d);
#endif

#define M_GEN_rotation_y(_s,_char) struct _s##_char* _s##_char##_rotation_y(struct _s##_char* result, m##_char##_t f)
#ifdef M_GEN_rotation_y
M_GEN_rotation_y(mat3,f);
M_GEN_rotation_y(mat3,d);
M_GEN_rotation_y(mat4,f);
M_GEN_rotation_y(mat4,d);
#endif

#define M_GEN_rotation_z(_s,_char) struct _s##_char* _s##_char##_rotation_z(struct _s##_char* result, m##_char##_t f)
#ifdef M_GEN_rotation_z
M_GEN_rotation_z(mat2,f);
M_GEN_rotation_z(mat2,d);
M_GEN_rotation_z(mat3,f);
M_GEN_rotation_z(mat3,d);
M_GEN_rotation_z(mat4,f);
M_GEN_rotation_z(mat4,d);
#endif

#define M_GEN_rotation_axis(_s,_char) struct _s##_char* _s##_char##_rotation_axis(struct _s##_char* result, const struct vec3##_char* v0, m##_char##_t f)
#ifdef M_GEN_rotation_axis
M_GEN_rotation_axis(mat3,f);
M_GEN_rotation_axis(mat3,d);
M_GEN_rotation_axis(mat4,f);
M_GEN_rotation_axis(mat4,d);
#endif

#define M_GEN_rotation_quat(_s,_char) struct _s##_char* _s##_char##_rotation_quat(struct _s##_char* result, const struct quat##_char* q0)
#ifdef M_GEN_rotation_quat
M_GEN_rotation_quat(mat3,f);
M_GEN_rotation_quat(mat3,d);
M_GEN_rotation_quat(mat4,f);
M_GEN_rotation_quat(mat4,d);
#endif

#define M_GEN_translation(_s,_char) struct _s##_char* _s##_char##_translation(struct _s##_char* result, const struct _s##_char* m0, const struct vec3##_char* v0)
#ifdef M_GEN_translation
M_GEN_translation(mat4,f);
M_GEN_translation(mat4,d);
#endif

#define M_GEN_translate(_s,_char) struct _s##_char* _s##_char##_translate(struct _s##_char* result, const struct _s##_char* m0, const struct vec3##_char* v0)
#ifdef M_GEN_translate
M_GEN_translate(mat4,f);
M_GEN_translate(mat4,d);
#endif

#define M_GEN_mat4_look_at(_char) struct mat4##_char* mat4##_char##_look_at(struct mat4##_char* result, const struct vec3##_char* position, const struct vec3##_char* target, const struct vec3##_char* up)
#ifdef M_GEN_mat4_look_at
M_GEN_mat4_look_at(f);
M_GEN_mat4_look_at(d);
#endif

#define M_GEN_ortho(_s,_char) struct _s##_char* _s##_char##_ortho(struct _s##_char* result, m##_char##_t l, m##_char##_t r, m##_char##_t b, m##_char##_t t, m##_char##_t n, m##_char##_t f)
#ifdef M_GEN_ortho
M_GEN_ortho(mat4,f);
M_GEN_ortho(mat4,d);
#endif

#define M_GEN_perspective(_s,_char) struct _s##_char* _s##_char##_perspective(struct _s##_char* result, m##_char##_t fov_y, m##_char##_t aspect, m##_char##_t n, m##_char##_t f)
#ifdef M_GEN_perspective
M_GEN_perspective(mat4,f);
M_GEN_perspective(mat4,d);
#endif

#define M_GEN_perspective_fov(_s,_char) struct _s##_char* _s##_char##_perspective_fov(struct _s##_char* result, m##_char##_t fov, m##_char##_t w, m##_char##_t h, m##_char##_t n, m##_char##_t f)
#ifdef M_GEN_perspective_fov
M_GEN_perspective_fov(mat4,f);
M_GEN_perspective_fov(mat4,d);
#endif

#define M_GEN_perspective_infinite(_s,_char) struct _s##_char* _s##_char##_perspective_infinite(struct _s##_char* result, m##_char##_t fov_y, m##_char##_t aspect, m##_char##_t n)
#ifdef M_GEN_perspective_infinite
M_GEN_perspective_infinite(mat4,f);
M_GEN_perspective_infinite(mat4,d);
#endif

#define M_GEN_rect_extend struct rect3i* rect3i_extend(struct rect3i* result, mi_t f)
#ifdef M_GEN_rect_extend
M_GEN_rect_extend;
#endif

#define M_GEN_triface_calc_norm(_char) struct triface3##_char* triface3##_char##_calc_norm(struct triface3##_char* t0)
#ifdef M_GEN_triface_calc_norm
M_GEN_triface_calc_norm(i);
#endif

#define M_GEN_extremes_find struct extremes3i* extremes3i_find( struct extremes3i* result, cvector_vec3i points )
#ifdef M_GEN_extremes_find
M_GEN_extremes_find;
#endif

#define M_GEN_convex_hull_update void convex_hull3i_update( struct convex_hull3i* result, cvector_vec3i points, const struct extremes3i* extremes )
#ifdef M_GEN_convex_hull_update
M_GEN_convex_hull_update;
#endif

#define M_GEN_vec_inside_convex_hull bool vec3i_inside_convex_hull3i(const struct convex_hull3i* c0, const struct vec3i* v0)
#ifdef M_GEN_vec_inside_convex_hull
M_GEN_vec_inside_convex_hull;
#endif

#define M_GEN_quadratic_ease_out(_char) m##_char##_t quadratic_ease_out##_char(m##_char##_t f)
#ifdef M_GEN_quadratic_ease_out
M_GEN_quadratic_ease_out(f);
M_GEN_quadratic_ease_out(d);
#endif

#define M_GEN_quadratic_ease_in(_char) m##_char##_t quadratic_ease_in##_char(m##_char##_t f)
#ifdef M_GEN_quadratic_ease_in
M_GEN_quadratic_ease_in(f);
M_GEN_quadratic_ease_in(d);
#endif

#define M_GEN_quadratic_ease_in_out(_char) m##_char##_t quadratic_ease_in_out##_char(m##_char##_t f)
#ifdef M_GEN_quadratic_ease_in_out
M_GEN_quadratic_ease_in_out(f);
M_GEN_quadratic_ease_in_out(d);
#endif

#define M_GEN_cubic_ease_out(_char) m##_char##_t cubic_ease_out##_char(m##_char##_t f)
#ifdef M_GEN_cubic_ease_out
M_GEN_cubic_ease_out(f);
M_GEN_cubic_ease_out(d);
#endif

#define M_GEN_cubic_ease_in(_char) m##_char##_t cubic_ease_in##_char(m##_char##_t f)
#ifdef M_GEN_cubic_ease_in
M_GEN_cubic_ease_in(f);
M_GEN_cubic_ease_in(d);
#endif

#define M_GEN_cubic_ease_in_out(_char) m##_char##_t cubic_ease_in_out##_char(m##_char##_t f)
#ifdef M_GEN_cubic_ease_in_out
M_GEN_cubic_ease_in_out(f);
M_GEN_cubic_ease_in_out(d);
#endif

#define M_GEN_quartic_ease_out(_char) m##_char##_t quartic_ease_out##_char(m##_char##_t f)
#ifdef M_GEN_quartic_ease_out
M_GEN_quartic_ease_out(f);
M_GEN_quartic_ease_out(d);
#endif

#define M_GEN_quartic_ease_in(_char) m##_char##_t quartic_ease_in##_char(m##_char##_t f)
#ifdef M_GEN_quartic_ease_in
M_GEN_quartic_ease_in(f);
M_GEN_quartic_ease_in(d);
#endif

#define M_GEN_quartic_ease_in_out(_char) m##_char##_t quartic_ease_in_out##_char(m##_char##_t f)
#ifdef M_GEN_quartic_ease_in_out
M_GEN_quartic_ease_in_out(f);
M_GEN_quartic_ease_in_out(d);
#endif

#define M_GEN_quintic_ease_out(_char) m##_char##_t quintic_ease_out##_char(m##_char##_t f)
#ifdef M_GEN_quintic_ease_out
M_GEN_quintic_ease_out(f);
M_GEN_quintic_ease_out(d);
#endif

#define M_GEN_quintic_ease_in(_char) m##_char##_t quintic_ease_in##_char(m##_char##_t f)
#ifdef M_GEN_quintic_ease_in
M_GEN_quintic_ease_in(f);
M_GEN_quintic_ease_in(d);
#endif

#define M_GEN_quintic_ease_in_out(_char) m##_char##_t quintic_ease_in_out##_char(m##_char##_t f)
#ifdef M_GEN_quintic_ease_in_out
M_GEN_quintic_ease_in_out(f);
M_GEN_quintic_ease_in_out(d);
#endif

#define M_GEN_sine_ease_out(_char) m##_char##_t sine_ease_out##_char(m##_char##_t f)
#ifdef M_GEN_sine_ease_out
M_GEN_sine_ease_out(f);
M_GEN_sine_ease_out(d);
#endif

#define M_GEN_sine_ease_in(_char) m##_char##_t sine_ease_in##_char(m##_char##_t f)
#ifdef M_GEN_sine_ease_in
M_GEN_sine_ease_in(f);
M_GEN_sine_ease_in(d);
#endif

#define M_GEN_sine_ease_in_out(_char) m##_char##_t sine_ease_in_out##_char(m##_char##_t f)
#ifdef M_GEN_sine_ease_in_out
M_GEN_sine_ease_in_out(f);
M_GEN_sine_ease_in_out(d);
#endif

#define M_GEN_circular_ease_out(_char) m##_char##_t circular_ease_out##_char(m##_char##_t f)
#ifdef M_GEN_circular_ease_out
M_GEN_circular_ease_out(f);
M_GEN_circular_ease_out(d);
#endif

#define M_GEN_circular_ease_in(_char) m##_char##_t circular_ease_in##_char(m##_char##_t f)
#ifdef M_GEN_circular_ease_in
M_GEN_circular_ease_in(f);
M_GEN_circular_ease_in(d);
#endif

#define M_GEN_circular_ease_in_out(_char) m##_char##_t circular_ease_in_out##_char(m##_char##_t f)
#ifdef M_GEN_circular_ease_in_out
M_GEN_circular_ease_in_out(f);
M_GEN_circular_ease_in_out(d);
#endif

#define M_GEN_exponential_ease_out(_char) m##_char##_t exponential_ease_out##_char(m##_char##_t f)
#ifdef M_GEN_exponential_ease_out
M_GEN_exponential_ease_out(f);
M_GEN_exponential_ease_out(d);
#endif

#define M_GEN_exponential_ease_in(_char) m##_char##_t exponential_ease_in##_char(m##_char##_t f)
#ifdef M_GEN_exponential_ease_in
M_GEN_exponential_ease_in(f);
M_GEN_exponential_ease_in(d);
#endif

#define M_GEN_exponential_ease_in_out(_char) m##_char##_t exponential_ease_in_out##_char(m##_char##_t f)
#ifdef M_GEN_exponential_ease_in_out
M_GEN_exponential_ease_in_out(f);
M_GEN_exponential_ease_in_out(d);
#endif

#define M_GEN_elastic_ease_out(_char) m##_char##_t elastic_ease_out##_char(m##_char##_t f)
#ifdef M_GEN_elastic_ease_out
M_GEN_elastic_ease_out(f);
M_GEN_elastic_ease_out(d);
#endif

#define M_GEN_elastic_ease_in(_char) m##_char##_t elastic_ease_in##_char(m##_char##_t f)
#ifdef M_GEN_elastic_ease_in
M_GEN_elastic_ease_in(f);
M_GEN_elastic_ease_in(d);
#endif

#define M_GEN_elastic_ease_in_out(_char) m##_char##_t elastic_ease_in_out##_char(m##_char##_t f)
#ifdef M_GEN_elastic_ease_in_out
M_GEN_elastic_ease_in_out(f);
M_GEN_elastic_ease_in_out(d);
#endif

#define M_GEN_back_ease_out(_char) m##_char##_t back_ease_out##_char(m##_char##_t f)
#ifdef M_GEN_back_ease_out
M_GEN_back_ease_out(f);
M_GEN_back_ease_out(d);
#endif

#define M_GEN_back_ease_in(_char) m##_char##_t back_ease_in##_char(m##_char##_t f)
#ifdef M_GEN_back_ease_in
M_GEN_back_ease_in(f);
M_GEN_back_ease_in(d);
#endif

#define M_GEN_back_ease_in_out(_char) m##_char##_t back_ease_in_out##_char(m##_char##_t f)
#ifdef M_GEN_back_ease_in_out
M_GEN_back_ease_in_out(f);
M_GEN_back_ease_in_out(d);
#endif

#define M_GEN_bounce_ease_out(_char) m##_char##_t bounce_ease_out##_char(m##_char##_t f)
#ifdef M_GEN_bounce_ease_out
M_GEN_bounce_ease_out(f);
M_GEN_bounce_ease_out(d);
#endif

#define M_GEN_bounce_ease_in(_char) m##_char##_t bounce_ease_in##_char(m##_char##_t f)
#ifdef M_GEN_bounce_ease_in
M_GEN_bounce_ease_in(f);
M_GEN_bounce_ease_in(d);
#endif

#define M_GEN_bounce_ease_in_out(_char) m##_char##_t bounce_ease_in_out##_char(m##_char##_t f)
#ifdef M_GEN_bounce_ease_in_out
M_GEN_bounce_ease_in_out(f);
M_GEN_bounce_ease_in_out(d);
#endif

#ifdef DEBUG
#include <stdio.h>

#define M_TRACE printf("%s:%d ",__MAPYR__FILENAME__,__LINE__)

#define M_PRINT_VEC3(vector) \
printf("[x=%.2f y=%.2f z=%.2f]", \
(float)vector.x, \
(float)vector.y, \
(float)vector.z);

#define M_PRINT_VEC3(vector) \
printf("[x=%.2f y=%.2f z=%.2f]", \
(float)vector.x, \
(float)vector.y, \
(float)vector.z);

#define M_PRINT_MAT4(mat) \
printf("| %5.2f | %5.2f | %5.2f | %5.2f |\n", (mat).m11, (mat).m12, (mat).m13, (mat).m14);\
printf("| %5.2f | %5.2f | %5.2f | %5.2f |\n", (mat).m21, (mat).m22, (mat).m23, (mat).m24);\
printf("| %5.2f | %5.2f | %5.2f | %5.2f |\n", (mat).m31, (mat).m32, (mat).m33, (mat).m34);\
printf("| %5.2f | %5.2f | %5.2f | %5.2f |\n", (mat).m41, (mat).m42, (mat).m43, (mat).m44);

#define M_PRINT_SEGMENT3(segment) \
M_PRINT_VEC3(segment.start) \
M_PRINT_VEC3(segment.end)

#define M_PRINT_TRIANGLE3(triangle) \
for(int i =0; i<3; i++){ \
    M_PRINT_VEC3(triangle.p[i]) \
} \
 \
printf(" normal="); \
M_PRINT_VEC3(triangle.normal);

#define M_PRINT_CONVEX_PLANE(plane) \
M_PRINT_TRIANGLE3(plane.triangle) \
printf(" outside_points="); \
cvector_iterator(gm_vct3i) pnt; \
cvector_for_each_in(pnt,(plane.outside_points)){ \
    M_PRINT_VEC3((*pnt)); \
}

#endif


#endif
