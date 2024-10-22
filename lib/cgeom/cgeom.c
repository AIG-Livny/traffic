#include "cgeom.h"
#include "iso646.h"

typedef void (*errorcb_t)(int);

int error_code = 0;
static errorcb_t error_callback = NULL;

void throw(int code){
    error_code = code;
    if ( error_callback ) {
        error_callback(code);
    }
}


#if defined(CGEOM_PRECISE_TRIGONOMETRY)
/* The following code (sincos1cos and float and double versions of
 * sncs1cs) is Copyright © 1985, 1995, 2000 Stephen L. Moshier and
 * Copyright © 2020 Neven Sajko. The intention is to get accurate
 * 1-cosine, while also getting the sine and cosine as a bonus. The
 * implementation is derived from the Cephes Math Library's sin.c and
 * sinf.c. To be more specific, I took Stephen Moshier's sin, cos, sinf
 * and cosf (without changing the polynomials) and adapted them to give
 * all three required function values (in double and float versions),
 * without unnecessary accuracy losses.
 *
 * sncs1cs is not correct for values of x of huge magnitude. That can
 * be fixed by more elaborate range reduction.
 */

/* Sine, cosine, 1-cosine */
#define M_GEN_sincos1cos(_char)     \
typedef struct {                    \
    m##_char##_t sin, cos, omc;     \
} sincos1cos##_char;

M_GEN_sincos1cos(f);
M_GEN_sincos1cos(d);

#define M_GEN_sncs1cs(_char) static sincos1cos##_char sncs1cs##_char(m##_char##_t x)

#define SNCSf \
const float sc[] = {-1.9515295891E-4, 8.3321608736E-3, -1.6666654611E-1}; \
const float cc[] = {2.443315711809948E-5, -1.388731625493765E-3, 4.166664568298827E-2}; \
\
/* These are for a 24-bit significand: */ \
const float DP1 = 0.78515625; \
const float DP2 = 2.4187564849853515625e-4; \
const float DP3 = 3.77489497744594108e-8;\
\
/* Extended precision modular arithmetic */ \
z = ((x - y * DP1) - y * DP2) - y * DP3; \
zz = z * z; \
r.sin = z + zz*z*((sc[0]*zz + sc[1])*zz + sc[2]); \
r.omc = (float)0.5*zz - zz*zz*((cc[0]*zz + cc[1])*zz + cc[2]);

#define SNCSd \
const double sc[] = { \
    1.58962301576546568060E-10, \
    -2.50507477628578072866E-8, \
    2.75573136213857245213E-6, \
    -1.98412698295895385996E-4, \
    8.33333333332211858878E-3, \
    -1.66666666666666307295E-1, \
}; \
 \
const double cc[] = { \
    -1.13585365213876817300E-11, \
    2.08757008419747316778E-9, \
    -2.75573141792967388112E-7, \
    2.48015872888517045348E-5, \
    -1.38888888888730564116E-3, \
    4.16666666666665929218E-2, \
}; \
 \
const double DP1 = 7.85398125648498535156E-1; \
const double DP2 = 3.77489470793079817668E-8; \
const double DP3 = 2.69515142907905952645E-15; \
 \
/* Extended precision modular arithmetic */ \
z = ((x - y * DP1) - y * DP2) - y * DP3; \
zz = z * z; \
r.sin = z + zz*z*(((((sc[0]*zz + sc[1])*zz + sc[2])*zz + sc[3])*zz + sc[4])*zz + sc[5]); \
r.omc = (double)0.5*zz - zz*zz*(((((cc[0]*zz + cc[1])*zz + cc[2])*zz + cc[3])*zz + cc[4])*zz + cc[5]);

#define M_GEN_IMPL_sncs1cs(_char) \
M_GEN_sncs1cs(_char) { \
    const m##_char##_t fourOverPi = 1.27323954473516268615; \
    \
    m##_char##_t y, z, zz; \
    mi_t j, sign = 1, csign = 1; \
    sincos1cos##_char r; \
    \
    /* Handle +-0. */ \
    if (x == (m##_char##_t)0) { \
        r.sin = x; \
        r.cos = 1; \
        r.omc = 0; \
        return r; \
    } \
    if (isnan(x)) { \
        r.sin = r.cos = r.omc = x; \
        return r; \
    } \
    if (isinf(x)) { \
        r.sin = r.cos = r.omc = x - x; \
        return r; \
    } \
    if (x < 0) { \
        sign = -1; \
        x = -x; \
    } \
    j = (mi_t)(x * fourOverPi); \
    y = (m##_char##_t)j; \
    /* map zeros to origin */ \
    if ((j & 1)) { \
        j += 1; \
        y += 1; \
    } \
    j = j & 7; /* octant modulo one turn */ \
    /* reflect in x axis */ \
    if (j > 3) { \
        sign = -sign; \
        csign = -csign; \
        j -= 4; \
    } \
    if (j > 1) { \
        csign = -csign; \
    } \
    SNCS##_char; \
    \
    if (j == 1 || j == 2) { \
        if (csign < 0) { \
            r.sin = -r.sin; \
        } \
        r.cos = r.sin; \
        r.sin = 1 - r.omc; \
        r.omc = 1 - r.cos; \
    } else { \
        if (csign < 0) { \
            r.cos = r.omc - 1; \
            r.omc = 1 - r.cos; \
        } else { \
            r.cos = 1 - r.omc; \
        } \
    } \
    if (sign < 0) { \
        r.sin = -r.sin; \
    } \
    return r; \
}

M_GEN_IMPL_sncs1cs(f);
M_GEN_IMPL_sncs1cs(d);
#endif

#define ELEM_TYPE(name) typeof((name)->v[0])
#define ELEM_NUM(name) sizeof((name)->v)/sizeof(ELEM_TYPE(name))


#ifdef M_GEN_vec3i_rotate
static const mi_t ortho_bases[24][9] = {
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
#endif

// Compare all memory with value
static int memvcmp(void *memory, unsigned char val, unsigned int size) {
    unsigned char *mm = (unsigned char*)memory;
    return (*mm == val) && memcmp(mm, mm + 1, size - 1) == 0;
}

#ifdef M_GEN_nearly_equal
#define M_GEN_IMPL_nearly_equal(_char) \
M_GEN_nearly_equal(_char){ \
    bool result = false; \
    if (a == b) { \
        result = true; \
    } else if (M##_char##FABS(a - b) <= epsilon) { \
        result = true; \
    } \
    return result; \
}

M_GEN_IMPL_nearly_equal(f);
M_GEN_IMPL_nearly_equal(d);
#endif

#ifdef M_GEN_clamp
#define M_GEN_IMPL_clamp(_char)  \
M_GEN_clamp(_char) { \
    if (value < min) { \
        value = min; \
    } else if (value > max) { \
        value = max; \
    } \
    return value; \
}

M_GEN_IMPL_clamp(i);
M_GEN_IMPL_clamp(f);
M_GEN_IMPL_clamp(d);
#endif

#ifdef M_GEN_to_radians
#define M_GEN_IMPL_to_radians(_char) \
M_GEN_to_radians(_char) { \
    return (degrees * M##_char##PI / (m##_char##_t)180.0); \
}

M_GEN_IMPL_to_radians(f);
M_GEN_IMPL_to_radians(d);
#endif

#ifdef M_GEN_to_degrees
#define M_GEN_IMPL_to_degrees(_char) \
M_GEN_to_degrees(_char) { \
    return (radians * (m##_char##_t)180.0 / M##_char##PI); \
}

M_GEN_IMPL_to_degrees(f);
M_GEN_IMPL_to_degrees(d);
#endif

#ifdef M_GEN_is_zero
#define M_GEN_IMPL_is_zero(_s,_char) \
M_GEN_is_zero(_s,_char){ \
    return memvcmp((void*)v0,0,sizeof(struct _s##_char)) == 0; \
}

M_GEN_IMPL_is_zero(vec2,i);
M_GEN_IMPL_is_zero(vec3,i);
M_GEN_IMPL_is_zero(vec4,i);

#define M_GEN_IMPL2_is_zero(_s,_char) \
M_GEN_is_zero(_s,_char) { \
    for ( int i = 0; i < sizeof(v0->v)/sizeof(typeof(v0->v[0])); i++ ) { \
        if ( M##_char##FABS(v0->v[i]) >= M##_char##EPSILON ) { \
            return false; \
        } \
    } \
    return true; \
}

M_GEN_IMPL2_is_zero(vec2,f);
M_GEN_IMPL2_is_zero(vec2,d);
M_GEN_IMPL2_is_zero(vec3,f);
M_GEN_IMPL2_is_zero(vec3,d);
M_GEN_IMPL2_is_zero(vec4,f);
M_GEN_IMPL2_is_zero(vec4,d);
M_GEN_IMPL2_is_zero(quat,f);
M_GEN_IMPL2_is_zero(quat,d);
#endif

#ifdef M_GEN_is_equal
#define M_GEN_IMPL_is_equal(_s,_char) \
M_GEN_is_equal(_s,_char){ \
    return memcmp((void*)v0, (void*)v1, sizeof(struct _s##_char)) == 0; \
}

M_GEN_IMPL_is_equal(vec2,i);
M_GEN_IMPL_is_equal(vec3,i);
M_GEN_IMPL_is_equal(vec4,i);

#define M_GEN_IMPL2_is_equal(_s,_char)                                      \
M_GEN_is_equal(_s,_char) {                                                  \
    for ( int i = 0; i < ELEM_NUM(v0); i++ ) {                              \
        if ( M##_char##FABS(v0->v[i] - v1->v[i]) >= M##_char##EPSILON ) {   \
            return false;                                                   \
        }                                                                   \
    }                                                                       \
    return true;                                                            \
}

M_GEN_IMPL2_is_equal(vec2,f);
M_GEN_IMPL2_is_equal(vec2,d);
M_GEN_IMPL2_is_equal(vec3,f);
M_GEN_IMPL2_is_equal(vec3,d);
M_GEN_IMPL2_is_equal(vec4,f);
M_GEN_IMPL2_is_equal(vec4,d);
M_GEN_IMPL2_is_equal(quat,f);
M_GEN_IMPL2_is_equal(quat,d);
#endif

#ifdef M_GEN_one
#define M_GEN_IMPL_one(_s,_char) \
M_GEN_one(_s,_char) { \
    for ( int i = 0; i < ELEM_NUM(result); i++ ) { \
        result->v[i] = 1; \
    } \
    return result; \
}

M_GEN_IMPL_one(vec2,i);
M_GEN_IMPL_one(vec2,f);
M_GEN_IMPL_one(vec2,d);
M_GEN_IMPL_one(vec3,i);
M_GEN_IMPL_one(vec3,f);
M_GEN_IMPL_one(vec3,d);
M_GEN_IMPL_one(vec4,i);
M_GEN_IMPL_one(vec4,f);
M_GEN_IMPL_one(vec4,d);
#endif

#ifdef M_GEN_sign
#define M_GEN_IMPL_sign(_s,_char) \
M_GEN_sign(_s,_char) { \
    for ( int i = 0; i < ELEM_NUM(result); i++ ){ \
        if (v0->v[i] >  (m##_char##_t)(0.0)) { \
            result->v[i] = (m##_char##_t)(1.0); \
        } else if (v0->v[i] < 0) { \
            result->v[i] = (m##_char##_t)(-1.0); \
        } else { \
            result->v[i] = (m##_char##_t)(0.0); \
        } \
    } \
    return result; \
}

M_GEN_IMPL_sign(vec2,i);
M_GEN_IMPL_sign(vec2,f);
M_GEN_IMPL_sign(vec2,d);
M_GEN_IMPL_sign(vec3,i);
M_GEN_IMPL_sign(vec3,f);
M_GEN_IMPL_sign(vec3,d);
M_GEN_IMPL_sign(vec4,i);
M_GEN_IMPL_sign(vec4,f);
M_GEN_IMPL_sign(vec4,d);
#endif

#ifdef M_GEN_add
#define M_GEN_IMPL_add(_s,_char) \
M_GEN_add(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] += v0->v[i]; \
    } \
    return result; \
}

M_GEN_IMPL_add(vec2,i);
M_GEN_IMPL_add(vec2,f);
M_GEN_IMPL_add(vec2,d);
M_GEN_IMPL_add(vec3,i);
M_GEN_IMPL_add(vec3,f);
M_GEN_IMPL_add(vec3,d);
M_GEN_IMPL_add(vec4,i);
M_GEN_IMPL_add(vec4,f);
M_GEN_IMPL_add(vec4,d);
#endif

#ifdef M_GEN_add_num
#define M_GEN_IMPL_add_num(_s,_char) \
M_GEN_add_num(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = v0->v[i] + (m##_char##_t)num; \
    } \
    return result; \
}

M_GEN_IMPL_add_num(vec2,i);
M_GEN_IMPL_add_num(vec2,f);
M_GEN_IMPL_add_num(vec2,d);
M_GEN_IMPL_add_num(vec3,i);
M_GEN_IMPL_add_num(vec3,f);
M_GEN_IMPL_add_num(vec3,d);
M_GEN_IMPL_add_num(vec4,i);
M_GEN_IMPL_add_num(vec4,f);
M_GEN_IMPL_add_num(vec4,d);
#endif

#ifdef M_GEN_subtract
#define M_GEN_IMPL_subtract(_s,_char) \
M_GEN_subtract(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] -= v0->v[i]; \
    } \
    return result; \
}

M_GEN_IMPL_subtract(vec2,i);
M_GEN_IMPL_subtract(vec2,f);
M_GEN_IMPL_subtract(vec2,d);
M_GEN_IMPL_subtract(vec3,i);
M_GEN_IMPL_subtract(vec3,f);
M_GEN_IMPL_subtract(vec3,d);
M_GEN_IMPL_subtract(vec4,i);
M_GEN_IMPL_subtract(vec4,f);
M_GEN_IMPL_subtract(vec4,d);
#endif

#ifdef M_GEN_subtract_num
#define M_GEN_IMPL_subtract_num(_s,_char) \
M_GEN_subtract_num(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = v0->v[i] - (m##_char##_t)num; \
    } \
    return result; \
}

M_GEN_IMPL_subtract_num(vec2,i);
M_GEN_IMPL_subtract_num(vec2,f);
M_GEN_IMPL_subtract_num(vec2,d);
M_GEN_IMPL_subtract_num(vec3,i);
M_GEN_IMPL_subtract_num(vec3,f);
M_GEN_IMPL_subtract_num(vec3,d);
M_GEN_IMPL_subtract_num(vec4,i);
M_GEN_IMPL_subtract_num(vec4,f);
M_GEN_IMPL_subtract_num(vec4,d);
#endif

#ifdef M_GEN_multiply
#define M_GEN_IMPL_vec_multiply(_s,_char) \
M_GEN_multiply(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = a0->v[i] * a1->v[i]; \
    } \
    return result; \
}

M_GEN_IMPL_vec_multiply(vec2,i);
M_GEN_IMPL_vec_multiply(vec2,f);
M_GEN_IMPL_vec_multiply(vec2,d);
M_GEN_IMPL_vec_multiply(vec3,i);
M_GEN_IMPL_vec_multiply(vec3,f);
M_GEN_IMPL_vec_multiply(vec3,d);
M_GEN_IMPL_vec_multiply(vec4,i);
M_GEN_IMPL_vec_multiply(vec4,f);
M_GEN_IMPL_vec_multiply(vec4,d);

#define M_GEN_IMPL_mat2_multiply(_s,_char) \
M_GEN_multiply(_s,_char){ \
    struct _s##_char multiplied; \
    multiplied.v[0] = a0->v[0] * a1->v[0] + a0->v[2] * a1->v[1]; \
    multiplied.v[1] = a0->v[1] * a1->v[0] + a0->v[3] * a1->v[1]; \
    multiplied.v[2] = a0->v[0] * a1->v[2] + a0->v[2] * a1->v[3]; \
    multiplied.v[3] = a0->v[1] * a1->v[2] + a0->v[3] * a1->v[3]; \
    result->v[0] = multiplied.v[0]; \
    result->v[1] = multiplied.v[1]; \
    result->v[2] = multiplied.v[2]; \
    result->v[3] = multiplied.v[3]; \
    return result; \
}

M_GEN_IMPL_mat2_multiply(mat2,f);
M_GEN_IMPL_mat2_multiply(mat2,d);

#define M_GEN_IMPL_mat3_multiply(_s,_char) \
M_GEN_multiply(_s,_char){ \
    struct _s##_char multiplied; \
    multiplied.v[0] = a0->v[0] * a1->v[0] + a0->v[3] * a1->v[1] + a0->v[6] * a1->v[2]; \
    multiplied.v[1] = a0->v[1] * a1->v[0] + a0->v[4] * a1->v[1] + a0->v[7] * a1->v[2]; \
    multiplied.v[2] = a0->v[2] * a1->v[0] + a0->v[5] * a1->v[1] + a0->v[8] * a1->v[2]; \
    multiplied.v[3] = a0->v[0] * a1->v[3] + a0->v[3] * a1->v[4] + a0->v[6] * a1->v[5]; \
    multiplied.v[4] = a0->v[1] * a1->v[3] + a0->v[4] * a1->v[4] + a0->v[7] * a1->v[5]; \
    multiplied.v[5] = a0->v[2] * a1->v[3] + a0->v[5] * a1->v[4] + a0->v[8] * a1->v[5]; \
    multiplied.v[6] = a0->v[0] * a1->v[6] + a0->v[3] * a1->v[7] + a0->v[6] * a1->v[8]; \
    multiplied.v[7] = a0->v[1] * a1->v[6] + a0->v[4] * a1->v[7] + a0->v[7] * a1->v[8]; \
    multiplied.v[8] = a0->v[2] * a1->v[6] + a0->v[5] * a1->v[7] + a0->v[8] * a1->v[8]; \
    result->v[0] = multiplied.v[0]; \
    result->v[1] = multiplied.v[1]; \
    result->v[2] = multiplied.v[2]; \
    result->v[3] = multiplied.v[3]; \
    result->v[4] = multiplied.v[4]; \
    result->v[5] = multiplied.v[5]; \
    result->v[6] = multiplied.v[6]; \
    result->v[7] = multiplied.v[7]; \
    result->v[8] = multiplied.v[8]; \
    return result; \
}

M_GEN_IMPL_mat3_multiply(mat3,f);
M_GEN_IMPL_mat3_multiply(mat3,d);

#define M_GEN_IMPL_mat4_multiply(_s,_char) \
M_GEN_multiply(_s,_char){ \
    struct _s##_char multiplied; \
    multiplied.v[0]  = a0->v[0] * a1->v[0]  + a0->v[4] * a1->v[1]  + a0->v[8]  * a1->v[2]  + a0->v[12] * a1->v[3]; \
    multiplied.v[1]  = a0->v[1] * a1->v[0]  + a0->v[5] * a1->v[1]  + a0->v[9]  * a1->v[2]  + a0->v[13] * a1->v[3]; \
    multiplied.v[2]  = a0->v[2] * a1->v[0]  + a0->v[6] * a1->v[1]  + a0->v[10] * a1->v[2]  + a0->v[14] * a1->v[3]; \
    multiplied.v[3]  = a0->v[3] * a1->v[0]  + a0->v[7] * a1->v[1]  + a0->v[11] * a1->v[2]  + a0->v[15] * a1->v[3]; \
    multiplied.v[4]  = a0->v[0] * a1->v[4]  + a0->v[4] * a1->v[5]  + a0->v[8]  * a1->v[6]  + a0->v[12] * a1->v[7]; \
    multiplied.v[5]  = a0->v[1] * a1->v[4]  + a0->v[5] * a1->v[5]  + a0->v[9]  * a1->v[6]  + a0->v[13] * a1->v[7]; \
    multiplied.v[6]  = a0->v[2] * a1->v[4]  + a0->v[6] * a1->v[5]  + a0->v[10] * a1->v[6]  + a0->v[14] * a1->v[7]; \
    multiplied.v[7]  = a0->v[3] * a1->v[4]  + a0->v[7] * a1->v[5]  + a0->v[11] * a1->v[6]  + a0->v[15] * a1->v[7]; \
    multiplied.v[8]  = a0->v[0] * a1->v[8]  + a0->v[4] * a1->v[9]  + a0->v[8]  * a1->v[10] + a0->v[12] * a1->v[11]; \
    multiplied.v[9]  = a0->v[1] * a1->v[8]  + a0->v[5] * a1->v[9]  + a0->v[9]  * a1->v[10] + a0->v[13] * a1->v[11]; \
    multiplied.v[10] = a0->v[2] * a1->v[8]  + a0->v[6] * a1->v[9]  + a0->v[10] * a1->v[10] + a0->v[14] * a1->v[11]; \
    multiplied.v[11] = a0->v[3] * a1->v[8]  + a0->v[7] * a1->v[9]  + a0->v[11] * a1->v[10] + a0->v[15] * a1->v[11]; \
    multiplied.v[12] = a0->v[0] * a1->v[12] + a0->v[4] * a1->v[13] + a0->v[8]  * a1->v[14] + a0->v[12] * a1->v[15]; \
    multiplied.v[13] = a0->v[1] * a1->v[12] + a0->v[5] * a1->v[13] + a0->v[9]  * a1->v[14] + a0->v[13] * a1->v[15]; \
    multiplied.v[14] = a0->v[2] * a1->v[12] + a0->v[6] * a1->v[13] + a0->v[10] * a1->v[14] + a0->v[14] * a1->v[15]; \
    multiplied.v[15] = a0->v[3] * a1->v[12] + a0->v[7] * a1->v[13] + a0->v[11] * a1->v[14] + a0->v[15] * a1->v[15]; \
    result->v[0]  = multiplied.v[0]; \
    result->v[1]  = multiplied.v[1]; \
    result->v[2]  = multiplied.v[2]; \
    result->v[3]  = multiplied.v[3]; \
    result->v[4]  = multiplied.v[4]; \
    result->v[5]  = multiplied.v[5]; \
    result->v[6]  = multiplied.v[6]; \
    result->v[7]  = multiplied.v[7]; \
    result->v[8]  = multiplied.v[8]; \
    result->v[9]  = multiplied.v[9]; \
    result->v[10] = multiplied.v[10]; \
    result->v[11] = multiplied.v[11]; \
    result->v[12] = multiplied.v[12]; \
    result->v[13] = multiplied.v[13]; \
    result->v[14] = multiplied.v[14]; \
    result->v[15] = multiplied.v[15]; \
    return result; \
}

M_GEN_IMPL_mat4_multiply(mat4,f);
M_GEN_IMPL_mat4_multiply(mat4,d);

#define M_GEN_IMPL_quat_multiply(_s,_char) \
M_GEN_multiply(_s,_char){ \
    result->v[0] = a0->v[3] * a1->v[0] + a0->v[0] * a1->v[3] + a0->v[1] * a1->v[2] - a0->v[2] * a1->v[1]; \
    result->v[1] = a0->v[3] * a1->v[1] + a0->v[1] * a1->v[3] + a0->v[2] * a1->v[0] - a0->v[0] * a1->v[2]; \
    result->v[2] = a0->v[3] * a1->v[2] + a0->v[2] * a1->v[3] + a0->v[0] * a1->v[1] - a0->v[1] * a1->v[0]; \
    result->v[3] = a0->v[3] * a1->v[3] - a0->v[0] * a1->v[0] - a0->v[1] * a1->v[1] - a0->v[2] * a1->v[2]; \
    return result; \
}

M_GEN_IMPL_quat_multiply(quat,f);
M_GEN_IMPL_quat_multiply(quat,d);
#endif

#ifdef M_GEN_multiply_num
#define M_GEN_IMPL_multiply_num(_s,_char) \
M_GEN_multiply_num(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = v0->v[i] * (m##_char##_t)num; \
    } \
    return result; \
}

M_GEN_IMPL_multiply_num(vec2,i);
M_GEN_IMPL_multiply_num(vec2,f);
M_GEN_IMPL_multiply_num(vec2,d);
M_GEN_IMPL_multiply_num(vec3,i);
M_GEN_IMPL_multiply_num(vec3,f);
M_GEN_IMPL_multiply_num(vec3,d);
M_GEN_IMPL_multiply_num(vec4,i);
M_GEN_IMPL_multiply_num(vec4,f);
M_GEN_IMPL_multiply_num(vec4,d);
M_GEN_IMPL_multiply_num(mat2,f);
M_GEN_IMPL_multiply_num(mat2,d);
M_GEN_IMPL_multiply_num(mat3,f);
M_GEN_IMPL_multiply_num(mat3,d);
M_GEN_IMPL_multiply_num(mat4,f);
M_GEN_IMPL_multiply_num(mat4,d);
M_GEN_IMPL_multiply_num(quat,f);
M_GEN_IMPL_multiply_num(quat,d);
#endif

#ifdef M_GEN_divide
#define M_GEN_IMPL_divide(_s,_char) \
M_GEN_divide(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = v0->v[i] / v1->v[i]; \
    } \
    return result; \
}

M_GEN_IMPL_divide(vec2,i);
M_GEN_IMPL_divide(vec2,f);
M_GEN_IMPL_divide(vec2,d);
M_GEN_IMPL_divide(vec3,i);
M_GEN_IMPL_divide(vec3,f);
M_GEN_IMPL_divide(vec3,d);
M_GEN_IMPL_divide(vec4,i);
M_GEN_IMPL_divide(vec4,f);
M_GEN_IMPL_divide(vec4,d);
#endif

#ifdef M_GEN_divide_num
#define M_GEN_IMPL_divide_num(_s,_char) \
M_GEN_divide_num(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = v0->v[i] / (m##_char##_t)num; \
    } \
    return result; \
}

M_GEN_IMPL_divide_num(vec2,i);
M_GEN_IMPL_divide_num(vec2,f);
M_GEN_IMPL_divide_num(vec2,d);
M_GEN_IMPL_divide_num(vec3,i);
M_GEN_IMPL_divide_num(vec3,f);
M_GEN_IMPL_divide_num(vec3,d);
M_GEN_IMPL_divide_num(vec4,i);
M_GEN_IMPL_divide_num(vec4,f);
M_GEN_IMPL_divide_num(vec4,d);
M_GEN_IMPL_divide_num(quat,f);
M_GEN_IMPL_divide_num(quat,d);
#endif

#ifdef M_GEN_snap
#define M_GEN_IMPL_snap(_s,_char)                                           \
M_GEN_snap(_s,_char) {                                                      \
    for ( int i=0; i < ELEM_NUM(result); i++ ){                             \
        result->v[i] = M##_char##FLOOR(v0->v[i] / v1->v[i]) * v1->v[i];     \
    }                                                                       \
    return result;                                                          \
}

M_GEN_IMPL_snap(vec2,f);
M_GEN_IMPL_snap(vec2,d);
M_GEN_IMPL_snap(vec3,f);
M_GEN_IMPL_snap(vec3,d);
M_GEN_IMPL_snap(vec4,f);
M_GEN_IMPL_snap(vec4,d);
#endif

#ifdef M_GEN_snap_num
#define M_GEN_IMPL_snap_num(_s,_char)                                                       \
M_GEN_snap_num(_s,_char) {                                                                  \
    for ( int i=0; i < ELEM_NUM(result); i++ ){                                             \
        result->v[i] = M##_char##FLOOR(v0->v[i] / (m##_char##_t)num) * (m##_char##_t)num;   \
    }                                                                                       \
    return result;                                                                          \
}

M_GEN_IMPL_snap_num(vec2,f);
M_GEN_IMPL_snap_num(vec2,d);
M_GEN_IMPL_snap_num(vec3,f);
M_GEN_IMPL_snap_num(vec3,d);
M_GEN_IMPL_snap_num(vec4,f);
M_GEN_IMPL_snap_num(vec4,d);
#endif

#ifdef M_GEN_negative
#define M_GEN_IMPL_negative(_s,_char) \
M_GEN_negative(_s,_char) { \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = -v0->v[i]; \
    } \
    return result; \
}

M_GEN_IMPL_negative(vec2,f);
M_GEN_IMPL_negative(vec2,d);
M_GEN_IMPL_negative(vec2,i);
M_GEN_IMPL_negative(vec3,f);
M_GEN_IMPL_negative(vec3,d);
M_GEN_IMPL_negative(vec3,i);
M_GEN_IMPL_negative(vec4,f);
M_GEN_IMPL_negative(vec4,d);
M_GEN_IMPL_negative(vec4,i);
M_GEN_IMPL_negative(quat,f);
M_GEN_IMPL_negative(quat,d);
M_GEN_IMPL_negative(mat2,f);
M_GEN_IMPL_negative(mat2,d);
M_GEN_IMPL_negative(mat3,f);
M_GEN_IMPL_negative(mat3,d);
M_GEN_IMPL_negative(mat4,f);
M_GEN_IMPL_negative(mat4,d);
#endif

#ifdef M_GEN_abs
#define M_GEN_IMPL_abs(_s,_char) \
M_GEN_abs(_s,_char) { \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = v0->v[i]; \
        if (result->v[i] < 0) { \
            result->v[i] = -result->v[i]; \
        } \
    } \
    return result; \
}

M_GEN_IMPL_abs(vec2,i);
M_GEN_IMPL_abs(vec3,i);
M_GEN_IMPL_abs(vec4,i);

#define M_GEN_IMPL2_abs(_s,_char)                   \
M_GEN_abs(_s,_char) {                               \
    for ( int i=0; i < ELEM_NUM(result); i++ ){     \
        result->v[i] = M##_char##FABS(v0->v[i]);    \
    }                                               \
    return result;                                  \
}

M_GEN_IMPL2_abs(vec2,f);
M_GEN_IMPL2_abs(vec2,d);
M_GEN_IMPL2_abs(vec3,f);
M_GEN_IMPL2_abs(vec3,d);
M_GEN_IMPL2_abs(vec4,f);
M_GEN_IMPL2_abs(vec4,d);
#endif

#ifdef M_GEN_max
#define M_GEN_IMPL_max(_s,_char) \
M_GEN_max(_s,_char) { \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        if (v0->v[i] > v1->v[i]) { \
            result->v[i] = v0->v[i]; \
        } else { \
            result->v[i] = v1->v[i]; \
        } \
    } \
    return result; \
}

M_GEN_IMPL_max(vec2,i);
M_GEN_IMPL_max(vec3,i);
M_GEN_IMPL_max(vec4,i);

#define M_GEN_IMPL2_max(_s,_char) \
M_GEN_max(_s,_char) { \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = M##_char##FMAX(v0->v[i], v1->v[i]); \
    } \
    return result; \
}

M_GEN_IMPL2_max(vec2,f);
M_GEN_IMPL2_max(vec2,d);
M_GEN_IMPL2_max(vec3,f);
M_GEN_IMPL2_max(vec3,d);
M_GEN_IMPL2_max(vec4,f);
M_GEN_IMPL2_max(vec4,d);
#endif

#ifdef M_GEN_min
#define M_GEN_IMPL_min(_s,_char) \
M_GEN_min(_s,_char) { \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        if (v0->v[i] < v1->v[i]) { \
            result->v[i] = v0->v[i]; \
        } else { \
            result->v[i] = v1->v[i]; \
        } \
    } \
    return result; \
}

M_GEN_IMPL_min(vec2,i);
M_GEN_IMPL_min(vec3,i);
M_GEN_IMPL_min(vec4,i);

#define M_GEN_IMPL2_min(_s,_char) \
M_GEN_min(_s,_char) { \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = M##_char##FMIN(v0->v[i], v1->v[i]); \
    } \
    return result; \
}

M_GEN_IMPL2_min(vec2,f);
M_GEN_IMPL2_min(vec2,d);
M_GEN_IMPL2_min(vec3,f);
M_GEN_IMPL2_min(vec3,d);
M_GEN_IMPL2_min(vec4,f);
M_GEN_IMPL2_min(vec4,d);
#endif

#ifdef M_GEN_vec_clamp
#define M_GEN_IMPL_vec_clamp(_s,_char) \
M_GEN_vec_clamp(_s,_char) { \
    _s##_char##_min(result, v0, v1); \
    _s##_char##_max(result, v0, v1); \
    return result; \
}

M_GEN_IMPL_vec_clamp(vec2,f);
M_GEN_IMPL_vec_clamp(vec2,d);
M_GEN_IMPL_vec_clamp(vec2,i);
M_GEN_IMPL_vec_clamp(vec3,f);
M_GEN_IMPL_vec_clamp(vec3,d);
M_GEN_IMPL_vec_clamp(vec3,i);
M_GEN_IMPL_vec_clamp(vec4,f);
M_GEN_IMPL_vec_clamp(vec4,d);
M_GEN_IMPL_vec_clamp(vec4,i);
#endif

#ifdef M_GEN_tangent
#define M_GEN_IMPL_tangent(_s,_char) \
M_GEN_tangent(_s,_char) { \
    m##_char##_t a0 = v0->v[0]; \
    m##_char##_t a1 = v0->v[1]; \
    result->v[0] = a1; \
    result->v[1] = -a0; \
    return result; \
}

M_GEN_IMPL_tangent(vec2,i);
M_GEN_IMPL_tangent(vec2,f);
M_GEN_IMPL_tangent(vec2,d);
#endif

#ifdef M_GEN_cross
#define M_GEN_IMPL_cross(_s,_char)                          \
M_GEN_cross(_s,_char){                                      \
    m##_char##_t cross[VEC3_SIZE];                          \
    cross[0] = v0->v[1] * v1->v[2] - v0->v[2] * v1->v[1];   \
    cross[1] = v0->v[2] * v1->v[0] - v0->v[0] * v1->v[2];   \
    cross[2] = v0->v[0] * v1->v[1] - v0->v[1] * v1->v[0];   \
    memcpy(result,cross,sizeof(m##_char##_t)*VEC3_SIZE);    \
    return result;                                          \
}

M_GEN_IMPL_cross(vec3,i);
M_GEN_IMPL_cross(vec3,f);
M_GEN_IMPL_cross(vec3,d);
#endif

#ifdef M_GEN_multiply_mat2
#define M_GEN_IMPL_multiply_mat2(_s,_char) \
M_GEN_multiply_mat2(_s,_char){ \
    m##_char##_t x = v0[0]; \
    m##_char##_t y = v0[1]; \
    result->v[0] = m0->v[0] * x + m0->v[2] * y; \
    result->v[1] = m0->v[1] * x + m0->v[3] * y; \
    return result; \
}

M_GEN_multiply_mat2(vec2,f);
M_GEN_multiply_mat2(vec2,d);
#endif

#ifdef M_GEN_floor
#define M_GEN_IMPL_floor(_s,_char) \
M_GEN_floor(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = M##_char##FLOOR(v0->v[i]); \
 \
    } \
    return result; \
}

M_GEN_IMPL_floor(vec2,f);
M_GEN_IMPL_floor(vec2,d);
M_GEN_IMPL_floor(vec3,f);
M_GEN_IMPL_floor(vec3,d);
M_GEN_IMPL_floor(vec4,f);
M_GEN_IMPL_floor(vec4,d);
#endif

#ifdef M_GEN_ceil
#define M_GEN_IMPL_ceil(_s,_char) \
M_GEN_ceil(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = M##_char##CEIL(v0->v[i]); \
 \
    } \
    return result; \
}

M_GEN_IMPL_ceil(vec2,f);
M_GEN_IMPL_ceil(vec2,d);
M_GEN_IMPL_ceil(vec3,f);
M_GEN_IMPL_ceil(vec3,d);
M_GEN_IMPL_ceil(vec4,f);
M_GEN_IMPL_ceil(vec4,d);
#endif

#ifdef M_GEN_round
#define M_GEN_IMPL_round(_s,_char) \
M_GEN_round(_s,_char){ \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = M##_char##ROUND(v0->v[i]); \
    } \
    return result; \
}

M_GEN_IMPL_round(vec2,f);
M_GEN_IMPL_round(vec2,d);
M_GEN_IMPL_round(vec3,f);
M_GEN_IMPL_round(vec3,d);
M_GEN_IMPL_round(vec4,f);
M_GEN_IMPL_round(vec4,d);
#endif

#ifdef M_GEN_length_squared
#define M_GEN_IMPL_length_squared(_s,_char) \
M_GEN_length_squared(_s,_char){ \
    m##_char##_t sum = 0; \
    for ( int i=0; i < ELEM_NUM(v0); i++ ){ \
        sum += v0->v[i] * v0->v[i]; \
    } \
    return sum; \
}

M_GEN_IMPL_length_squared(vec2,f);
M_GEN_IMPL_length_squared(vec2,d);
M_GEN_IMPL_length_squared(vec3,f);
M_GEN_IMPL_length_squared(vec3,d);
M_GEN_IMPL_length_squared(vec4,f);
M_GEN_IMPL_length_squared(vec4,d);
M_GEN_IMPL_length_squared(quat,f);
M_GEN_IMPL_length_squared(quat,d);
#endif

#ifdef M_GEN_length
#define M_GEN_IMPL_length(_s,_char)                         \
M_GEN_length(_s,_char){                                     \
    return M##_char##SQRT(_s##_char##_length_squared(v0));  \
}

M_GEN_IMPL_length(vec2,f);
M_GEN_IMPL_length(vec2,d);
M_GEN_IMPL_length(vec3,f);
M_GEN_IMPL_length(vec3,d);
M_GEN_IMPL_length(vec4,f);
M_GEN_IMPL_length(vec4,d);
M_GEN_IMPL_length(quat,f);
M_GEN_IMPL_length(quat,d);
#endif

#ifdef M_GEN_normalize
#define M_GEN_IMPL_normalize(_s,_char) \
M_GEN_normalize(_s,_char){ \
    m##_char##_t l = _s##_char##_length(v0); \
    for ( int i=0; i < ELEM_NUM(result); i++ ){ \
        result->v[i] = v0->v[i] / l; \
    } \
    return result; \
}

M_GEN_IMPL_normalize(vec2,f);
M_GEN_IMPL_normalize(vec2,d);
M_GEN_IMPL_normalize(vec3,f);
M_GEN_IMPL_normalize(vec3,d);
M_GEN_IMPL_normalize(vec4,f);
M_GEN_IMPL_normalize(vec4,d);
#endif

#ifdef M_GEN_dot
#define M_GEN_IMPL_dot(_s,_char) \
M_GEN_dot(_s,_char) { \
    m##_char##_t sum = 0; \
    for ( int i=0; i < ELEM_NUM(v0); i++ ){ \
        sum += v0->v[i] * v1->v[i]; \
    } \
    return sum; \
}

M_GEN_IMPL_dot(vec2,i);
M_GEN_IMPL_dot(vec2,f);
M_GEN_IMPL_dot(vec2,d);
M_GEN_IMPL_dot(vec3,i);
M_GEN_IMPL_dot(vec3,f);
M_GEN_IMPL_dot(vec3,d);
M_GEN_IMPL_dot(vec4,i);
M_GEN_IMPL_dot(vec4,f);
M_GEN_IMPL_dot(vec4,d);
M_GEN_IMPL_dot(quat,f);
M_GEN_IMPL_dot(quat,d);
#endif

#ifdef M_GEN_project
#define M_GEN_IMPL_project(_s,_char) \
M_GEN_project(_s,_char) { \
    m##_char##_t d = _s##_char##_dot(v1, v1); \
    m##_char##_t s = _s##_char##_dot(v0, v1) / d; \
    for ( int i=0; i < ELEM_NUM(v0); i++ ){ \
        result->v[i] = v1->v[i] * s; \
    } \
    return result; \
}

M_GEN_IMPL_project(vec2,f);
M_GEN_IMPL_project(vec2,d);
M_GEN_IMPL_project(vec3,f);
M_GEN_IMPL_project(vec3,d);
M_GEN_IMPL_project(vec4,f);
M_GEN_IMPL_project(vec4,d);
#endif

#ifdef M_GEN_slide
#define M_GEN_IMPL_slide(_s,_char) \
M_GEN_slide(_s,_char) { \
    m##_char##_t d = _s##_char##_dot(v0, normal); \
    for ( int i=0; i < ELEM_NUM(v0); i++ ){ \
        result->v[i] = v0->v[i] - normal->v[i] * d; \
    } \
    return result; \
}

M_GEN_IMPL_slide(vec2,f);
M_GEN_IMPL_slide(vec2,d);
M_GEN_IMPL_slide(vec3,f);
M_GEN_IMPL_slide(vec3,d);
M_GEN_IMPL_slide(vec4,f);
M_GEN_IMPL_slide(vec4,d);
#endif

#ifdef M_GEN_reflect
#define M_GEN_IMPL_reflect(_s,_char)                                    \
M_GEN_reflect(_s,_char) {                                               \
    m##_char##_t d = (m##_char##_t)2.0 * _s##_char##_dot(v0, normal);   \
    for ( int i=0; i < ELEM_NUM(v0); i++ ){                             \
        result->v[i] = normal->v[i] * d - v0->v[i];                     \
    }                                                                   \
    return result;                                                      \
}

M_GEN_IMPL_reflect(vec2,f);
M_GEN_IMPL_reflect(vec2,d);
M_GEN_IMPL_reflect(vec3,f);
M_GEN_IMPL_reflect(vec3,d);
M_GEN_IMPL_reflect(vec4,f);
M_GEN_IMPL_reflect(vec4,d);
#endif

#ifdef M_GEN_vec2_rotate
#define M_GEN_IMPL_vec2_rotate(_char)   \
M_GEN_vec2_rotate(_char){               \
    m##_char##_t cs = M##_char##COS(f); \
    m##_char##_t sn = M##_char##SIN(f); \
    m##_char##_t x = v0->v[0];          \
    m##_char##_t y = v0->v[1];          \
    result->v[0] = x * cs - y * sn;     \
    result->v[1] = x * sn + y * cs;     \
    return result;                      \
}

M_GEN_IMPL_vec2_rotate(f);
M_GEN_IMPL_vec2_rotate(d);
#endif

#ifdef M_GEN_vec3_rotate
#define M_GEN_IMPL_vec3_rotate(_char) \
M_GEN_vec3_rotate(_char){ \
    m##_char##_t cs; \
    m##_char##_t sn; \
    m##_char##_t x; \
    m##_char##_t y; \
    m##_char##_t z; \
    m##_char##_t rx; \
    m##_char##_t ry; \
    m##_char##_t rz; \
    cs = M##_char##COS(f); \
    sn = M##_char##SIN(f); \
    x = v0->v[0]; \
    y = v0->v[1]; \
    z = v0->v[2]; \
    struct vec3##_char norm; \
    vec3##_char##_normalize(&norm, ra); \
    rx = norm.x; \
    ry = norm.y; \
    rz = norm.z; \
    result->v[0] = x * (cs + rx * rx * (1 - cs)) + y * (rx * ry * (1 - cs) - rz * sn) + z * (rx * rz * (1 - cs) + ry * sn); \
    result->v[1] = x * (ry * rx * (1 - cs) + rz * sn) + y * (cs + ry * ry * (1 - cs)) + z * (ry * rz * (1 - cs) - rx * sn); \
    result->v[2] = x * (rz * rx * (1 - cs) - ry * sn) + y * (rz * ry * (1 - cs) + rx * sn) + z * (cs + rz * rz * (1 - cs)); \
    return result; \
}

M_GEN_IMPL_vec3_rotate(f);
M_GEN_IMPL_vec3_rotate(d);
#endif

#ifdef M_GEN_vec3i_rotate
M_GEN_vec3i_rotate {
    #define M_EL(r,c) r*3+c
    const mi_t* matrix = (const mi_t*)&ortho_bases[cv];

    result->v[M_X] = matrix[M_EL(M_X,M_X)] * v->v[M_X] +
                     matrix[M_EL(M_Y,M_X)] * v->v[M_Y] +
                     matrix[M_EL(M_Z,M_X)] * v->v[M_Z];
    result->v[M_Y] = matrix[M_EL(M_X,M_Y)] * v->v[M_X] +
                     matrix[M_EL(M_Y,M_Y)] * v->v[M_Y] +
                     matrix[M_EL(M_Z,M_Y)] * v->v[M_Z];
    result->v[M_Z] = matrix[M_EL(M_X,M_Z)] * v->v[M_X] +
                     matrix[M_EL(M_Y,M_Z)] * v->v[M_Y] +
                     matrix[M_EL(M_Z,M_Z)] * v->v[M_Z];
    return result;
}
#endif

#ifdef M_GEN_lerp
#define M_GEN_IMPL_vec_lerp(_s,_char) \
M_GEN_lerp(_s,_char) { \
    for ( int i=0; i < ELEM_NUM(a0); i++ ){ \
        result->v[i] = a0->v[i] + (a1->v[i] - a0->v[i]) * f; \
    } \
    return result; \
}

M_GEN_IMPL_vec_lerp(vec2,f);
M_GEN_IMPL_vec_lerp(vec2,d);
M_GEN_IMPL_vec_lerp(vec3,f);
M_GEN_IMPL_vec_lerp(vec3,d);
M_GEN_IMPL_vec_lerp(vec4,f);
M_GEN_IMPL_vec_lerp(vec4,d);
M_GEN_IMPL_vec_lerp(quat,f);
M_GEN_IMPL_vec_lerp(quat,d);

#define M_GEN_IMPL_mat2_lerp(_s,_char) \
M_GEN_lerp(_s,_char){ \
    result->v[0] = a0->v[0] + (a1->v[0] - a0->v[0]) * f; \
    result->v[1] = a0->v[1] + (a1->v[1] - a0->v[1]) * f; \
    result->v[2] = a0->v[2] + (a1->v[2] - a0->v[2]) * f; \
    result->v[3] = a0->v[3] + (a1->v[3] - a0->v[3]) * f; \
    return result; \
}

M_GEN_IMPL_mat2_lerp(mat2,f);
M_GEN_IMPL_mat2_lerp(mat2,d);

#define M_GEN_IMPL_mat3_lerp(_s,_char) \
M_GEN_lerp(_s,_char){ \
    result->v[0] = a0->v[0] + (a1->v[0] - a0->v[0]) * f; \
    result->v[1] = a0->v[1] + (a1->v[1] - a0->v[1]) * f; \
    result->v[2] = a0->v[2] + (a1->v[2] - a0->v[2]) * f; \
    result->v[3] = a0->v[3] + (a1->v[3] - a0->v[3]) * f; \
    result->v[4] = a0->v[4] + (a1->v[4] - a0->v[4]) * f; \
    result->v[5] = a0->v[5] + (a1->v[5] - a0->v[5]) * f; \
    result->v[6] = a0->v[6] + (a1->v[6] - a0->v[6]) * f; \
    result->v[7] = a0->v[7] + (a1->v[7] - a0->v[7]) * f; \
    result->v[8] = a0->v[8] + (a1->v[8] - a0->v[8]) * f; \
    return result; \
}

M_GEN_IMPL_mat3_lerp(mat3,f);
M_GEN_IMPL_mat3_lerp(mat3,d);

#define M_GEN_IMPL_mat4_lerp(_s,_char) \
M_GEN_lerp(_s,_char){ \
    result->v[0]  = a0->v[0]  + (a1->v[0]  - a0->v[0]) * f; \
    result->v[1]  = a0->v[1]  + (a1->v[1]  - a0->v[1]) * f; \
    result->v[2]  = a0->v[2]  + (a1->v[2]  - a0->v[2]) * f; \
    result->v[3]  = a0->v[3]  + (a1->v[3]  - a0->v[3]) * f; \
    result->v[4]  = a0->v[4]  + (a1->v[4]  - a0->v[4]) * f; \
    result->v[5]  = a0->v[5]  + (a1->v[5]  - a0->v[5]) * f; \
    result->v[6]  = a0->v[6]  + (a1->v[6]  - a0->v[6]) * f; \
    result->v[7]  = a0->v[7]  + (a1->v[7]  - a0->v[7]) * f; \
    result->v[8]  = a0->v[8]  + (a1->v[8]  - a0->v[8]) * f; \
    result->v[9]  = a0->v[9]  + (a1->v[9]  - a0->v[9]) * f; \
    result->v[10] = a0->v[10] + (a1->v[10] - a0->v[10]) * f; \
    result->v[11] = a0->v[11] + (a1->v[11] - a0->v[11]) * f; \
    result->v[12] = a0->v[12] + (a1->v[12] - a0->v[12]) * f; \
    result->v[13] = a0->v[13] + (a1->v[13] - a0->v[13]) * f; \
    result->v[14] = a0->v[14] + (a1->v[14] - a0->v[14]) * f; \
    result->v[15] = a0->v[15] + (a1->v[15] - a0->v[15]) * f; \
    return result; \
}

M_GEN_IMPL_mat4_lerp(mat4,f);
M_GEN_IMPL_mat4_lerp(mat4,d);
#endif

#ifdef M_GEN_bezier3
#define M_GEN_IMPL_bezier3(_s,_char) \
M_GEN_bezier3(_s,_char) { \
    struct _s##_char tmp0; \
    struct _s##_char tmp1; \
    _s##_char##_lerp(&tmp0, v0, v1, f); \
    _s##_char##_lerp(&tmp1, v1, v2, f); \
    _s##_char##_lerp(result, &tmp0, &tmp1, f); \
    return result; \
}

M_GEN_IMPL_bezier3(vec2,f);
M_GEN_IMPL_bezier3(vec2,d);
M_GEN_IMPL_bezier3(vec3,f);
M_GEN_IMPL_bezier3(vec3,d);
#endif

#ifdef M_GEN_bezier4
#define M_GEN_IMPL_bezier4(_s,_char) \
M_GEN_bezier4(_s,_char) { \
    struct _s##_char tmp0; \
    struct _s##_char tmp1; \
    struct _s##_char tmp2; \
    struct _s##_char tmp3; \
    struct _s##_char tmp4; \
    _s##_char##_lerp(&tmp0, v0, v1, f); \
    _s##_char##_lerp(&tmp1, v1, v2, f); \
    _s##_char##_lerp(&tmp2, v2, v3, f); \
    _s##_char##_lerp(&tmp3, &tmp0, &tmp1, f); \
    _s##_char##_lerp(&tmp4, &tmp1, &tmp2, f); \
    _s##_char##_lerp(result, &tmp3, &tmp4, f); \
    return result; \
}

M_GEN_IMPL_bezier4(vec2,f);
M_GEN_IMPL_bezier4(vec2,d);
M_GEN_IMPL_bezier4(vec3,f);
M_GEN_IMPL_bezier4(vec3,d);
#endif

#ifdef M_GEN_vec2_angle
#define M_GEN_IMPL_vec2_angle(_char) \
M_GEN_vec2_angle(_char) { \
    return M##_char##ATAN2(v0->v[1], v0->v[0]); \
}

M_GEN_IMPL_vec2_angle(f);
M_GEN_IMPL_vec2_angle(d);
#endif

#ifdef M_GEN_distance_squared
#define M_GEN_IMPL_distance_squared(_reschar,_s,_char) \
M_GEN_distance_squared(_reschar,_s,_char) { \
    m##_reschar##_t sum = 0; \
    m##_reschar##_t subs; \
    for ( int i=0; i < ELEM_NUM(v0); i++ ){ \
        subs = v0->v[i] - v1->v[i]; \
        sum += subs * subs; \
    } \
    return sum; \
}

M_GEN_IMPL_distance_squared(d,vec2,i);
M_GEN_IMPL_distance_squared(f,vec2,f);
M_GEN_IMPL_distance_squared(d,vec2,d);
M_GEN_IMPL_distance_squared(d,vec3,i);
M_GEN_IMPL_distance_squared(f,vec3,f);
M_GEN_IMPL_distance_squared(d,vec3,d);
M_GEN_IMPL_distance_squared(d,vec4,i);
M_GEN_IMPL_distance_squared(f,vec4,f);
M_GEN_IMPL_distance_squared(d,vec4,d);
#endif

#ifdef M_GEN_distance
#define M_GEN_IMPL_distance(_reschar,_s,_char) \
M_GEN_distance(_reschar,_s,_char) { \
    return M##_reschar##SQRT(_s##_char##_distance_squared_##_s##_char(v0,v1)); \
}

M_GEN_IMPL_distance(d,vec2,i);
M_GEN_IMPL_distance(f,vec2,f);
M_GEN_IMPL_distance(d,vec2,d);
M_GEN_IMPL_distance(d,vec3,i);
M_GEN_IMPL_distance(f,vec3,f);
M_GEN_IMPL_distance(d,vec3,d);
M_GEN_IMPL_distance(d,vec4,i);
M_GEN_IMPL_distance(f,vec4,f);
M_GEN_IMPL_distance(d,vec4,d);
#endif

#ifdef M_GEN_triface_distance
#define M_GEN_IMPL_triface_distance(_trichar,_s,_n,_char) \
M_GEN_triface_distance(_trichar,_s,_n,_char) { \
    struct _s##_n##_char tmp_v = *v0; \
    _s##_n##_char##_subtract(&tmp_v, VEC3_CAST(_char,&t0->p[0])); \
    return _s##_n##_char##_dot(VEC3_CAST(_char, &t0->normal), &tmp_v); \
}

M_GEN_IMPL_triface_distance(i,vec,3,i);
M_GEN_IMPL_triface_distance(i,vec,3,f);
M_GEN_IMPL_triface_distance(i,vec,3,d);
#endif

#ifdef M_GEN_line_distance_squared

M_GEN_line_distance_squared {
    double dir_len = vec3d_length(&l0->direction);
    if (dir_len == 0){
        throw(M_ERROR_CALCULATION);
        return -1;
    } else {
        struct vec3d dir_norm = l0->direction;
        vec3d_divide_num(&dir_norm,&dir_norm, dir_len);

        struct vec3d vec_to_point = *VEC3_CAST(d,v0);
        vec3d_subtract(&vec_to_point, VEC3_CAST(d,&l0->point));

        md_t projection = vec3d_dot(&vec_to_point, &dir_norm);
        vec3d_multiply_num(&dir_norm, &dir_norm, projection);
        vec3d_subtract(&vec_to_point, &dir_norm);
        return vec3d_length_squared(&vec_to_point);
    }
}

#endif

#ifdef M_GEN_line_distance

M_GEN_line_distance {
    return MdSQRT(line3i_distance_squared_vec3i(l0,v0));
}

#endif

#ifdef M_GEN_vec2_linear_independent
#define M_GEN_IMPL_vec2_linear_independent(_char) \
M_GEN_vec2_linear_independent(_char) { \
    return (v0->v[0] * v1->v[1] - v1->v[0] * v0->v[1]) != 0; \
}

M_GEN_IMPL_vec2_linear_independent(f);
M_GEN_IMPL_vec2_linear_independent(d);
#endif

#ifdef M_GEN_vec3_linear_independent
#define M_GEN_IMPL_vec3_linear_independent(_char) \
M_GEN_vec3_linear_independent(_char) {  \
    return v0->v[0] * v1->v[1] * v2->v[2] + v0->v[1] * v1->v[2] * v2->v[0] + v0->v[2] * v1->v[0] * v2->v[1] \
        - v0->v[2] * v1->v[1] * v2->v[0] - v0->v[1] * v1->v[0] * v2->v[2] - v0->v[0] * v1->v[2] * v2->v[1]; \
}

M_GEN_IMPL_vec3_linear_independent(f)
M_GEN_IMPL_vec3_linear_independent(d)
#endif


#ifdef M_GEN_multiply_mat3
#define M_GEN_IMPL_multiply_mat3(_s,_char) \
M_GEN_multiply_mat3(_s,_char) { \
    m##_char##_t x = v0->v[0]; \
    m##_char##_t y = v0->v[1]; \
    m##_char##_t z = v0->v[2]; \
    result->v[0] = m0->v[0] * x + m0->v[3] * y + m0->v[6] * z; \
    result->v[1] = m0->v[1] * x + m0->v[4] * y + m0->v[7] * z; \
    result->v[2] = m0->v[2] * x + m0->v[5] * y + m0->v[8] * z; \
    return result; \
}

M_GEN_IMPL_multiply_mat3(vec3,f);
M_GEN_IMPL_multiply_mat3(vec3,d);
#endif

#ifdef M_GEN_multiply_mat4
#define M_GEN_IMPL_multiply_mat4(_s,_char) \
M_GEN_multiply_mat4(_s,_char) { \
    m##_char##_t x = v0->v[0]; \
    m##_char##_t y = v0->v[1]; \
    m##_char##_t z = v0->v[2]; \
    m##_char##_t w = v0->v[3]; \
    result->v[0] = m0->v[0] * x + m0->v[4] * y + m0->v[8] * z + m0->v[12] * w; \
    result->v[1] = m0->v[1] * x + m0->v[5] * y + m0->v[9] * z + m0->v[13] * w; \
    result->v[2] = m0->v[2] * x + m0->v[6] * y + m0->v[10] * z + m0->v[14] * w; \
    result->v[3] = m0->v[3] * x + m0->v[7] * y + m0->v[11] * z + m0->v[15] * w; \
    return result; \
}

M_GEN_IMPL_multiply_mat4(vec4,f);
M_GEN_IMPL_multiply_mat4(vec4,d);
#endif

#ifdef M_GEN_null
#define M_GEN_IMPL_null(_s,_char) \
M_GEN_null(_s,_char){ \
    result->v[0] = (m##_char##_t)(0.0); \
    result->v[1] = (m##_char##_t)(0.0); \
    result->v[2] = (m##_char##_t)(0.0); \
    result->v[3] = (m##_char##_t)(1.0); \
    return result; \
}

M_GEN_IMPL_null(quat,f);
M_GEN_IMPL_null(quat,d);
#endif

#ifdef M_GEN_quat_divide

#define M_GEN_IMPL_quat_divide(_s,_char) \
M_GEN_quat_divide(_s,_char) { \
    struct _s##_char cj; \
    _s##_char##_conjugate(&cj,q1); \
    \
    result->v[0] = q0->w * cj.x + q0->x * cj.w + q0->y * cj.z - q0->z * cj.y; \
    result->v[1] = q0->w * cj.y - q0->x * cj.z + q0->y * cj.w + q0->z * cj.x; \
    result->v[2] = q0->w * cj.z + q0->x * cj.y - q0->y * cj.x + q0->z * cj.w; \
    result->v[3] = q0->w * cj.w - q0->x * cj.x - q0->y * cj.y - q0->z * cj.z; \
    \
    quat##_char##_normalize(result,result); \
    return result; \
}

M_GEN_IMPL_quat_divide(quat,f);
M_GEN_IMPL_quat_divide(quat,d);
#endif

#ifdef M_GEN_quat_conjugate
#define M_GEN_IMPL_quat_conjugate(_s,_char) \
M_GEN_quat_conjugate(_s,_char){ \
    result->v[0] = -q0->v[0]; \
    result->v[1] = -q0->v[1]; \
    result->v[2] = -q0->v[2]; \
    result->v[3] = q0->v[3]; \
    return result; \
}

M_GEN_IMPL_quat_conjugate(quat,f);
M_GEN_IMPL_quat_conjugate(quat,d);
#endif

#ifdef M_GEN_inverse
#define M_GEN_IMPL_quat_inverse(_s,_char) \
M_GEN_inverse(_s,_char){ \
    m##_char##_t l = (m##_char##_t)(1.0) / (a0->v[0] * a0->v[0] + a0->v[1] * a0->v[1] + a0->v[2] * a0->v[2] + a0->v[3] * a0->v[3]); \
    result->v[0] = -a0->v[0] * l; \
    result->v[1] = -a0->v[1] * l; \
    result->v[2] = -a0->v[2] * l; \
    result->v[3] = a0->v[3] * l; \
    return result; \
}

M_GEN_IMPL_quat_inverse(quat,f);
M_GEN_IMPL_quat_inverse(quat,d);

#define M_GEN_IMPL_mat2_inverse(_s,_char) \
M_GEN_inverse(_s,_char){ \
    struct mat2##_char inverse; \
    m##_char##_t det = mat2##_char##_determinant(a0); \
    mat2##_char##_cofactor(&inverse, a0); \
    mat2##_char##_multiply_num(&inverse, &inverse, (m##_char##_t)(1.0) / det); \
    result->v[0] = inverse.v[0]; \
    result->v[1] = inverse.v[1]; \
    result->v[2] = inverse.v[2]; \
    result->v[3] = inverse.v[3]; \
    return result; \
}

M_GEN_IMPL_mat2_inverse(mat2,f);
M_GEN_IMPL_mat2_inverse(mat2,d);

#define M_GEN_IMPL_mat3_inverse(_s,_char) \
M_GEN_inverse(_s,_char){ \
    struct mat3##_char inverse; \
    m##_char##_t inverted_determinant; \
    inverse.v[0] = a0->m22 * a0->m33 - a0->m32 * a0->m23; \
    inverse.v[3] = a0->m13 * a0->m32 - a0->m12 * a0->m33; \
    inverse.v[6] = a0->m12 * a0->m23 - a0->m13 * a0->m22; \
    inverse.v[1] = a0->m23 * a0->m31 - a0->m21 * a0->m33; \
    inverse.v[4] = a0->m11 * a0->m33 - a0->m13 * a0->m31; \
    inverse.v[7] = a0->m21 * a0->m13 - a0->m11 * a0->m23; \
    inverse.v[2] = a0->m21 * a0->m32 - a0->m31 * a0->m22; \
    inverse.v[5] = a0->m31 * a0->m12 - a0->m11 * a0->m32; \
    inverse.v[8] = a0->m11 * a0->m22 - a0->m21 * a0->m12; \
    inverted_determinant = (m##_char##_t)(1.0) / (a0->m11 * inverse.v[0] + a0->m21 * inverse.v[3] + a0->m31 * inverse.v[6]); \
    result->v[0] = inverse.v[0] * inverted_determinant; \
    result->v[1] = inverse.v[1] * inverted_determinant; \
    result->v[2] = inverse.v[2] * inverted_determinant; \
    result->v[3] = inverse.v[3] * inverted_determinant; \
    result->v[4] = inverse.v[4] * inverted_determinant; \
    result->v[5] = inverse.v[5] * inverted_determinant; \
    result->v[6] = inverse.v[6] * inverted_determinant; \
    result->v[7] = inverse.v[7] * inverted_determinant; \
    result->v[8] = inverse.v[8] * inverted_determinant; \
    return result; \
}

M_GEN_IMPL_mat3_inverse(mat3,f);
M_GEN_IMPL_mat3_inverse(mat3,d);

#define M_GEN_IMPL_mat4_inverse(_s,_char) \
M_GEN_inverse(_s,_char){ \
    struct mat4##_char inverse; \
    m##_char##_t inverted_determinant; \
    inverse.v[0] = a0->m22 * a0->m33 * a0->m44 \
        - a0->m22 * a0->m43 * a0->m34 \
        - a0->m23 * a0->m32 * a0->m44 \
        + a0->m23 * a0->m42 * a0->m34 \
        + a0->m24 * a0->m32 * a0->m43 \
        - a0->m24 * a0->m42 * a0->m33; \
    inverse.v[4] = -a0->m12 * a0->m33 * a0->m44 \
        + a0->m12 * a0->m43 * a0->m34 \
        + a0->m13 * a0->m32 * a0->m44 \
        - a0->m13 * a0->m42 * a0->m34 \
        - a0->m14 * a0->m32 * a0->m43 \
        + a0->m14 * a0->m42 * a0->m33; \
    inverse.v[8] = a0->m12 * a0->m23 * a0->m44 \
        - a0->m12 * a0->m43 * a0->m24 \
        - a0->m13 * a0->m22 * a0->m44 \
        + a0->m13 * a0->m42 * a0->m24 \
        + a0->m14 * a0->m22 * a0->m43 \
        - a0->m14 * a0->m42 * a0->m23; \
    inverse.v[12] = -a0->m12 * a0->m23 * a0->m34 \
        + a0->m12 * a0->m33 * a0->m24 \
        + a0->m13 * a0->m22 * a0->m34 \
        - a0->m13 * a0->m32 * a0->m24 \
        - a0->m14 * a0->m22 * a0->m33 \
        + a0->m14 * a0->m32 * a0->m23; \
    inverse.v[1] = -a0->m21 * a0->m33 * a0->m44 \
        + a0->m21 * a0->m43 * a0->m34 \
        + a0->m23 * a0->m31 * a0->m44 \
        - a0->m23 * a0->m41 * a0->m34 \
        - a0->m24 * a0->m31 * a0->m43 \
        + a0->m24 * a0->m41 * a0->m33; \
    inverse.v[5] = a0->m11 * a0->m33 * a0->m44 \
        -a0->m11 * a0->m43 * a0->m34 \
        - a0->m13 * a0->m31 * a0->m44 \
        + a0->m13 * a0->m41 * a0->m34 \
        + a0->m14 * a0->m31 * a0->m43 \
        - a0->m14 * a0->m41 * a0->m33; \
    inverse.v[9] = -a0->m11 * a0->m23 * a0->m44 \
        +a0->m11 * a0->m43 * a0->m24 \
        + a0->m13 * a0->m21 * a0->m44 \
        - a0->m13 * a0->m41 * a0->m24 \
        - a0->m14 * a0->m21 * a0->m43 \
        + a0->m14 * a0->m41 * a0->m23; \
    inverse.v[13] =a0->m11 * a0->m23 * a0->m34 \
        -a0->m11 * a0->m33 * a0->m24 \
        - a0->m13 * a0->m21 * a0->m34 \
        + a0->m13 * a0->m31 * a0->m24 \
        + a0->m14 * a0->m21 * a0->m33 \
        - a0->m14 * a0->m31 * a0->m23; \
    inverse.v[2] = a0->m21 * a0->m32 * a0->m44 \
        - a0->m21 * a0->m42 * a0->m34 \
        - a0->m22 * a0->m31 * a0->m44 \
        + a0->m22 * a0->m41 * a0->m34 \
        + a0->m24 * a0->m31 * a0->m42 \
        - a0->m24 * a0->m41 * a0->m32; \
    inverse.v[6] = -a0->m11 * a0->m32 * a0->m44 \
        + a0->m11 * a0->m42 * a0->m34 \
        + a0->m12 * a0->m31 * a0->m44 \
        - a0->m12 * a0->m41 * a0->m34 \
        - a0->m14 * a0->m31 * a0->m42 \
        + a0->m14 * a0->m41 * a0->m32; \
    inverse.v[10] =a0->m11 * a0->m22 * a0->m44 \
        -a0->m11 * a0->m42 * a0->m24 \
        - a0->m12 * a0->m21 * a0->m44 \
        + a0->m12 * a0->m41 * a0->m24 \
        + a0->m14 * a0->m21 * a0->m42 \
        - a0->m14 * a0->m41 * a0->m22; \
    inverse.v[14] = -a0->m11 * a0->m22 * a0->m34 \
        + a0->m11 * a0->m32 * a0->m24 \
        + a0->m12 * a0->m21 * a0->m34 \
        - a0->m12 * a0->m31 * a0->m24 \
        - a0->m14 * a0->m21 * a0->m32 \
        + a0->m14 * a0->m31 * a0->m22; \
    inverse.v[3] = -a0->m21 * a0->m32 * a0->m43 \
        + a0->m21 * a0->m42 * a0->m33 \
        + a0->m22 * a0->m31 * a0->m43 \
        - a0->m22 * a0->m41 * a0->m33 \
        - a0->m23 * a0->m31 * a0->m42 \
        + a0->m23 * a0->m41 * a0->m32; \
    inverse.v[7] =a0->m11 * a0->m32 * a0->m43 \
        -a0->m11 * a0->m42 * a0->m33 \
        - a0->m12 * a0->m31 * a0->m43 \
        + a0->m12 * a0->m41 * a0->m33 \
        + a0->m13 * a0->m31 * a0->m42 \
        - a0->m13 * a0->m41 * a0->m32; \
    inverse.v[11] = -a0->m11 * a0->m22 * a0->m43 \
        + a0->m11 * a0->m42 * a0->m23 \
        + a0->m12 * a0->m21 * a0->m43 \
        - a0->m12 * a0->m41 * a0->m23 \
        - a0->m13 * a0->m21 * a0->m42 \
        + a0->m13 * a0->m41 * a0->m22; \
    inverse.v[15] =a0->m11 * a0->m22 * a0->m33 \
        -a0->m11 * a0->m32 * a0->m23 \
        - a0->m12 * a0->m21 * a0->m33 \
        + a0->m12 * a0->m31 * a0->m23 \
        + a0->m13 * a0->m21 * a0->m32 \
        - a0->m13 * a0->m31 * a0->m22; \
    inverted_determinant = (m##_char##_t)(1.0) / (a0->m11 * inverse.v[0] + a0->m21 * inverse.v[4] + a0->m31 * inverse.v[8] + a0->m41 * inverse.v[12]); \
    result->v[0] = inverse.v[0] * inverted_determinant; \
    result->v[1] = inverse.v[1] * inverted_determinant; \
    result->v[2] = inverse.v[2] * inverted_determinant; \
    result->v[3] = inverse.v[3] * inverted_determinant; \
    result->v[4] = inverse.v[4] * inverted_determinant; \
    result->v[5] = inverse.v[5] * inverted_determinant; \
    result->v[6] = inverse.v[6] * inverted_determinant; \
    result->v[7] = inverse.v[7] * inverted_determinant; \
    result->v[8] = inverse.v[8] * inverted_determinant; \
    result->v[9] = inverse.v[9] * inverted_determinant; \
    result->v[10] = inverse.v[10] * inverted_determinant; \
    result->v[11] = inverse.v[11] * inverted_determinant; \
    result->v[12] = inverse.v[12] * inverted_determinant; \
    result->v[13] = inverse.v[13] * inverted_determinant; \
    result->v[14] = inverse.v[14] * inverted_determinant; \
    result->v[15] = inverse.v[15] * inverted_determinant; \
    return result; \
}

M_GEN_IMPL_mat4_inverse(mat4,f);
M_GEN_IMPL_mat4_inverse(mat4,d);
#endif

#ifdef M_GEN_quat_normalize
#define M_GEN_IMPL_quat_normalize(_s,_char) \
M_GEN_quat_normalize(_s,_char){ \
    m##_char##_t l = (m##_char##_t)(1.0) / M##_char##SQRT(q0->v[0] * q0->v[0] + q0->v[1] * q0->v[1] + q0->v[2] * q0->v[2] + q0->v[3] * q0->v[3]); \
    result->v[0] = q0->v[0] * l; \
    result->v[1] = q0->v[1] * l; \
    result->v[2] = q0->v[2] * l; \
    result->v[3] = q0->v[3] * l; \
    return result; \
}

M_GEN_IMPL_quat_normalize(quat,f);
M_GEN_IMPL_quat_normalize(quat,d);
#endif

#ifdef M_GEN_quat_power
#define M_GEN_IMPL_quat_power(_s,_char) \
M_GEN_quat_power(_s,_char){ \
    if (M##_char##FABS(q0->v[3]) < (m##_char##_t)(1.0) - M##_char##EPSILON) { \
        m##_char##_t alpha = M##_char##ACOS(q0->v[3]); \
        m##_char##_t new_alpha = alpha * exponent; \
        m##_char##_t s = M##_char##SIN(new_alpha) / M##_char##SIN(alpha); \
        result->v[0] = result->v[0] * s; \
        result->v[1] = result->v[1] * s; \
        result->v[2] = result->v[2] * s; \
        result->v[3] = M##_char##COS(new_alpha); \
    } else { \
        memcpy(result,q0,sizeof(m##_char##_t)*QUAT_SIZE); \
    } \
    return result; \
}

M_GEN_IMPL_quat_power(quat,f);
M_GEN_IMPL_quat_power(quat,d);
#endif

#ifdef M_GEN_quat_from_axis_angle
#define M_GEN_IMPL_quat_from_axis_angle(_s,_char) \
M_GEN_quat_from_axis_angle(_s,_char){ \
    m##_char##_t half = angle * (m##_char##_t)(0.5); \
    m##_char##_t s = M##_char##SIN(half); \
    result->v[0] = v0->v[0] * s; \
    result->v[1] = v0->v[1] * s; \
    result->v[2] = v0->v[2] * s; \
    result->v[3] = M##_char##COS(half); \
    return result; \
}

M_GEN_IMPL_quat_from_axis_angle(quat,f);
M_GEN_IMPL_quat_from_axis_angle(quat,d);
#endif

#ifdef M_GEN_quat_from_vec3
#define M_GEN_IMPL_quat_from_vec3(_char) \
M_GEN_quat_from_vec3(_char){ \
    struct vec3##_char cross; \
    m##_char##_t d = vec3##_char##_dot(v0, v1); \
    m##_char##_t a_ls = vec3##_char##_length_squared(v0); \
    m##_char##_t b_ls = vec3##_char##_length_squared(v0); \
    vec3##_char##_cross(&cross, v0, v1); \
    *result = (struct quat##_char){.v={cross.v[0], cross.v[1], cross.v[1], d + M##_char##SQRT(a_ls * b_ls)}}; \
    quat##_char##_normalize(result, result); \
    return result; \
}

M_GEN_IMPL_quat_from_vec3(f);
M_GEN_IMPL_quat_from_vec3(d);
#endif

#ifdef M_GEN_quat_from_mat4
#define M_GEN_IMPL_quat_from_mat4(_s,_char) \
M_GEN_quat_from_mat4(_s,_char){ \
    m##_char##_t scale = m0->v[0] + m0->v[5] + m0->v[10]; \
    if (scale > (m##_char##_t)(0.0)) { \
        m##_char##_t sr = M##_char##SQRT(scale + (m##_char##_t)(1.0)); \
        result->v[3] = sr * (m##_char##_t)(0.5); \
        sr = (m##_char##_t)(0.5) / sr; \
        result->v[0] = (m0->v[9] - m0->v[6]) * sr; \
        result->v[1] = (m0->v[2] - m0->v[8]) * sr; \
        result->v[2] = (m0->v[4] - m0->v[1]) * sr; \
    } else if ((m0->v[0] >= m0->v[5]) && (m0->v[0] >= m0->v[10])) { \
        m##_char##_t sr = M##_char##SQRT((m##_char##_t)(1.0) + m0->v[0] - m0->v[5] - m0->v[10]); \
        m##_char##_t half = (m##_char##_t)(0.5) / sr; \
        result->v[0] = (m##_char##_t)(0.5) * sr; \
        result->v[1] = (m0->v[4] + m0->v[1]) * half; \
        result->v[2] = (m0->v[8] + m0->v[2]) * half; \
        result->v[3] = (m0->v[9] - m0->v[6]) * half; \
    } else if (m0->v[5] > m0->v[10]) { \
        m##_char##_t sr = M##_char##SQRT((m##_char##_t)(1.0) + m0->v[5] - m0->v[0] - m0->v[10]); \
        m##_char##_t half = (m##_char##_t)(0.5) / sr; \
        result->v[0] = (m0->v[1] + m0->v[4]) * half; \
        result->v[1] = (m##_char##_t)(0.5) * sr; \
        result->v[2] = (m0->v[6] + m0->v[9]) * half; \
        result->v[3] = (m0->v[2] - m0->v[8]) * half; \
    } else { \
        m##_char##_t sr = M##_char##SQRT((m##_char##_t)(1.0) + m0->v[10] - m0->v[0] - m0->v[5]); \
        m##_char##_t half = (m##_char##_t)(0.5) / sr; \
        result->v[0] = (m0->v[2] + m0->v[8]) * half; \
        result->v[1] = (m0->v[6] + m0->v[9]) * half; \
        result->v[2] = (m##_char##_t)(0.5) * sr; \
        result->v[3] = (m0->v[4] - m0->v[1]) * half; \
    } \
    return result; \
}

M_GEN_IMPL_quat_from_mat4(quat,f);
M_GEN_IMPL_quat_from_mat4(quat,d);
#endif

#ifdef M_GEN_quat_slerp
#define M_GEN_IMPL_quat_slerp(_s,_char) \
M_GEN_quat_slerp(_s,_char){ \
    struct quat##_char tmp1; \
    m##_char##_t d = quat##_char##_dot(q0, q1); \
    m##_char##_t f0; \
    m##_char##_t f1; \
    tmp1 = *q1; \
    if (d < (m##_char##_t)(0.0)) { \
        quat##_char##_negative(&tmp1, &tmp1); \
        d = -d; \
    } \
    if (d > (m##_char##_t)(0.9995)) { \
        f0 = (m##_char##_t)(1.0) - f; \
        f1 = f; \
    } else { \
        m##_char##_t theta = M##_char##ACOS(d); \
        m##_char##_t sin_theta = M##_char##SIN(theta); \
        f0 = M##_char##SIN(((m##_char##_t)(1.0) - f) * theta) / sin_theta; \
        f1 = M##_char##SIN(f * theta) / sin_theta; \
    } \
    result->v[0] = q0->v[0] * f0 + tmp1.v[0] * f1; \
    result->v[1] = q0->v[1] * f0 + tmp1.v[1] * f1; \
    result->v[2] = q0->v[2] * f0 + tmp1.v[2] * f1; \
    result->v[3] = q0->v[3] * f0 + tmp1.v[3] * f1; \
    return result; \
}

M_GEN_IMPL_quat_slerp(quat,f);
M_GEN_IMPL_quat_slerp(quat,d);
#endif

#ifdef M_GEN_quat_angle
#define M_GEN_IMPL_quat_angle(_s,_char) \
M_GEN_quat_angle(_s,_char){ \
    m##_char##_t s = M##_char##SQRT(quat##_char##_length_squared(q0) * quat##_char##_length_squared(q1)); \
    s = (m##_char##_t)(1.0) / s; \
    return M##_char##ACOS(quat##_char##_dot(q0, q1) * s); \
}

M_GEN_IMPL_quat_angle(quat,f);
M_GEN_IMPL_quat_angle(quat,d);
#endif

#ifdef M_GEN_identity
#define M_GEN_IMPL2_identity(_s,_char) \
M_GEN_identity(_s,_char){ \
    *result = (struct _s##_char) \
    {1,0, \
     0,1}; \
    return result; \
}

#define M_GEN_IMPL3_identity(_s,_char) \
M_GEN_identity(_s,_char){ \
    *result = (struct _s##_char) \
    {1,0,0, \
     0,1,0, \
     0,0,1}; \
    return result; \
}

#define M_GEN_IMPL4_identity(_s,_char) \
M_GEN_identity(_s,_char){ \
    *result = (struct _s##_char) \
    {1,0,0,0, \
     0,1,0,0, \
     0,0,1,0, \
     0,0,0,1}; \
    return result; \
}

M_GEN_IMPL2_identity(mat2,f);
M_GEN_IMPL2_identity(mat2,d);
M_GEN_IMPL3_identity(mat3,f);
M_GEN_IMPL3_identity(mat3,d);
M_GEN_IMPL4_identity(mat4,f);
M_GEN_IMPL4_identity(mat4,d);
#endif

#ifdef M_GEN_determinant
#define M_GEN_IMPL_mat2_determinant(_s,_char) \
M_GEN_determinant(_s,_char){ \
    return m0->m11 * m0->m22 - m0->m12 * m0->m21; \
}

M_GEN_IMPL_mat2_determinant(mat2,f);
M_GEN_IMPL_mat2_determinant(mat2,d);

#define M_GEN_IMPL_mat3_determinant(_s,_char) \
M_GEN_determinant(_s,_char){ \
    return \
          m0->m11 * m0->m22 * m0->m33 \
        + m0->m12 * m0->m23 * m0->m31 \
        + m0->m13 * m0->m21 * m0->m32 \
        - m0->m11 * m0->m23 * m0->m32 \
        - m0->m12 * m0->m21 * m0->m33 \
        - m0->m13 * m0->m22 * m0->m31; \
}

M_GEN_IMPL_mat3_determinant(mat3,f);
M_GEN_IMPL_mat3_determinant(mat3,d);

#define M_GEN_IMPL_mat4_determinant(_s,_char) \
M_GEN_determinant(_s,_char){ \
    return m0->m14 * m0->m23 * m0->m32 * m0->m41 - m0->m13 * m0->m24 * m0->m32 * m0->m41 \
         - m0->m14 * m0->m22 * m0->m33 * m0->m41 + m0->m12 * m0->m24 * m0->m33 * m0->m41 \
         + m0->m13 * m0->m22 * m0->m34 * m0->m41 - m0->m12 * m0->m23 * m0->m34 * m0->m41 \
         - m0->m14 * m0->m23 * m0->m31 * m0->m42 + m0->m13 * m0->m24 * m0->m31 * m0->m42 \
         + m0->m14 * m0->m21 * m0->m33 * m0->m42 - m0->m11 * m0->m24 * m0->m33 * m0->m42 \
         - m0->m13 * m0->m21 * m0->m34 * m0->m42 + m0->m11 * m0->m23 * m0->m34 * m0->m42 \
         + m0->m14 * m0->m22 * m0->m31 * m0->m43 - m0->m12 * m0->m24 * m0->m31 * m0->m43 \
         - m0->m14 * m0->m21 * m0->m32 * m0->m43 + m0->m11 * m0->m24 * m0->m32 * m0->m43 \
         + m0->m12 * m0->m21 * m0->m34 * m0->m43 - m0->m11 * m0->m22 * m0->m34 * m0->m43 \
         - m0->m13 * m0->m22 * m0->m31 * m0->m44 + m0->m12 * m0->m23 * m0->m31 * m0->m44 \
         + m0->m13 * m0->m21 * m0->m32 * m0->m44 - m0->m11 * m0->m23 * m0->m32 * m0->m44 \
         - m0->m12 * m0->m21 * m0->m33 * m0->m44 + m0->m11 * m0->m22 * m0->m33 * m0->m44; \
}

M_GEN_IMPL_mat4_determinant(mat4,f);
M_GEN_IMPL_mat4_determinant(mat4,d);
#endif


#ifdef M_GEN_transpose
#define M_GEN_IMPL_mat2_transpose(_s,_char) \
M_GEN_transpose(_s,_char){ \
    struct _s##_char transposed; \
    transposed.v[0] = m0->v[0]; \
    transposed.v[1] = m0->v[2]; \
    transposed.v[2] = m0->v[1]; \
    transposed.v[3] = m0->v[3]; \
    result->v[0] = transposed.v[0]; \
    result->v[1] = transposed.v[1]; \
    result->v[2] = transposed.v[2]; \
    result->v[3] = transposed.v[3]; \
    return result; \
}

M_GEN_IMPL_mat2_transpose(mat2,f);
M_GEN_IMPL_mat2_transpose(mat2,d);

#define M_GEN_IMPL_mat3_transpose(_s,_char) \
M_GEN_transpose(_s,_char){ \
    struct _s##_char transposed; \
    transposed.v[0] = m0->v[0]; \
    transposed.v[1] = m0->v[3]; \
    transposed.v[2] = m0->v[6]; \
    transposed.v[3] = m0->v[1]; \
    transposed.v[4] = m0->v[4]; \
    transposed.v[5] = m0->v[7]; \
    transposed.v[6] = m0->v[2]; \
    transposed.v[7] = m0->v[5]; \
    transposed.v[8] = m0->v[8]; \
    result->v[0] = transposed.v[0]; \
    result->v[1] = transposed.v[1]; \
    result->v[2] = transposed.v[2]; \
    result->v[3] = transposed.v[3]; \
    result->v[4] = transposed.v[4]; \
    result->v[5] = transposed.v[5]; \
    result->v[6] = transposed.v[6]; \
    result->v[7] = transposed.v[7]; \
    result->v[8] = transposed.v[8]; \
    return result; \
}

M_GEN_IMPL_mat3_transpose(mat3,f);
M_GEN_IMPL_mat3_transpose(mat3,d);

#define M_GEN_IMPL_mat4_transpose(_s,_char) \
M_GEN_transpose(_s,_char){ \
    struct _s##_char transposed; \
    transposed.v[0] = m0->v[0]; \
    transposed.v[1] = m0->v[4]; \
    transposed.v[2] = m0->v[8]; \
    transposed.v[3] = m0->v[12]; \
    transposed.v[4] = m0->v[1]; \
    transposed.v[5] = m0->v[5]; \
    transposed.v[6] = m0->v[9]; \
    transposed.v[7] = m0->v[13]; \
    transposed.v[8] = m0->v[2]; \
    transposed.v[9] = m0->v[6]; \
    transposed.v[10] = m0->v[10]; \
    transposed.v[11] = m0->v[14]; \
    transposed.v[12] = m0->v[3]; \
    transposed.v[13] = m0->v[7]; \
    transposed.v[14] = m0->v[11]; \
    transposed.v[15] = m0->v[15]; \
    result->v[0] = transposed.v[0]; \
    result->v[1] = transposed.v[1]; \
    result->v[2] = transposed.v[2]; \
    result->v[3] = transposed.v[3]; \
    result->v[4] = transposed.v[4]; \
    result->v[5] = transposed.v[5]; \
    result->v[6] = transposed.v[6]; \
    result->v[7] = transposed.v[7]; \
    result->v[8] = transposed.v[8]; \
    result->v[9] = transposed.v[9]; \
    result->v[10] = transposed.v[10]; \
    result->v[11] = transposed.v[11]; \
    result->v[12] = transposed.v[12]; \
    result->v[13] = transposed.v[13]; \
    result->v[14] = transposed.v[14]; \
    result->v[15] = transposed.v[15]; \
    return result; \
}

M_GEN_IMPL_mat4_transpose(mat4,f);
M_GEN_IMPL_mat4_transpose(mat4,d);
#endif

#ifdef M_GEN_cofactor
#define M_GEN_IMPL_mat2_cofactor(_s,_char) \
M_GEN_cofactor(_s,_char){ \
    struct _s##_char cofactor; \
    cofactor.v[0] = m0->v[3]; \
    cofactor.v[1] = -m0->v[2]; \
    cofactor.v[2] = -m0->v[1]; \
    cofactor.v[3] = m0->v[0]; \
    result->v[0] = cofactor.v[0]; \
    result->v[1] = cofactor.v[1]; \
    result->v[2] = cofactor.v[2]; \
    result->v[3] = cofactor.v[3]; \
    return result; \
}

M_GEN_IMPL_mat2_cofactor(mat2,f);
M_GEN_IMPL_mat2_cofactor(mat2,d);

#define M_GEN_IMPL_mat3_cofactor(_s,_char) \
M_GEN_cofactor(_s,_char){ \
    struct mat3##_char cofactor; \
    struct mat2##_char minor; \
    minor.v[0] = m0->v[4]; \
    minor.v[1] = m0->v[5]; \
    minor.v[2] = m0->v[7]; \
    minor.v[3] = m0->v[8]; \
    cofactor.v[0] = mat2##_char##_determinant(&minor); \
    minor.v[0] = m0->v[3]; \
    minor.v[1] = m0->v[5]; \
    minor.v[2] = m0->v[6]; \
    minor.v[3] = m0->v[8]; \
    cofactor.v[1] = -mat2##_char##_determinant(&minor); \
    minor.v[0] = m0->v[3]; \
    minor.v[1] = m0->v[4]; \
    minor.v[2] = m0->v[6]; \
    minor.v[3] = m0->v[7]; \
    cofactor.v[2] = mat2##_char##_determinant(&minor); \
    minor.v[0] = m0->v[1]; \
    minor.v[1] = m0->v[2]; \
    minor.v[2] = m0->v[7]; \
    minor.v[3] = m0->v[8]; \
    cofactor.v[3] = -mat2##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[2]; \
    minor.v[2] = m0->v[6]; \
    minor.v[3] = m0->v[8]; \
    cofactor.v[4] = mat2##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[1]; \
    minor.v[2] = m0->v[6]; \
    minor.v[3] = m0->v[7]; \
    cofactor.v[5] = -mat2##_char##_determinant(&minor); \
    minor.v[0] = m0->v[1]; \
    minor.v[1] = m0->v[2]; \
    minor.v[2] = m0->v[4]; \
    minor.v[3] = m0->v[5]; \
    cofactor.v[6] = mat2##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[2]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[5]; \
    cofactor.v[7] = -mat2##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[1]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[4]; \
    cofactor.v[8] = mat2##_char##_determinant(&minor); \
    result->v[0] = cofactor.v[0]; \
    result->v[1] = cofactor.v[1]; \
    result->v[2] = cofactor.v[2]; \
    result->v[3] = cofactor.v[3]; \
    result->v[4] = cofactor.v[4]; \
    result->v[5] = cofactor.v[5]; \
    result->v[6] = cofactor.v[6]; \
    result->v[7] = cofactor.v[7]; \
    result->v[8] = cofactor.v[8]; \
    return result; \
}

M_GEN_IMPL_mat3_cofactor(mat3,f);
M_GEN_IMPL_mat3_cofactor(mat3,d);

#define M_GEN_IMPL_cofactor(_s,_char) \
M_GEN_cofactor(_s,_char){ \
    struct mat4##_char cofactor; \
    struct mat3##_char minor; \
    minor.v[0] = m0->v[5]; \
    minor.v[1] = m0->v[6]; \
    minor.v[2] = m0->v[7]; \
    minor.v[3] = m0->v[9]; \
    minor.v[4] = m0->v[10]; \
    minor.v[5] = m0->v[11]; \
    minor.v[6] = m0->v[13]; \
    minor.v[7] = m0->v[14]; \
    minor.v[8] = m0->v[15]; \
    cofactor.v[0] = mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[4]; \
    minor.v[1] = m0->v[6]; \
    minor.v[2] = m0->v[7]; \
    minor.v[3] = m0->v[8]; \
    minor.v[4] = m0->v[10]; \
    minor.v[5] = m0->v[11]; \
    minor.v[6] = m0->v[12]; \
    minor.v[7] = m0->v[14]; \
    minor.v[8] = m0->v[15]; \
    cofactor.v[1] = -mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[4]; \
    minor.v[1] = m0->v[5]; \
    minor.v[2] = m0->v[7]; \
    minor.v[3] = m0->v[8]; \
    minor.v[4] = m0->v[9]; \
    minor.v[5] = m0->v[11]; \
    minor.v[6] = m0->v[12]; \
    minor.v[7] = m0->v[13]; \
    minor.v[8] = m0->v[15]; \
    cofactor.v[2] = mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[4]; \
    minor.v[1] = m0->v[5]; \
    minor.v[2] = m0->v[6]; \
    minor.v[3] = m0->v[8]; \
    minor.v[4] = m0->v[9]; \
    minor.v[5] = m0->v[10]; \
    minor.v[6] = m0->v[12]; \
    minor.v[7] = m0->v[13]; \
    minor.v[8] = m0->v[14]; \
    cofactor.v[3] = -mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[1]; \
    minor.v[1] = m0->v[2]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[9]; \
    minor.v[4] = m0->v[10]; \
    minor.v[5] = m0->v[11]; \
    minor.v[6] = m0->v[13]; \
    minor.v[7] = m0->v[14]; \
    minor.v[8] = m0->v[15]; \
    cofactor.v[4] = -mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[2]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[8]; \
    minor.v[4] = m0->v[10]; \
    minor.v[5] = m0->v[11]; \
    minor.v[6] = m0->v[12]; \
    minor.v[7] = m0->v[14]; \
    minor.v[8] = m0->v[15]; \
    cofactor.v[5] = mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[1]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[8]; \
    minor.v[4] = m0->v[9]; \
    minor.v[5] = m0->v[11]; \
    minor.v[6] = m0->v[12]; \
    minor.v[7] = m0->v[13]; \
    minor.v[8] = m0->v[15]; \
    cofactor.v[6] = -mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[1]; \
    minor.v[2] = m0->v[2]; \
    minor.v[3] = m0->v[8]; \
    minor.v[4] = m0->v[9]; \
    minor.v[5] = m0->v[10]; \
    minor.v[6] = m0->v[12]; \
    minor.v[7] = m0->v[13]; \
    minor.v[8] = m0->v[14]; \
    cofactor.v[7] = mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[1]; \
    minor.v[1] = m0->v[2]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[5]; \
    minor.v[4] = m0->v[6]; \
    minor.v[5] = m0->v[7]; \
    minor.v[6] = m0->v[13]; \
    minor.v[7] = m0->v[14]; \
    minor.v[8] = m0->v[15]; \
    cofactor.v[8] = mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[2]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[4]; \
    minor.v[4] = m0->v[6]; \
    minor.v[5] = m0->v[7]; \
    minor.v[6] = m0->v[12]; \
    minor.v[7] = m0->v[14]; \
    minor.v[8] = m0->v[15]; \
    cofactor.v[9] = -mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[1]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[4]; \
    minor.v[4] = m0->v[5]; \
    minor.v[5] = m0->v[7]; \
    minor.v[6] = m0->v[12]; \
    minor.v[7] = m0->v[13]; \
    minor.v[8] = m0->v[15]; \
    cofactor.v[10] = mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[1]; \
    minor.v[2] = m0->v[2]; \
    minor.v[3] = m0->v[4]; \
    minor.v[4] = m0->v[5]; \
    minor.v[5] = m0->v[6]; \
    minor.v[6] = m0->v[12]; \
    minor.v[7] = m0->v[13]; \
    minor.v[8] = m0->v[14]; \
    cofactor.v[11] = -mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[1]; \
    minor.v[1] = m0->v[2]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[5]; \
    minor.v[4] = m0->v[6]; \
    minor.v[5] = m0->v[7]; \
    minor.v[6] = m0->v[9]; \
    minor.v[7] = m0->v[10]; \
    minor.v[8] = m0->v[11]; \
    cofactor.v[12] = -mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[2]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[4]; \
    minor.v[4] = m0->v[6]; \
    minor.v[5] = m0->v[7]; \
    minor.v[6] = m0->v[8]; \
    minor.v[7] = m0->v[10]; \
    minor.v[8] = m0->v[11]; \
    cofactor.v[13] = mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[1]; \
    minor.v[2] = m0->v[3]; \
    minor.v[3] = m0->v[4]; \
    minor.v[4] = m0->v[5]; \
    minor.v[5] = m0->v[7]; \
    minor.v[6] = m0->v[8]; \
    minor.v[7] = m0->v[9]; \
    minor.v[8] = m0->v[11]; \
    cofactor.v[14] = -mat3##_char##_determinant(&minor); \
    minor.v[0] = m0->v[0]; \
    minor.v[1] = m0->v[1]; \
    minor.v[2] = m0->v[2]; \
    minor.v[3] = m0->v[4]; \
    minor.v[4] = m0->v[5]; \
    minor.v[5] = m0->v[6]; \
    minor.v[6] = m0->v[8]; \
    minor.v[7] = m0->v[9]; \
    minor.v[8] = m0->v[10]; \
    cofactor.v[15] = mat3##_char##_determinant(&minor); \
    result->v[0] = cofactor.v[0]; \
    result->v[1] = cofactor.v[1]; \
    result->v[2] = cofactor.v[2]; \
    result->v[3] = cofactor.v[3]; \
    result->v[4] = cofactor.v[4]; \
    result->v[5] = cofactor.v[5]; \
    result->v[6] = cofactor.v[6]; \
    result->v[7] = cofactor.v[7]; \
    result->v[8] = cofactor.v[8]; \
    result->v[9] = cofactor.v[9]; \
    result->v[10] = cofactor.v[10]; \
    result->v[11] = cofactor.v[11]; \
    result->v[12] = cofactor.v[12]; \
    result->v[13] = cofactor.v[13]; \
    result->v[14] = cofactor.v[14]; \
    result->v[15] = cofactor.v[15]; \
    return result; \
}

M_GEN_IMPL_cofactor(mat4,f);
M_GEN_IMPL_cofactor(mat4,d);
#endif

#ifdef M_GEN_adjugate
#define M_GEN_IMPL_adjugate(_s,_char) \
M_GEN_adjugate(_s,_char){ \
    struct _s##_char adjugate; \
    adjugate.v[0] = m0->v[3]; \
    adjugate.v[1] = -m0->v[1]; \
    adjugate.v[2] = -m0->v[2]; \
    adjugate.v[3] = m0->v[0]; \
    result->v[0] = adjugate.v[0]; \
    result->v[1] = adjugate.v[1]; \
    result->v[2] = adjugate.v[2]; \
    result->v[3] = adjugate.v[3]; \
    return result; \
}

M_GEN_IMPL_adjugate(mat2,f);
M_GEN_IMPL_adjugate(mat2,d);
#endif

#ifdef M_GEN_scaling
#define M_GEN_IMPL_mat2_scaling(_s,_n,_char) \
M_GEN_scaling(_s,_n,_char){ \
    result->v[0] = v0->v[0]; \
    result->v[3] = v0->v[1]; \
    return result; \
}

M_GEN_IMPL_mat2_scaling(mat,2,f);
M_GEN_IMPL_mat2_scaling(mat,2,d);

#define M_GEN_IMPL_mat3_scaling(_s,_n,_char) \
M_GEN_scaling(_s,_n,_char){ \
    result->v[0] = v0->v[0]; \
    result->v[4] = v0->v[1]; \
    result->v[8] = v0->v[2]; \
    return result; \
}

M_GEN_IMPL_mat3_scaling(mat,3,f);
M_GEN_IMPL_mat3_scaling(mat,3,d);

#define M_GEN_IMPL_mat4_scaling(_s,_n,_char) \
M_GEN_scaling(_s,_n,_char){ \
    result->v[0] = v0->v[0]; \
    result->v[5] = v0->v[1]; \
    result->v[10] = v0->v[2]; \
    return result; \
}

M_GEN_IMPL_mat4_scaling(mat,4,f);
M_GEN_IMPL_mat4_scaling(mat,4,d);
#endif

#ifdef M_GEN_scale
#define M_GEN_IMPL_mat2_scale(_s,_n,_char) \
M_GEN_scale(_s,_n,_char){ \
    result->v[0] *= v0->v[0]; \
    result->v[3] *= v0->v[1]; \
    return result; \
}

M_GEN_IMPL_mat2_scale(mat,2,f);
M_GEN_IMPL_mat2_scale(mat,2,d);

#define M_GEN_IMPL_mat3_scale(_s,_n,_char) \
M_GEN_scale(_s,_n,_char){ \
    result->v[0] *= v0->v[0]; \
    result->v[4] *= v0->v[1]; \
    result->v[8] *= v0->v[2]; \
    return result; \
}

M_GEN_IMPL_mat3_scale(mat,3,f);
M_GEN_IMPL_mat3_scale(mat,3,d);

#define M_GEN_IMPL_mat4_scale(_s,_n,_char) \
M_GEN_scale(_s,_n,_char){ \
    result->v[0]  *= v0->v[0]; \
    result->v[5]  *= v0->v[1]; \
    result->v[10] *= v0->v[2]; \
    return result; \
}

M_GEN_IMPL_mat4_scale(mat,4,f);
M_GEN_IMPL_mat4_scale(mat,4,d);
#endif

#ifdef M_GEN_rotation_x
#define M_GEN_IMPL_mat3_rotation_x(_s,_char) \
M_GEN_rotation_x(_s,_char){ \
    m##_char##_t c = M##_char##COS(f); \
    m##_char##_t s = M##_char##SIN(f); \
    result->v[4] = c; \
    result->v[5] = s; \
    result->v[7] = -s; \
    result->v[8] = c; \
    return result; \
}

M_GEN_IMPL_mat3_rotation_x(mat3,f);
M_GEN_IMPL_mat3_rotation_x(mat3,d);

#define M_GEN_IMPL_mat4_rotation_x(_s,_char) \
M_GEN_rotation_x(_s,_char){ \
    m##_char##_t c = M##_char##COS(f); \
    m##_char##_t s = M##_char##SIN(f); \
    result->v[5] = c; \
    result->v[6] = s; \
    result->v[9] = -s; \
    result->v[10] = c; \
    return result; \
}

M_GEN_IMPL_mat4_rotation_x(mat4,f);
M_GEN_IMPL_mat4_rotation_x(mat4,d);
#endif

#ifdef M_GEN_rotation_y
#define M_GEN_IMPL_mat3_rotation_y(_s,_char) \
M_GEN_rotation_y(_s,_char){ \
    m##_char##_t c = M##_char##COS(f); \
    m##_char##_t s = M##_char##SIN(f); \
    result->v[0] = c; \
    result->v[2] = -s; \
    result->v[6] = s; \
    result->v[8] = c; \
    return result; \
}

M_GEN_IMPL_mat3_rotation_y(mat3,f);
M_GEN_IMPL_mat3_rotation_y(mat3,d);

#define M_GEN_IMPL_mat4_rotation_y(_s,_char) \
M_GEN_rotation_y(_s,_char){ \
    m##_char##_t c = M##_char##COS(f); \
    m##_char##_t s = M##_char##SIN(f); \
    result->v[0] = c; \
    result->v[2] = -s; \
    result->v[8] = s; \
    result->v[10] = c; \
    return result; \
}

M_GEN_IMPL_mat4_rotation_y(mat4,f);
M_GEN_IMPL_mat4_rotation_y(mat4,d);
#endif

#ifdef M_GEN_rotation_z
#define M_GEN_IMPL_mat2_rotation_z(_s,_char) \
M_GEN_rotation_z(_s,_char){ \
    m##_char##_t c = M##_char##COS(f); \
    m##_char##_t s = M##_char##SIN(f); \
    result->v[0] = c; \
    result->v[1] = s; \
    result->v[2] = -s; \
    result->v[3] = c; \
    return result; \
}

M_GEN_IMPL_mat2_rotation_z(mat2,f);
M_GEN_IMPL_mat2_rotation_z(mat2,d);

#define M_GEN_IMPL_mat3_rotation_z(_s,_char) \
M_GEN_rotation_z(_s,_char){ \
    m##_char##_t c = M##_char##COS(f); \
    m##_char##_t s = M##_char##SIN(f); \
    result->v[0] = c; \
    result->v[1] = s; \
    result->v[3] = -s; \
    result->v[4] = c; \
    return result; \
}

M_GEN_IMPL_mat3_rotation_z(mat3,f);
M_GEN_IMPL_mat3_rotation_z(mat3,d);

#define M_GEN_IMPL_mat4_rotation_z(_s,_char) \
M_GEN_rotation_z(_s,_char){ \
    m##_char##_t c = M##_char##COS(f); \
    m##_char##_t s = M##_char##SIN(f); \
    result->v[0] = c; \
    result->v[1] = s; \
    result->v[4] = -s; \
    result->v[5] = c; \
    return result; \
}

M_GEN_IMPL_mat4_rotation_z(mat4,f);
M_GEN_IMPL_mat4_rotation_z(mat4,d);
#endif

#ifdef M_GEN_rotation_axis

#ifdef CGEOM_PRECISE_TRIGONOMETRY
#define MAT_ROTATION_AXIS_TRIGONOMETRY(_char) \
    sincos1cos sc1c = sncs1cs(f); \
    m##_char##_t c = sc1c.cos, s = sc1c.sin, one_c = sc1c.omc;
#else
#define MAT_ROTATION_AXIS_TRIGONOMETRY(_char) \
    m##_char##_t c = M##_char##COS(f); \
    m##_char##_t s = M##_char##SIN(f); \
    m##_char##_t one_c = (m##_char##_t)(1.0) - c;
#endif

#define M_GEN_IMPL_mat3_rotation_axis(_s,_char) \
M_GEN_rotation_axis(_s,_char){ \
    MAT_ROTATION_AXIS_TRIGONOMETRY(_char); \
    m##_char##_t x = v0->v[0]; \
    m##_char##_t y = v0->v[1]; \
    m##_char##_t z = v0->v[2]; \
    m##_char##_t xx = x * x; \
    m##_char##_t xy = x * y; \
    m##_char##_t xz = x * z; \
    m##_char##_t yy = y * y; \
    m##_char##_t yz = y * z; \
    m##_char##_t zz = z * z; \
    m##_char##_t l = xx + yy + zz; \
    m##_char##_t sqrt_l = M##_char##SQRT(l); \
    result->v[0] = (xx + (yy + zz) * c) / l; \
    result->v[1] = (xy * one_c + v0->v[2] * sqrt_l * s) / l; \
    result->v[2] = (xz * one_c - v0->v[1] * sqrt_l * s) / l; \
    result->v[3] = (xy * one_c - v0->v[2] * sqrt_l * s) / l; \
    result->v[4] = (yy + (xx + zz) * c) / l; \
    result->v[5] = (yz * one_c + v0->v[0] * sqrt_l * s) / l; \
    result->v[6] = (xz * one_c + v0->v[1] * sqrt_l * s) / l; \
    result->v[7] = (yz * one_c - v0->v[0] * sqrt_l * s) / l; \
    result->v[8] = (zz + (xx + yy) * c) / l; \
    return result; \
}

M_GEN_IMPL_mat3_rotation_axis(mat3,f);
M_GEN_IMPL_mat3_rotation_axis(mat3,d);

#define M_GEN_IMPL_mat4_rotation_axis(_s,_char) \
M_GEN_rotation_axis(_s,_char){ \
    MAT_ROTATION_AXIS_TRIGONOMETRY(_char);\
    m##_char##_t x = v0->v[0]; \
    m##_char##_t y = v0->v[1]; \
    m##_char##_t z = v0->v[2]; \
    m##_char##_t xx = x * x; \
    m##_char##_t xy = x * y; \
    m##_char##_t xz = x * z; \
    m##_char##_t yy = y * y; \
    m##_char##_t yz = y * z; \
    m##_char##_t zz = z * z; \
    m##_char##_t l = xx + yy + zz; \
    m##_char##_t sqrt_l = M##_char##SQRT(l); \
    result->v[0] = (xx + (yy + zz) * c) / l; \
    result->v[1] = (xy * one_c + v0->v[2] * sqrt_l * s) / l; \
    result->v[2] = (xz * one_c - v0->v[1] * sqrt_l * s) / l; \
    result->v[3] = (m##_char##_t)(0.0); \
    result->v[4] = (xy * one_c - v0->v[2] * sqrt_l * s) / l; \
    result->v[5] = (yy + (xx + zz) * c) / l; \
    result->v[6] = (yz * one_c + v0->v[0] * sqrt_l * s) / l; \
    result->v[7] = (m##_char##_t)(0.0); \
    result->v[8] = (xz * one_c + v0->v[1] * sqrt_l * s) / l; \
    result->v[9] = (yz * one_c - v0->v[0] * sqrt_l * s) / l; \
    result->v[10] = (zz + (xx + yy) * c) / l; \
    result->v[11] = (m##_char##_t)(0.0); \
    result->v[12] = (m##_char##_t)(0.0); \
    result->v[13] = (m##_char##_t)(0.0); \
    result->v[14] = (m##_char##_t)(0.0); \
    result->v[15] = (m##_char##_t)(1.0); \
    return result; \
}

M_GEN_IMPL_mat4_rotation_axis(mat4,f);
M_GEN_IMPL_mat4_rotation_axis(mat4,d);
#endif

#ifdef M_GEN_rotation_quat
#define M_GEN_IMPL_mat3_rotation_quat(_s,_char) \
M_GEN_rotation_quat(_s,_char){ \
    m##_char##_t xx = q0->v[0] * q0->v[0]; \
    m##_char##_t yy = q0->v[1] * q0->v[1]; \
    m##_char##_t zz = q0->v[2] * q0->v[2]; \
    m##_char##_t xy = q0->v[0] * q0->v[1]; \
    m##_char##_t zw = q0->v[2] * q0->v[3]; \
    m##_char##_t xz = q0->v[0] * q0->v[0]; \
    m##_char##_t yw = q0->v[1] * q0->v[3]; \
    m##_char##_t yz = q0->v[1] * q0->v[2]; \
    m##_char##_t xw = q0->v[0] * q0->v[3]; \
    result->v[0] = (m##_char##_t)(1.0) - (m##_char##_t)(2.0) * (yy - zz); \
    result->v[1] = (m##_char##_t)(2.0) * (xy + zw); \
    result->v[2] = (m##_char##_t)(2.0) * (xz - yw); \
    result->v[3] = (m##_char##_t)(2.0) * (xy - zw); \
    result->v[4] = (m##_char##_t)(1.0) - (m##_char##_t)(2.0) * (xx - zz); \
    result->v[5] = (m##_char##_t)(2.0) * (yz + xw); \
    result->v[6] = (m##_char##_t)(2.0) * (xz + yw); \
    result->v[7] = (m##_char##_t)(2.0) * (yz - xw); \
    result->v[8] = (m##_char##_t)(1.0) - (m##_char##_t)(2.0) * (xx - yy); \
    return result; \
}

M_GEN_IMPL_mat3_rotation_quat(mat3,f);
M_GEN_IMPL_mat3_rotation_quat(mat3,d);

#define M_GEN_IMPL_mat4_rotation_quat(_s,_char) \
M_GEN_rotation_quat(_s,_char){ \
    m##_char##_t xx = q0->v[0] * q0->v[0]; \
    m##_char##_t yy = q0->v[1] * q0->v[1]; \
    m##_char##_t zz = q0->v[2] * q0->v[2]; \
    m##_char##_t xy = q0->v[0] * q0->v[1]; \
    m##_char##_t zw = q0->v[2] * q0->v[3]; \
    m##_char##_t xz = q0->v[0] * q0->v[2]; \
    m##_char##_t yw = q0->v[1] * q0->v[3]; \
    m##_char##_t yz = q0->v[1] * q0->v[2]; \
    m##_char##_t xw = q0->v[0] * q0->v[3]; \
    result->v[0] = (m##_char##_t)(1.0) - (m##_char##_t)(2.0) * (yy + zz); \
    result->v[1] = (m##_char##_t)(2.0) * (xy + zw); \
    result->v[2] = (m##_char##_t)(2.0) * (xz - yw); \
    result->v[3] = (m##_char##_t)(0.0); \
    result->v[4] = (m##_char##_t)(2.0) * (xy - zw); \
    result->v[5] = (m##_char##_t)(1.0) - (m##_char##_t)(2.0) * (xx + zz); \
    result->v[6] = (m##_char##_t)(2.0) * (yz + xw); \
    result->v[7] = (m##_char##_t)(0.0); \
    result->v[8] = (m##_char##_t)(2.0) * (xz + yw); \
    result->v[9] = (m##_char##_t)(2.0) * (yz - xw); \
    result->v[10] = (m##_char##_t)(1.0) - (m##_char##_t)(2.0) * (xx + yy); \
    result->v[11] = (m##_char##_t)(0.0); \
    result->v[12] = (m##_char##_t)(0.0); \
    result->v[13] = (m##_char##_t)(0.0); \
    result->v[14] = (m##_char##_t)(0.0); \
    result->v[15] = (m##_char##_t)(1.0); \
    return result; \
}

M_GEN_IMPL_mat4_rotation_quat(mat4,f);
M_GEN_IMPL_mat4_rotation_quat(mat4,d);
#endif


#ifdef M_GEN_translation
#define M_GEN_IMPL_translation(_s,_char) \
M_GEN_translation(_s,_char){ \
    result->v[0] = m0->v[0]; \
    result->v[1] = m0->v[1]; \
    result->v[2] = m0->v[2]; \
    result->v[3] = m0->v[3]; \
    result->v[4] = m0->v[4]; \
    result->v[5] = m0->v[5]; \
    result->v[6] = m0->v[6]; \
    result->v[7] = m0->v[7]; \
    result->v[8] = m0->v[8]; \
    result->v[9] = m0->v[9]; \
    result->v[10] = m0->v[10]; \
    result->v[11] = m0->v[11]; \
    result->v[12] = v0->v[0]; \
    result->v[13] = v0->v[1]; \
    result->v[14] = v0->v[2]; \
    result->v[15] = m0->v[15]; \
    return result; \
}

M_GEN_IMPL_translation(mat4,f);
M_GEN_IMPL_translation(mat4,d);
#endif

#ifdef M_GEN_translate
#define M_GEN_IMPL_translate(_s,_char) \
M_GEN_translate(_s,_char){ \
    result->v[0] = m0->v[0]; \
    result->v[1] = m0->v[1]; \
    result->v[2] = m0->v[2]; \
    result->v[3] = m0->v[3]; \
    result->v[4] = m0->v[4]; \
    result->v[5] = m0->v[5]; \
    result->v[6] = m0->v[6]; \
    result->v[7] = m0->v[7]; \
    result->v[8] = m0->v[8]; \
    result->v[9] = m0->v[9]; \
    result->v[10] = m0->v[10]; \
    result->v[11] = m0->v[11]; \
    result->v[12] = m0->v[12] + v0->v[0]; \
    result->v[13] = m0->v[13] + v0->v[1]; \
    result->v[14] = m0->v[14] + v0->v[2]; \
    result->v[15] = m0->v[15]; \
    return result; \
}

M_GEN_IMPL_translate(mat4,f);
M_GEN_IMPL_translate(mat4,d);
#endif

#ifdef M_GEN_mat4_look_at
#define M_GEN_IMPL_mat4_look_at(_char) \
M_GEN_mat4_look_at(_char){ \
    struct vec3##_char tmp_forward = *target; \
    struct vec3##_char tmp_side; \
    struct vec3##_char tmp_up; \
    vec3##_char##_subtract(&tmp_forward, position); \
    vec3##_char##_normalize(&tmp_forward, &tmp_forward); \
    vec3##_char##_cross(&tmp_side, &tmp_forward, up); \
    vec3##_char##_normalize(&tmp_side, &tmp_side); \
    vec3##_char##_cross(&tmp_up, &tmp_side, &tmp_forward); \
    result->v[0] = tmp_side.v[0]; \
    result->v[1] = tmp_up.v[0]; \
    result->v[2] = -tmp_forward.v[0]; \
    result->v[3] = (m##_char##_t)(0.0); \
    result->v[4] = tmp_side.v[1]; \
    result->v[5] = tmp_up.v[1]; \
    result->v[6] = -tmp_forward.v[1]; \
    result->v[7] = (m##_char##_t)(0.0); \
    result->v[8] = tmp_side.v[2]; \
    result->v[9] = tmp_up.v[2]; \
    result->v[10] = -tmp_forward.v[2]; \
    result->v[11] = (m##_char##_t)(0.0); \
    result->v[12] = -vec3##_char##_dot(&tmp_side, position); \
    result->v[13] = -vec3##_char##_dot(&tmp_up, position); \
    result->v[14] = vec3##_char##_dot(&tmp_forward, position); \
    result->v[15] = (m##_char##_t)(1.0); \
    return result; \
}

M_GEN_IMPL_mat4_look_at(f);
M_GEN_IMPL_mat4_look_at(d);
#endif

#ifdef M_GEN_ortho
#define M_GEN_IMPL_ortho(_s,_char) \
M_GEN_ortho(_s,_char){ \
    result->v[0] = (m##_char##_t)(2.0) / (r - l); \
    result->v[1] = (m##_char##_t)(0.0); \
    result->v[2] = (m##_char##_t)(0.0); \
    result->v[3] = (m##_char##_t)(0.0); \
    result->v[4] = (m##_char##_t)(0.0); \
    result->v[5] = (m##_char##_t)(2.0) / (t - b); \
    result->v[6] = (m##_char##_t)(0.0); \
    result->v[7] = (m##_char##_t)(0.0); \
    result->v[8] = (m##_char##_t)(0.0); \
    result->v[9] = (m##_char##_t)(0.0); \
    result->v[10] = -(m##_char##_t)(2.0) / (f - n); \
    result->v[11] = (m##_char##_t)(0.0); \
    result->v[12] = -((r + l) / (r - l)); \
    result->v[13] = -((t + b) / (t - b)); \
    result->v[14] = -((f + n) / (f - n)); \
    result->v[15] = (m##_char##_t)(1.0); \
    return result; \
}

M_GEN_IMPL_ortho(mat4,f);
M_GEN_IMPL_ortho(mat4,d);
#endif

#ifdef M_GEN_perspective
#define M_GEN_IMPL_perspective(_s,_char) \
M_GEN_perspective(_s,_char){ \
    m##_char##_t tan_half_fov_y = (m##_char##_t)(1.0) / M##_char##TAN(fov_y * (m##_char##_t)(0.5)); \
    result->v[0] = (m##_char##_t)(1.0) / aspect * tan_half_fov_y; \
    result->v[1] = (m##_char##_t)(0.0); \
    result->v[2] = (m##_char##_t)(0.0); \
    result->v[3] = (m##_char##_t)(0.0); \
    result->v[4] = (m##_char##_t)(0.0); \
    result->v[5] = (m##_char##_t)(1.0) / tan_half_fov_y; \
    result->v[6] = (m##_char##_t)(0.0); \
    result->v[7] = (m##_char##_t)(0.0); \
    result->v[8] = (m##_char##_t)(0.0); \
    result->v[9] = (m##_char##_t)(0.0); \
    result->v[10] = f / (n - f); \
    result->v[11] = -(m##_char##_t)(1.0); \
    result->v[12] = (m##_char##_t)(0.0); \
    result->v[13] = (m##_char##_t)(0.0); \
    result->v[14] = -(f * n) / (f - n); \
    result->v[15] = (m##_char##_t)(0.0); \
    return result; \
}

M_GEN_IMPL_perspective(mat4,f);
M_GEN_IMPL_perspective(mat4,d);
#endif

#ifdef M_GEN_perspective_fov
#define M_GEN_IMPL_perspective_fov(_s,_char) \
M_GEN_perspective_fov(_s,_char){ \
    m##_char##_t h2 = M##_char##COS(fov * (m##_char##_t)(0.5)) / M##_char##SIN(fov * (m##_char##_t)(0.5)); \
    m##_char##_t w2 = h2 * h / w; \
    result->v[0] = w2; \
    result->v[1] = (m##_char##_t)(0.0); \
    result->v[2] = (m##_char##_t)(0.0); \
    result->v[3] = (m##_char##_t)(0.0); \
    result->v[4] = (m##_char##_t)(0.0); \
    result->v[5] = h2; \
    result->v[6] = (m##_char##_t)(0.0); \
    result->v[7] = (m##_char##_t)(0.0); \
    result->v[8] = (m##_char##_t)(0.0); \
    result->v[9] = (m##_char##_t)(0.0); \
    result->v[10] = f / (n - f); \
    result->v[11] = -(m##_char##_t)(1.0); \
    result->v[12] = (m##_char##_t)(0.0); \
    result->v[13] = (m##_char##_t)(0.0); \
    result->v[14] = -(f * n) / (f - n); \
    result->v[15] = (m##_char##_t)(0.0); \
    return result; \
}

M_GEN_IMPL_perspective_fov(mat4,f);
M_GEN_IMPL_perspective_fov(mat4,d);
#endif

#ifdef M_GEN_perspective_infinite
#define M_GEN_IMPL_perspective_infinite(_s,_char) \
M_GEN_perspective_infinite(_s,_char){ \
    m##_char##_t range = M##_char##TAN(fov_y * (m##_char##_t)(0.5)) * n; \
    m##_char##_t left = -range * aspect; \
    m##_char##_t right = range * aspect; \
    m##_char##_t top = range; \
    m##_char##_t bottom = -range; \
    result->v[0] = (m##_char##_t)(2.0) * n / (right - left); \
    result->v[1] = (m##_char##_t)(0.0); \
    result->v[2] = (m##_char##_t)(0.0); \
    result->v[3] = (m##_char##_t)(0.0); \
    result->v[4] = (m##_char##_t)(0.0); \
    result->v[5] = (m##_char##_t)(2.0) * n / (top - bottom); \
    result->v[6] = (m##_char##_t)(0.0); \
    result->v[7] = (m##_char##_t)(0.0); \
    result->v[8] = (m##_char##_t)(0.0); \
    result->v[9] = (m##_char##_t)(0.0); \
    result->v[10] = -(m##_char##_t)(1.0); \
    result->v[11] = -(m##_char##_t)(1.0); \
    result->v[12] = (m##_char##_t)(0.0); \
    result->v[13] = (m##_char##_t)(0.0); \
    result->v[14] = -(m##_char##_t)(2.0) * n; \
    result->v[15] = (m##_char##_t)(0.0); \
    return result; \
}

M_GEN_IMPL_perspective_infinite(mat4,f);
M_GEN_IMPL_perspective_infinite(mat4,d);
#endif

#ifdef M_GEN_rect_extend
#define M_GEN_IMPL_rect_extend \
M_GEN_rect_extend { \
    struct vec3i vct = {f,f,f}; \
    vec3i_subtract(&result->min, &vct); \
    vec3i_add(&result->max, &vct); \
    return result; \
}

M_GEN_IMPL_rect_extend;
#endif

#ifdef M_GEN_triface_calc_norm
#define M_GEN_IMPL_triface_calc_norm(_char) \
M_GEN_triface_calc_norm(_char) { \
    struct vec3##_char tmp_v1 = t0->p[0]; \
    struct vec3##_char tmp_v2 = t0->p[1]; \
    vec3##_char##_subtract(&tmp_v1, &t0->p[1]); \
    vec3##_char##_subtract(&tmp_v2, &t0->p[2]); \
    vec3##_char##_cross(&tmp_v1, &tmp_v1, &tmp_v2); \
    t0->normal = *VEC3_CAST(d,&tmp_v1); \
    return t0; \
}

M_GEN_IMPL_triface_calc_norm(i);
#endif

#ifdef M_GEN_extremes_find

M_GEN_extremes_find {
    *result = (struct extremes3i){.v = {points,points,points,points,points,points}};

    #define SET_MAX(axis) \
        if (it->axis > result->max_##axis->axis){ \
            result->max_##axis=it; \
        }
    #define SET_MIN(axis) \
        if (it->axis < result->min_##axis->axis){ \
            result->min_##axis=it; \
        }

    cvector_iterator(struct vec3i) it;
    cvector_for_each_in(it, points){
        SET_MIN(x);SET_MAX(x);
        SET_MIN(y);SET_MAX(y);
        SET_MIN(z);SET_MAX(z);
    }
    return result;
}

#endif

#ifdef M_GEN_convex_hull_update
struct convex_plane {
    struct triface3i triface;
    cvector(struct vec3i) outside_points;
};

// Calculating the horizon for an eye to make new faces
bool calc_horizon(
    cvector_vector_type(int) all_planes,
    cvector_vector_type(int) visited_planes,
    int current_plane,
    struct vec3i* eye_point,
    cvector_vector_type(struct segment3i) segments,
    cvector_vector_type(struct convex_plane) plane_storage
    )
{
    cvector_iterator(int) plane;
    double dist =  triface3i_distance_vec3i( &plane_storage[current_plane].triface, eye_point);

    if( dist > 0){
        cvector_push_back(visited_planes, current_plane);
        for(int i = 0; i < 3; i++){

            // Edge
            struct vec3i* e1 = &plane_storage[current_plane].triface.p[i];
            struct vec3i* e2;
            if (i < 2){e2 = &plane_storage[current_plane].triface.p[i+1];}else{e2 = &plane_storage[current_plane].triface.p[0];}

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
                    if(memcmp(e1,&plane_storage[*plane].triface.p[i],sizeof(struct vec3i)) == 0){
                        e1_found = true;

                    }
                    if(not e2_found)
                    if(memcmp(e2,&plane_storage[*plane].triface.p[i],sizeof(struct vec3i)) == 0){
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
                    cvector_push_back(segments, ((struct segment3i){.start=*e1, .end=*e2}) );
                }
            }
        }
        return false;
    }else{
        return true;
    }
}

M_GEN_convex_hull_update {
    // Delete old data
    cvector_free(result->tris);
    result->tris = NULL;

    // If no extremes then find them
    struct extremes3i tmp_extremes;
    struct extremes3i* pextremes = (struct extremes3i*)extremes;

    if ( not extremes ) {
        pextremes = &tmp_extremes;
        extremes3i_find(pextremes, points);
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
            double dist = vec3i_distance_squared_vec3i(pextremes->v[i],pextremes->v[j]);
            if(dist > max_dist){
                max_dist = dist;
                start_points[0] = pextremes->v[i];
                start_points[1] = pextremes->v[j];
            }
        }
    }

    if (max_dist == 0){
        throw(M_ERROR_CALCULATION);
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

        struct vec3i dir = *start_points[1];
        vec3i_subtract(&dir, start_points[0]);
        error_code = 0;
        errorcb_t old_callback = error_callback;
        error_callback = NULL;
        double dist = line3i_distance_squared_vec3i(&(struct line3i){.direction = *VEC3_CAST(d,&dir), .point=*start_points[0]}, point);
        error_callback = old_callback;

        if(error_code){continue;}
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
            .triface=(struct triface3i){.p={*start_points[0],*start_points[1],*start_points[2]}},
            .outside_points = NULL,
        }));
    cvector_init(cvector_back(plane_storage)->outside_points, 20, NULL);
    struct convex_plane* cp = cvector_back(plane_storage);
    triface3i_calc_norm(&cp->triface);

    // Fourth point
    max_dist = 0;
    cvector_for_each_in(point, (cvector_vector_type(struct vec3i))points){
        double dist = triface3i_distance_vec3i(&cp->triface, point);
        if ( fabs(dist) > max_dist ){
            max_dist = dist;
            start_points[3] = point;
        }
    }

    // Other 3 planes
    cvector_push_back(plane_storage,
        ((struct convex_plane){
            .triface=(struct triface3i){.p={*start_points[0],*start_points[1],*start_points[3]}},
            .outside_points = NULL,
        }));
    cvector_init(cvector_back(plane_storage)->outside_points, 20, NULL);
    triface3i_calc_norm(&cvector_back(plane_storage)->triface);

    cvector_push_back(plane_storage,
        ((struct convex_plane){
            .triface=(struct triface3i){.p={*start_points[0],*start_points[3],*start_points[2]}},
            .outside_points = NULL,
        }));
    cvector_init(cvector_back(plane_storage)->outside_points, 20, NULL);
    triface3i_calc_norm(&cvector_back(plane_storage)->triface);

    cvector_push_back(plane_storage,
        ((struct convex_plane){
            .triface=(struct triface3i){.p={*start_points[1],*start_points[2],*start_points[3]}},
            .outside_points = NULL,
        }));
    cvector_init(cvector_back(plane_storage)->outside_points, 20, NULL);
    triface3i_calc_norm(&cvector_back(plane_storage)->triface);


    cvector_iterator(struct convex_plane) stored_plane;
    cvector_for_each_in(stored_plane, plane_storage){
        // Correct normals
        for (int pi = 0; pi < 4; pi++){
            struct vec3i tmp_v = *start_points[pi];
            vec3i_subtract(&tmp_v, &stored_plane->triface.p[0]);
            struct vec3d tmp_vf = {.x=tmp_v.x, .y=tmp_v.y, .z=tmp_v.z};
            double dist = vec3d_dot(&stored_plane->triface.normal, &tmp_vf);
            if(dist > 0){
                vec3d_multiply_num(&stored_plane->triface.normal, &stored_plane->triface.normal, -1);
            }
        }

        // Find all outside points
        cvector_for_each_in(point, (cvector_vector_type(struct vec3i))points){
            double dist = triface3i_distance_vec3i(&stored_plane->triface, point);

            if (dist > 0){
                cvector_push_back(stored_plane->outside_points,*point);
            }
        }
    }

    cvector_vector_type(int) visited_planes         = NULL;
    cvector_vector_type(int) planes                 = NULL;
    cvector_vector_type(struct segment3i) segments  = NULL;

    cvector_reserve(visited_planes, 40);
    cvector_reserve(planes, 40);
    cvector_reserve(segments, 40);

    cvector_iterator(struct segment3i) segment;
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
                    double dist = triface3i_distance_vec3i(&plane_storage[*plane].triface, point);
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
                        .triface={.p={
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

                    triface3i_calc_norm(&new_plane->triface);

                    // Correct normal new plane
                    for (int pi = 0; pi < 4; pi++){
                        struct vec3i tmp_v = *start_points[pi];
                        vec3i_subtract(&tmp_v, &new_plane->triface.p[0]);
                        struct vec3d tmp_vf = {.x=tmp_v.x, .y=tmp_v.y, .z=tmp_v.z};
                        double dist = vec3d_dot(&new_plane->triface.normal, &tmp_vf);
                        if(dist > 0){
                            vec3d_multiply_num(&new_plane->triface.normal, &new_plane->triface.normal, -1);
                        }
                    }

                    // Find all outside points for new plane
                    cvector_for_each_in(visited_plane, visited_planes){
                        cvector_for_each_in(point, plane_storage[*visited_plane].outside_points){
                            double dist = triface3i_distance_vec3i(&new_plane->triface, point);
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
    cvector_iterator(struct triface3i) tris;
    cvector_init(result->tris, cvector_size(planes), NULL);
    cvector_for_each_in(plane, planes){
        cvector_push_back(result->tris, plane_storage[*plane].triface);
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

#endif

#ifdef M_GEN_vec_inside_convex_hull
M_GEN_vec_inside_convex_hull {
    cvector_iterator(struct triface3i) it;
    cvector_for_each_in(it, c0->tris){
        double dist = triface3i_distance_vec3i(it, v0);
        if(dist > 0){
            return false;
        }
    }
    return true;
}

#endif

#ifdef M_GEN_quadratic_ease_out
#define M_GEN_IMPL_quadratic_ease_out(_char) \
M_GEN_quadratic_ease_out(_char){ \
    return -f * (f - (m##_char##_t)(2.0)); \
}

M_GEN_IMPL_quadratic_ease_out(f);
M_GEN_IMPL_quadratic_ease_out(d);
#endif

#ifdef M_GEN_quadratic_ease_in
#define M_GEN_IMPL_quadratic_ease_in(_char) \
M_GEN_quadratic_ease_in(_char){ \
    return f * f; \
}

M_GEN_IMPL_quadratic_ease_in(f);
M_GEN_IMPL_quadratic_ease_in(d);
#endif

#ifdef M_GEN_quadratic_ease_in_out
#define M_GEN_IMPL_quadratic_ease_in_out(_char) \
M_GEN_quadratic_ease_in_out(_char){ \
    m##_char##_t a = (m##_char##_t)(0.0); \
    if (f < (m##_char##_t)(0.5)) { \
        a = (m##_char##_t)(2.0) * f * f; \
    } else { \
        a = -(m##_char##_t)(2.0) * f * f + (m##_char##_t)(4.0) * f - (m##_char##_t)(1.0); \
    } \
    return a; \
}

M_GEN_IMPL_quadratic_ease_in_out(f);
M_GEN_IMPL_quadratic_ease_in_out(d);
#endif

#ifdef M_GEN_cubic_ease_out
#define M_GEN_IMPL_cubic_ease_out(_char) \
M_GEN_cubic_ease_out(_char){ \
    m##_char##_t a = f - (m##_char##_t)(1.0); \
    return a * a * a + (m##_char##_t)(1.0); \
}

M_GEN_IMPL_cubic_ease_out(f);
M_GEN_IMPL_cubic_ease_out(d);
#endif

#ifdef M_GEN_cubic_ease_in
#define M_GEN_IMPL_cubic_ease_in(_char) \
M_GEN_cubic_ease_in(_char){ \
    return f * f * f; \
}

M_GEN_IMPL_cubic_ease_in(f);
M_GEN_IMPL_cubic_ease_in(d);
#endif

#ifdef M_GEN_cubic_ease_in_out
#define M_GEN_IMPL_cubic_ease_in_out(_char) \
M_GEN_cubic_ease_in_out(_char){ \
    m##_char##_t a = (m##_char##_t)(0.0); \
    if (f < (m##_char##_t)(0.5)) { \
        a = (m##_char##_t)(4.0) * f * f * f; \
    } else { \
        a = (m##_char##_t)(2.0) * f - (m##_char##_t)(2.0); \
        a = (m##_char##_t)(0.5) * a * a * a + (m##_char##_t)(1.0); \
    } \
    return a; \
}

M_GEN_IMPL_cubic_ease_in_out(f);
M_GEN_IMPL_cubic_ease_in_out(d);
#endif

#ifdef M_GEN_quartic_ease_out
#define M_GEN_IMPL_quartic_ease_out(_char) \
M_GEN_quartic_ease_out(_char){ \
    m##_char##_t a = f - (m##_char##_t)(1.0); \
    return a * a * a * ((m##_char##_t)(1.0) - f) + (m##_char##_t)(1.0); \
}

M_GEN_IMPL_quartic_ease_out(f);
M_GEN_IMPL_quartic_ease_out(d);
#endif

#ifdef M_GEN_quartic_ease_in
#define M_GEN_IMPL_quartic_ease_in(_char) \
M_GEN_quartic_ease_in(_char){ \
    return f * f * f * f; \
}

M_GEN_IMPL_quartic_ease_in(f);
M_GEN_IMPL_quartic_ease_in(d);
#endif

#ifdef M_GEN_quartic_ease_in_out
#define M_GEN_IMPL_quartic_ease_in_out(_char) \
M_GEN_quartic_ease_in_out(_char){ \
    m##_char##_t a = (m##_char##_t)(0.0); \
    if (f < (m##_char##_t)(0.5)) { \
        a = (m##_char##_t)(8.0) * f * f * f * f; \
    } else { \
        a = f - (m##_char##_t)(1.0); \
        a = -(m##_char##_t)(8.0) * a * a * a * a + (m##_char##_t)(1.0); \
    } \
    return a; \
}

M_GEN_IMPL_quartic_ease_in_out(f);
M_GEN_IMPL_quartic_ease_in_out(d);
#endif

#ifdef M_GEN_quintic_ease_out
#define M_GEN_IMPL_quintic_ease_out(_char) \
M_GEN_quintic_ease_out(_char){ \
    m##_char##_t a = f - (m##_char##_t)(1.0); \
    return a * a * a * a * a + (m##_char##_t)(1.0); \
}

M_GEN_IMPL_quintic_ease_out(f);
M_GEN_IMPL_quintic_ease_out(d);
#endif

#ifdef M_GEN_quintic_ease_in
#define M_GEN_IMPL_quintic_ease_in(_char) \
M_GEN_quintic_ease_in(_char){ \
    return f * f * f * f * f; \
}

M_GEN_IMPL_quintic_ease_in(f);
M_GEN_IMPL_quintic_ease_in(d);
#endif

#ifdef M_GEN_quintic_ease_in_out
#define M_GEN_IMPL_quintic_ease_in_out(_char) \
M_GEN_quintic_ease_in_out(_char){ \
    m##_char##_t a = (m##_char##_t)(0.0); \
    if (f < (m##_char##_t)(0.5)) { \
        a = (m##_char##_t)(16.0) * f * f * f * f * f; \
    } else { \
        a = (m##_char##_t)(2.0) * f - (m##_char##_t)(2.0); \
        a = (m##_char##_t)(0.5) * a * a * a * a * a + (m##_char##_t)(1.0); \
    } \
    return a; \
}

M_GEN_IMPL_quintic_ease_in_out(f);
M_GEN_IMPL_quintic_ease_in_out(d);
#endif

#ifdef M_GEN_sine_ease_out
#define M_GEN_IMPL_sine_ease_out(_char) \
M_GEN_sine_ease_out(_char){ \
    return M##_char##SIN(f * M##_char##PI_2); \
}

M_GEN_IMPL_sine_ease_out(f);
M_GEN_IMPL_sine_ease_out(d);
#endif

#ifdef M_GEN_sine_ease_in
#define M_GEN_IMPL_sine_ease_in(_char) \
M_GEN_sine_ease_in(_char){ \
    return M##_char##SIN((f - (m##_char##_t)(1.0)) * M##_char##PI_2) + (m##_char##_t)(1.0); \
}

M_GEN_IMPL_sine_ease_in(f);
M_GEN_IMPL_sine_ease_in(d);
#endif

#ifdef M_GEN_sine_ease_in_out
#if defined(CGEOM_PRECISE_TRIGONOMETRY)
#define M_GEN_IMPL_sine_ease_in_out(_char) \
M_GEN_sine_ease_in_out(_char){ \
    sincos1cos sc1c = sncs1cs(f * M##_char##PI); \
    return (m##_char##_t)(0.5) * sc1c.omc; \
}
#else
#define M_GEN_IMPL_sine_ease_in_out(_char) \
M_GEN_sine_ease_in_out(_char){ \
    return (m##_char##_t)(0.5) * ((m##_char##_t)(1.0) - M##_char##COS(f * M##_char##PI)); \
}
#endif

M_GEN_IMPL_sine_ease_in_out(f);
M_GEN_IMPL_sine_ease_in_out(d);
#endif

#ifdef M_GEN_circular_ease_out
#define M_GEN_IMPL_circular_ease_out(_char) \
M_GEN_circular_ease_out(_char){ \
    return M##_char##SQRT(((m##_char##_t)(2.0) - f) * f); \
}

M_GEN_IMPL_circular_ease_out(f);
M_GEN_IMPL_circular_ease_out(d);
#endif

#ifdef M_GEN_circular_ease_in
#define M_GEN_IMPL_circular_ease_in(_char) \
M_GEN_circular_ease_in(_char){ \
    return (m##_char##_t)(1.0) - M##_char##SQRT((m##_char##_t)(1.0) - (f * f)); \
}

M_GEN_IMPL_circular_ease_in(f);
M_GEN_IMPL_circular_ease_in(d);
#endif

#ifdef M_GEN_circular_ease_in_out
#define M_GEN_IMPL_circular_ease_in_out(_char) \
M_GEN_circular_ease_in_out(_char){ \
    m##_char##_t a = (m##_char##_t)(0.0); \
    if (f < (m##_char##_t)(0.5)) { \
        a = (m##_char##_t)(0.5) * ((m##_char##_t)(1.0) - M##_char##SQRT((m##_char##_t)(1.0) - (m##_char##_t)(4.0) * f * f)); \
    } else { \
        a = (m##_char##_t)(0.5) * (M##_char##SQRT(-((m##_char##_t)(2.0) * f - (m##_char##_t)(3.0)) * ((m##_char##_t)(2.0) * f - (m##_char##_t)(1.0))) + (m##_char##_t)(1.0)); \
    } \
    return a; \
}

M_GEN_IMPL_circular_ease_in_out(f);
M_GEN_IMPL_circular_ease_in_out(d);
#endif

#ifdef M_GEN_exponential_ease_out
#define M_GEN_IMPL_exponential_ease_out(_char) \
M_GEN_exponential_ease_out(_char){ \
    m##_char##_t a = f; \
    if (M##_char##FABS(a) > M##_char##EPSILON) { \
        a = (m##_char##_t)(1.0) - M##_char##POW((m##_char##_t)(2.0), -(m##_char##_t)(10.0) * f); \
    } \
    return a; \
}

M_GEN_IMPL_exponential_ease_out(f);
M_GEN_IMPL_exponential_ease_out(d);
#endif

#ifdef M_GEN_exponential_ease_in
#define M_GEN_IMPL_exponential_ease_in(_char) \
M_GEN_exponential_ease_in(_char){ \
    m##_char##_t a = f; \
    if (M##_char##FABS(a) > M##_char##EPSILON) { \
        a = M##_char##POW((m##_char##_t)(2.0), (m##_char##_t)(10.0) * (f - (m##_char##_t)(1.0))); \
    } \
    return a; \
}

M_GEN_IMPL_exponential_ease_in(f);
M_GEN_IMPL_exponential_ease_in(d);
#endif

#ifdef M_GEN_exponential_ease_in_out
#define M_GEN_IMPL_exponential_ease_in_out(_char) \
M_GEN_exponential_ease_in_out(_char){ \
    m##_char##_t a = f; \
    if (f < (m##_char##_t)(0.5)) { \
        a = (m##_char##_t)(0.5) * M##_char##POW((m##_char##_t)(2.0), ((m##_char##_t)(20.0) * f) - (m##_char##_t)(10.0)); \
    } else { \
        a = -(m##_char##_t)(0.5) * M##_char##POW((m##_char##_t)(2.0), -(m##_char##_t)(20.0) * f + (m##_char##_t)(10.0)) + (m##_char##_t)(1.0); \
    } \
    return a; \
}

M_GEN_IMPL_exponential_ease_in_out(f);
M_GEN_IMPL_exponential_ease_in_out(d);
#endif

#ifdef M_GEN_elastic_ease_out
#define M_GEN_IMPL_elastic_ease_out(_char) \
M_GEN_elastic_ease_out(_char){ \
    return M##_char##SIN(-(m##_char##_t)(13.0) * M##_char##PI_2 * (f + (m##_char##_t)(1.0))) * M##_char##POW((m##_char##_t)(2.0), -(m##_char##_t)(10.0) * f) + (m##_char##_t)(1.0); \
}

M_GEN_IMPL_elastic_ease_out(f);
M_GEN_IMPL_elastic_ease_out(d);
#endif

#ifdef M_GEN_elastic_ease_in
#define M_GEN_IMPL_elastic_ease_in(_char) \
M_GEN_elastic_ease_in(_char){ \
    return M##_char##SIN((m##_char##_t)(13.0) * M##_char##PI_2 * f) * M##_char##POW((m##_char##_t)(2.0), (m##_char##_t)(10.0) * (f - (m##_char##_t)(1.0))); \
}

M_GEN_IMPL_elastic_ease_in(f);
M_GEN_IMPL_elastic_ease_in(d);
#endif

#ifdef M_GEN_elastic_ease_in_out
#define M_GEN_IMPL_elastic_ease_in_out(_char) \
M_GEN_elastic_ease_in_out(_char){ \
    m##_char##_t a = (m##_char##_t)(0.0); \
    if (f < (m##_char##_t)(0.5)) { \
        a = (m##_char##_t)(0.5) * M##_char##SIN((m##_char##_t)(13.0) * M##_char##PI_2 * ((m##_char##_t)(2.0) * f)) * M##_char##POW((m##_char##_t)(2.0), (m##_char##_t)(10.0) * (((m##_char##_t)(2.0) * f) - (m##_char##_t)(1.0))); \
    } else { \
        a = (m##_char##_t)(0.5) * (M##_char##SIN(-(m##_char##_t)(13.0) * M##_char##PI_2 * (((m##_char##_t)(2.0) * f - (m##_char##_t)(1.0)) + (m##_char##_t)(1.0))) * M##_char##POW((m##_char##_t)(2.0), -(m##_char##_t)(10.0) * ((m##_char##_t)(2.0) * f - (m##_char##_t)(1.0))) + (m##_char##_t)(2.0)); \
    } \
    return a; \
}

M_GEN_IMPL_elastic_ease_in_out(f);
M_GEN_IMPL_elastic_ease_in_out(d);
#endif

#ifdef M_GEN_back_ease_out
#define M_GEN_IMPL_back_ease_out(_char) \
M_GEN_back_ease_out(_char){ \
    m##_char##_t a = (m##_char##_t)(1.0) - f; \
    return (m##_char##_t)(1.0) - (a * a * a - a * M##_char##SIN(a * M##_char##PI)); \
}

M_GEN_IMPL_back_ease_out(f);
M_GEN_IMPL_back_ease_out(d);
#endif

#ifdef M_GEN_back_ease_in
#define M_GEN_IMPL_back_ease_in(_char) \
M_GEN_back_ease_in(_char){ \
    return f * f * f - f * M##_char##SIN(f * M##_char##PI); \
}

M_GEN_IMPL_back_ease_in(f);
M_GEN_IMPL_back_ease_in(d);
#endif

#ifdef M_GEN_back_ease_in_out
#define M_GEN_IMPL_M_GEN_back_ease_in_out(_char) \
M_GEN_back_ease_in_out(_char){ \
    m##_char##_t a = (m##_char##_t)(0.0); \
    if (f < (m##_char##_t)(0.5)) { \
        a = (m##_char##_t)(2.0) * f; \
        a = (m##_char##_t)(0.5) * (a * a * a - a * M##_char##SIN(a * M##_char##PI)); \
    } else { \
        a = ((m##_char##_t)(1.0) - ((m##_char##_t)(2.0) * f - (m##_char##_t)(1.0))); \
        a = (m##_char##_t)(0.5) * ((m##_char##_t)(1.0) - (a * a * a - a * M##_char##SIN(f * M##_char##PI))) + (m##_char##_t)(0.5); \
    } \
    return a; \
}

M_GEN_IMPL_M_GEN_back_ease_in_out(f);
M_GEN_IMPL_M_GEN_back_ease_in_out(d);
#endif

#ifdef M_GEN_bounce_ease_out
#define M_GEN_IMPL_bounce_ease_out(_char) \
M_GEN_bounce_ease_out(_char){ \
    m##_char##_t a = (m##_char##_t)(0.0); \
    if (f < (m##_char##_t)(4.0) / (m##_char##_t)(11.0)) { \
        a = ((m##_char##_t)(121.0) * f * f) / (m##_char##_t)(16.0); \
    } else if (f < (m##_char##_t)(8.0) / (m##_char##_t)(11.0)) { \
        a = ((m##_char##_t)(363.0) / (m##_char##_t)(40.0) * f * f) - ((m##_char##_t)(99.0) / (m##_char##_t)(10.0) * f) + (m##_char##_t)(17.0) / (m##_char##_t)(5.0); \
    } else if (f < (m##_char##_t)(9.0) / (m##_char##_t)(10.0)) { \
        a = ((m##_char##_t)(4356.0) / (m##_char##_t)(361.0) * f * f) - ((m##_char##_t)(35442.0) / (m##_char##_t)(1805.0) * f) + (m##_char##_t)(16061.0) / (m##_char##_t)(1805.0); \
    } else { \
        a = ((m##_char##_t)(54.0) / (m##_char##_t)(5.0) * f * f) - ((m##_char##_t)(513.0) / (m##_char##_t)(25.0) * f) + (m##_char##_t)(268.0) / (m##_char##_t)(25.0); \
    } \
    return a; \
}

M_GEN_IMPL_bounce_ease_out(f);
M_GEN_IMPL_bounce_ease_out(d);
#endif

#ifdef M_GEN_bounce_ease_in
#define M_GEN_IMPL_bounce_ease_in(_char) \
M_GEN_bounce_ease_in(_char){ \
    return (m##_char##_t)(1.0) - bounce_ease_out##_char((m##_char##_t)(1.0) - f); \
}

M_GEN_IMPL_bounce_ease_in(f);
M_GEN_IMPL_bounce_ease_in(d);
#endif

#ifdef M_GEN_bounce_ease_in_out
#define M_GEN_IMPL_bounce_ease_in_out(_char) \
M_GEN_bounce_ease_in_out(_char){ \
    m##_char##_t a = (m##_char##_t)(0.0); \
    if (f < (m##_char##_t)(0.5)) { \
        a = (m##_char##_t)(0.5) * bounce_ease_in##_char(f * (m##_char##_t)(2.0)); \
    } else { \
        a = (m##_char##_t)(0.5) * bounce_ease_out##_char(f * (m##_char##_t)(2.0) - (m##_char##_t)(1.0)) + (m##_char##_t)(0.5); \
    } \
    return a; \
}

M_GEN_IMPL_bounce_ease_in_out(f);
M_GEN_IMPL_bounce_ease_in_out(d);
#endif
