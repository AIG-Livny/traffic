#include "cgeom.h"

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

typedef struct {
    /* Sine, cosine, 1-cosine */
    mreal_t sin, cos, omc;
} sincos1cos;

static
sincos1cos sncs1cs(mreal_t x) {
    const mreal_t fourOverPi = 1.27323954473516268615;

    mreal_t y, z, zz;
    mint_t j, sign = 1, csign = 1;
    sincos1cos r;

    /* Handle +-0. */
    if (x == (mreal_t)0) {
        r.sin = x;
        r.cos = 1;
        r.omc = 0;
        return r;
    }
    if (isnan(x)) {
        r.sin = r.cos = r.omc = x;
        return r;
    }
    if (isinf(x)) {
        r.sin = r.cos = r.omc = x - x;
        return r;
    }
    if (x < 0) {
        sign = -1;
        x = -x;
    }
    j = (mint_t)(x * fourOverPi);
    y = (mreal_t)j;
    /* map zeros to origin */
    if ((j & 1)) {
        j += 1;
        y += 1;
    }
    j = j & 7; /* octant modulo one turn */
    /* reflect in x axis */
    if (j > 3) {
        sign = -sign;
        csign = -csign;
        j -= 4;
    }
    if (j > 1) {
        csign = -csign;
    }

# if defined(CGEOM_REAL_TYPE_FLOAT)
    const float sc[] = {-1.9515295891E-4, 8.3321608736E-3, -1.6666654611E-1};

    const float cc[] = {2.443315711809948E-5, -1.388731625493765E-3, 4.166664568298827E-2};

    /* These are for a 24-bit significand: */
    const float DP1 = 0.78515625;
    const float DP2 = 2.4187564849853515625e-4;
    const float DP3 = 3.77489497744594108e-8;

    /* Extended precision modular arithmetic */
    z = ((x - y * DP1) - y * DP2) - y * DP3;
    zz = z * z;
    r.sin = z + zz*z*((sc[0]*zz + sc[1])*zz + sc[2]);
    r.omc = (mreal_t)0.5*zz - zz*zz*((cc[0]*zz + cc[1])*zz + cc[2]);
# else
    const double sc[] = {
        1.58962301576546568060E-10,
        -2.50507477628578072866E-8,
        2.75573136213857245213E-6,
        -1.98412698295895385996E-4,
        8.33333333332211858878E-3,
        -1.66666666666666307295E-1,
    };

    const double cc[] = {
        -1.13585365213876817300E-11,
        2.08757008419747316778E-9,
        -2.75573141792967388112E-7,
        2.48015872888517045348E-5,
        -1.38888888888730564116E-3,
        4.16666666666665929218E-2,
    };

    const double DP1 = 7.85398125648498535156E-1;
    const double DP2 = 3.77489470793079817668E-8;
    const double DP3 = 2.69515142907905952645E-15;

    /* Extended precision modular arithmetic */
    z = ((x - y * DP1) - y * DP2) - y * DP3;
    zz = z * z;
    r.sin = z + zz*z*(((((sc[0]*zz + sc[1])*zz + sc[2])*zz + sc[3])*zz + sc[4])*zz + sc[5]);
    r.omc = (mreal_t)0.5*zz - zz*zz*(((((cc[0]*zz + cc[1])*zz + cc[2])*zz + cc[3])*zz + cc[4])*zz + cc[5]);
# endif

    if (j == 1 || j == 2) {
        if (csign < 0) {
            r.sin = -r.sin;
        }
        r.cos = r.sin;
        r.sin = 1 - r.omc;
        r.omc = 1 - r.cos;
    } else {
        if (csign < 0) {
            r.cos = r.omc - 1;
            r.omc = 1 - r.cos;
        } else {
            r.cos = 1 - r.omc;
        }
    }
    if (sign < 0) {
        r.sin = -r.sin;
    }
    return r;
}
#endif

#define M_GEN_IMPL_nearly_equal(_t,_char)               \
M_GEN_nearly_equal(_t,_char){                           \
    bool result = false;                                \
    if (a == b) {                                       \
        result = true;                                  \
    } else if (M##_char##FABS(a - b) <= epsilon) {      \
        result = true;                                  \
    }                                                   \
    return result;                                      \
}

M_GEN_IMPL_nearly_equal(mfloat_t,f);
M_GEN_IMPL_nearly_equal(mdouble_t,d);


#define M_GEN_IMPL_clamp(_t,_char)  \
M_GEN_clamp(_t,_char) {             \
    if (value < min) {              \
        value = min;                \
    } else if (value > max) {       \
        value = max;                \
    }                               \
    return value;                   \
}

M_GEN_IMPL_clamp(mint_t,i);
M_GEN_IMPL_clamp(mfloat_t,f);
M_GEN_IMPL_clamp(mdouble_t,d);

#define M_GEN_IMPL_to_radians(_t,_char)             \
M_GEN_to_radians(_t,_char) {                        \
    return (degrees * M##_char##PI / (_t)180.0); \
}

M_GEN_IMPL_to_radians(mfloat_t,f);
M_GEN_IMPL_to_radians(mdouble_t,d);


#define M_GEN_IMPL_to_degrees(_t,_char)             \
M_GEN_to_degrees(_t,_char) {                        \
    return (radians * (_t)180.0 / M##_char##PI);    \
}

M_GEN_IMPL_to_degrees(mfloat_t,f);
M_GEN_IMPL_to_degrees(mdouble_t,d);

// Compare all memory with value
static int memvcmp(void *memory, unsigned char val, unsigned int size) {
    unsigned char *mm = (unsigned char*)memory;
    return (*mm == val) && memcmp(mm, mm + 1, size - 1) == 0;
}

#define M_GEN_IMPL_is_zero(_t,_char)                                                \
M_GEN_is_zero(_t,_char){                                                            \
    return memvcmp((void*)v0,0,sizeof(*_t)) == 0;                                   \
}

M_GEN_IMPL_is_zero(vec2,i);
M_GEN_IMPL_is_zero(vec3,i);
M_GEN_IMPL_is_zero(vec4,i);
M_GEN_IMPL_is_zero(quat,i);

#define M_GEN_IMPL2_is_zero(_t,_char)                                       \
M_GEN_is_zero(_t,_char) {                                                   \
    for ( int i = 0; i < sizeof(v0->v)/sizeof(typeof(v0->v[0])); i++ ) {    \
        if ( M##_char##FABS(v0->v[i]) >= M##_char##EPSILON ) {              \
            return false;                                                   \
        }                                                                   \
    }                                                                       \
    return true;                                                            \
}

M_GEN_IMPL2_is_zero(vec2,f);
M_GEN_IMPL2_is_zero(vec2,d);
M_GEN_IMPL2_is_zero(vec3,f);
M_GEN_IMPL2_is_zero(vec3,d);
M_GEN_IMPL2_is_zero(vec4,f);
M_GEN_IMPL2_is_zero(vec4,d);
M_GEN_IMPL2_is_zero(quat,f);
M_GEN_IMPL2_is_zero(quat,d);


#define M_GEN_IMPL_is_equal(_t,_char)                       \
M_GEN_is_equal(_t,_char){                                   \
    return memcmp((void*)v0, (void*)v1, sizeof(*_t)) == 0;  \
}

M_GEN_IMPL_is_equal(vec2,i);
M_GEN_IMPL_is_equal(vec3,i);
M_GEN_IMPL_is_equal(vec4,i);
M_GEN_IMPL_is_equal(quat,i);


#define ELEM_TYPE(name) typeof((name)->v[0])
#define ELEM_NUM(name) sizeof((name)->v)/sizeof(ELEM_TYPE(name))

#define M_GEN_IMPL2_is_equal(_t,_char)                                      \
M_GEN_is_equal(_t,_char) {                                                  \
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

#define M_GEN_IMPL_one(_t,_char)                    \
M_GEN_one(_t,_char) {                               \
    for ( int i = 0; i < ELEM_NUM(result); i++ ) {  \
        result->v[i] = 1;                           \
    }                                               \
    return result;                                  \
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

#define M_GEN_IMPL_sign(_t,_char)                       \
M_GEN_sign(_t,_char) {                                  \
    for ( int i = 0; i < ELEM_NUM(result); i++ ){       \
        if (v0->v[i] >  (ELEM_TYPE(result))(0.0)) {     \
            result->v[i] = (ELEM_TYPE(result))(1.0);    \
        } else if (v0->v[i] < 0) {                      \
            result->v[i] = -(ELEM_TYPE(result))(1.0);   \
        } else {                                        \
            result->v[i] = (ELEM_TYPE(result))(0.0);    \
        }                                               \
    }                                                   \
    return result;                                      \
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



mreal_t *vec2_add(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] + v1[0];
    result[1] = v0[1] + v1[1];
    return result;
}


mint_t *vec2i_add(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] + v1[0];
    result[1] = v0[1] + v1[1];
    return result;
}

mint_t *vec2i_add_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] + i;
    result[1] = v0[1] + i;
    return result;
}

mint_t *vec2i_subtract(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] - v1[0];
    result[1] = v0[1] - v1[1];
    return result;
}

mint_t *vec2i_subtract_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] - i;
    result[1] = v0[1] - i;
    return result;
}

mint_t *vec2i_multiply(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] * v1[0];
    result[1] = v0[1] * v1[1];
    return result;
}

mint_t *vec2i_multiply_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] * i;
    result[1] = v0[1] * i;
    return result;
}

mint_t *vec2i_divide(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] / v1[0];
    result[1] = v0[1] / v1[1];
    return result;
}

mint_t *vec2i_divide_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] / i;
    result[1] = v0[1] / i;
    return result;
}

mint_t *vec2i_snap(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = (v0[0] / v1[0]) * v1[0];
    result[1] = (v0[1] / v1[1]) * v1[1];
    return result;
}

mint_t *vec2i_snap_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = (v0[0] / i) * i;
    result[1] = (v0[1] / i) * i;
    return result;
}

mint_t *vec2i_negative(mint_t *result, const mint_t *v0) {
    result[0] = -v0[0];
    result[1] = -v0[1];
    return result;
}

mint_t *vec2i_abs(mint_t *result, const mint_t *v0) {
    result[0] = v0[0];
    if (result[0] < 0) {
        result[0] = -result[0];
    }
    result[1] = v0[1];
    if (result[1] < 0) {
        result[1] = -result[1];
    }
    return result;
}

mint_t *vec2i_max(mint_t *result, const mint_t *v0, const mint_t *v1) {
    if (v0[0] > v1[0]) {
        result[0] = v0[0];
    } else {
        result[0] = v1[0];
    }
    if (v0[1] > v1[1]) {
        result[1] = v0[1];
    } else {
        result[1] = v1[1];
    }
    return result;
}

mint_t *vec2i_min(mint_t *result, const mint_t *v0, const mint_t *v1) {
    if (v0[0] < v1[0]) {
        result[0] = v0[0];
    } else {
        result[0] = v1[0];
    }
    if (v0[1] < v1[1]) {
        result[1] = v0[1];
    } else {
        result[1] = v1[1];
    }
    return result;
}

mint_t *vec2i_clamp(mint_t *result, const mint_t *v0, const mint_t *v1, const mint_t *v2) {
    vec2i_min(result, v0, v1);
    vec2i_max(result, v0, v2);
    return result;
}

mint_t *vec2i_tangent(mint_t *result, const mint_t *v0) {
    mint_t a0 = v0[0];
    mint_t a1 = v0[1];
    result[0] = a1;
    result[1] = -a0;
    return result;
}

static const mint_t ortho_bases[24][9] = {
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

mint_t *vec3i_rotate(mint_t *result, const mint_t *v, enum mcorner_vector cv) {
#define M_EL(r,c) r*3+c
    const mint_t* matrix = (const mint_t*)&ortho_bases[cv];

    result[M_X] = matrix[M_EL(M_X,M_X)] * v[M_X] +
                  matrix[M_EL(M_Y,M_X)] * v[M_Y] +
                  matrix[M_EL(M_Z,M_X)] * v[M_Z];
    result[M_Y] = matrix[M_EL(M_X,M_Y)] * v[M_X] +
                  matrix[M_EL(M_Y,M_Y)] * v[M_Y] +
                  matrix[M_EL(M_Z,M_Y)] * v[M_Z];
    result[M_Z] = matrix[M_EL(M_X,M_Z)] * v[M_X] +
                  matrix[M_EL(M_Y,M_Z)] * v[M_Y] +
                  matrix[M_EL(M_Z,M_Z)] * v[M_Z];
    return result;
}


bool vec3i_is_zero(const mint_t *v0) {
    return v0[0] == 0 && v0[1] == 0 && v0[2] == 0;
}

bool vec3i_is_equal(const mint_t *v0, const mint_t *v1) {
    return v0[0] == v1[0] && v0[1] == v1[1] && v0[2] == v1[2];
}

mint_t *vec3i(mint_t *result, mint_t x, mint_t y, mint_t z) {
    result[0] = x;
    result[1] = y;
    result[2] = z;
    return result;
}

mint_t *vec3i_assign(mint_t *result, const mint_t *v0) {
    memcpy(result, v0, sizeof(mint_t)*VEC3_SIZE);
    return result;
}

mint_t *vec3i_assign_vec3(mint_t *result, const mreal_t *v0) {
    result[0] = v0[0];
    result[1] = v0[1];
    result[2] = v0[2];
    return result;
}

mint_t *vec3i_zero(mint_t *result) {
    memset(result,0,sizeof(mint_t)*VEC3_SIZE);
    return result;
}

mint_t *vec3i_one(mint_t *result) {
    memset(result,1,sizeof(mint_t)*VEC3_SIZE);
    return result;
}

mint_t *vec3i_sign(mint_t *result, const mint_t *v0) {
    if (v0[0] > 0) {
        result[0] = 1;
    } else if (v0[0] < 0) {
        result[0] = -1;
    } else {
        result[0] = 0;
    }
    if (v0[1] > 0) {
        result[1] = 1;
    } else if (v0[1] < 0) {
        result[1] = -1;
    } else {
        result[1] = 0;
    }
    if (v0[2] > 0) {
        result[2] = 1;
    } else if (v0[2] < 0) {
        result[2] = -1;
    } else {
        result[2] = 0;
    }
    return result;
}

mint_t *vec3i_add(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] + v1[0];
    result[1] = v0[1] + v1[1];
    result[2] = v0[2] + v1[2];
    return result;
}

mint_t *vec3i_add_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] + i;
    result[1] = v0[1] + i;
    result[2] = v0[2] + i;
    return result;
}

mint_t *vec3i_subtract(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] - v1[0];
    result[1] = v0[1] - v1[1];
    result[2] = v0[2] - v1[2];
    return result;
}

mint_t *vec3i_subtract_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] - i;
    result[1] = v0[1] - i;
    result[2] = v0[2] - i;
    return result;
}

mint_t *vec3i_multiply(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] * v1[0];
    result[1] = v0[1] * v1[1];
    result[2] = v0[2] * v1[2];
    return result;
}

mint_t *vec3i_multiply_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] * i;
    result[1] = v0[1] * i;
    result[2] = v0[2] * i;
    return result;
}

mint_t *vec3i_divide(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] / v1[0];
    result[1] = v0[1] / v1[1];
    result[2] = v0[2] / v1[2];
    return result;
}

mint_t *vec3i_divide_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] / i;
    result[1] = v0[1] / i;
    result[2] = v0[2] / i;
    return result;
}

mint_t *vec3i_snap(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = (v0[0] / v1[0]) * v1[0];
    result[1] = (v0[1] / v1[1]) * v1[1];
    result[2] = (v0[2] / v1[2]) * v1[2];
    return result;
}

mint_t *vec3i_snap_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = (v0[0] / i) * i;
    result[1] = (v0[1] / i) * i;
    result[2] = (v0[2] / i) * i;
    return result;
}

mint_t *vec3i_cross(mint_t *result, const mint_t *v0, const mint_t *v1) {
    mint_t cross[VEC3_SIZE];
    cross[0] = v0[1] * v1[2] - v0[2] * v1[1];
    cross[1] = v0[2] * v1[0] - v0[0] * v1[2];
    cross[2] = v0[0] * v1[1] - v0[1] * v1[0];
    memcpy(result,cross,sizeof(mint_t)*VEC3_SIZE);
    return result;
}

mint_t *vec3i_negative(mint_t *result, const mint_t *v0) {
    result[0] = -v0[0];
    result[1] = -v0[1];
    result[2] = -v0[2];
    return result;
}

mint_t *vec3i_abs(mint_t *result, const mint_t *v0) {
    result[0] = v0[0];
    if (result[0] < 0) {
        result[0] = -result[0];
    }
    result[1] = v0[1];
    if (result[1] < 0) {
        result[1] = -result[1];
    }
    result[2] = v0[2];
    if (result[2] < 0) {
        result[2] = -result[2];
    }
    return result;
}

mint_t *vec3i_max(mint_t *result, const mint_t *v0, const mint_t *v1) {
    if (v0[0] > v1[0]) {
        result[0] = v0[0];
    } else {
        result[0] = v1[0];
    }
    if (v0[1] > v1[1]) {
        result[1] = v0[1];
    } else {
        result[1] = v1[1];
    }
    if (v0[2] > v1[2]) {
        result[2] = v0[2];
    } else {
        result[2] = v1[2];
    }
    return result;
}

mint_t *vec3i_min(mint_t *result, const mint_t *v0, const mint_t *v1) {
    if (v0[0] < v1[0]) {
        result[0] = v0[0];
    } else {
        result[0] = v1[0];
    }
    if (v0[1] < v1[1]) {
        result[1] = v0[1];
    } else {
        result[1] = v1[1];
    }
    if (v0[2] < v1[2]) {
        result[2] = v0[2];
    } else {
        result[2] = v1[2];
    }
    return result;
}

mint_t *vec3i_clamp(mint_t *result, const mint_t *v0, const mint_t *v1, const mint_t *v2) {
    vec3i_min(result, v0, v1);
    vec3i_max(result, v0, v2);
    return result;
}

bool vec4i_is_zero(const mint_t *v0) {
    return v0[0] == 0 && v0[1] == 0 && v0[2] == 0 && v0[3] == 0;
}

bool vec4i_is_equal(const mint_t *v0, const mint_t *v1) {
    return v0[0] == v1[0] && v0[1] == v1[1] && v0[2] == v1[2] && v0[3] == v1[3];
}

mint_t *vec4i(mint_t *result, mint_t x, mint_t y, mint_t z, mint_t w) {
    result[0] = x;
    result[1] = y;
    result[2] = z;
    result[3] = w;
    return result;
}

mint_t *vec4i_assign(mint_t *result, const mint_t *v0) {
    memcpy(result, v0, sizeof(mint_t)*VEC4_SIZE);
    return result;
}

mint_t *vec4i_assign_vec4(mint_t *result, const mreal_t *v0) {
    result[0] = v0[0];
    result[1] = v0[1];
    result[2] = v0[2];
    result[3] = v0[3];
    return result;
}

mint_t *vec4i_zero(mint_t *result) {
    memset(result,0,sizeof(mint_t)*VEC4_SIZE);
    return result;
}

mint_t *vec4i_one(mint_t *result) {
    memset(result,1,sizeof(mint_t)*VEC4_SIZE);
    return result;
}

mint_t *vec4i_sign(mint_t *result, const mint_t *v0) {
    if (v0[0] > 0) {
        result[0] = 1;
    } else if (v0[0] < 0) {
        result[0] = -1;
    } else {
        result[0] = 0;
    }
    if (v0[1] > 0) {
        result[1] = 1;
    } else if (v0[1] < 0) {
        result[1] = -1;
    } else {
        result[1] = 0;
    }
    if (v0[2] > 0) {
        result[2] = 1;
    } else if (v0[2] < 0) {
        result[2] = -1;
    } else {
        result[2] = 0;
    }
    if (v0[3] > 0) {
        result[3] = 1;
    } else if (v0[3] < 0) {
        result[3] = -1;
    } else {
        result[3] = 0;
    }
    return result;
}

mint_t *vec4i_add(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] + v1[0];
    result[1] = v0[1] + v1[1];
    result[2] = v0[2] + v1[2];
    result[3] = v0[3] + v1[3];
    return result;
}

mint_t *vec4i_add_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] + i;
    result[1] = v0[1] + i;
    result[2] = v0[2] + i;
    result[3] = v0[3] + i;
    return result;
}

mint_t *vec4i_subtract(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] - v1[0];
    result[1] = v0[1] - v1[1];
    result[2] = v0[2] - v1[2];
    result[3] = v0[3] - v1[3];
    return result;
}

mint_t *vec4i_subtract_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] - i;
    result[1] = v0[1] - i;
    result[2] = v0[2] - i;
    result[3] = v0[3] - i;
    return result;
}

mint_t *vec4i_multiply(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] * v1[0];
    result[1] = v0[1] * v1[1];
    result[2] = v0[2] * v1[2];
    result[3] = v0[3] * v1[3];
    return result;
}

mint_t *vec4i_multiply_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] * i;
    result[1] = v0[1] * i;
    result[2] = v0[2] * i;
    result[3] = v0[3] * i;
    return result;
}

mint_t *vec4i_divide(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = v0[0] / v1[0];
    result[1] = v0[1] / v1[1];
    result[2] = v0[2] / v1[2];
    result[3] = v0[3] / v1[3];
    return result;
}

mint_t *vec4i_divide_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = v0[0] / i;
    result[1] = v0[1] / i;
    result[2] = v0[2] / i;
    result[3] = v0[3] / i;
    return result;
}

mint_t *vec4i_snap(mint_t *result, const mint_t *v0, const mint_t *v1) {
    result[0] = (v0[0] / v1[0]) * v1[0];
    result[1] = (v0[1] / v1[1]) * v1[1];
    result[2] = (v0[2] / v1[2]) * v1[2];
    result[3] = (v0[3] / v1[3]) * v1[3];
    return result;
}

mint_t *vec4i_snap_i(mint_t *result, const mint_t *v0, mint_t i) {
    result[0] = (v0[0] / i) * i;
    result[1] = (v0[1] / i) * i;
    result[2] = (v0[2] / i) * i;
    result[3] = (v0[3] / i) * i;
    return result;
}

mint_t *vec4i_negative(mint_t *result, const mint_t *v0) {
    result[0] = -v0[0];
    result[1] = -v0[1];
    result[2] = -v0[2];
    result[3] = -v0[3];
    return result;
}

mint_t *vec4i_abs(mint_t *result, const mint_t *v0) {
    result[0] = v0[0];
    if (result[0] < 0) {
        result[0] = -result[0];
    }
    result[1] = v0[1];
    if (result[1] < 0) {
        result[1] = -result[1];
    }
    result[2] = v0[2];
    if (result[2] < 0) {
        result[2] = -result[2];
    }
    result[3] = v0[3];
    if (result[3] < 0) {
        result[3] = -result[3];
    }
    return result;
}

mint_t *vec4i_max(mint_t *result, const mint_t *v0, const mint_t *v1) {
    if (v0[0] > v1[0]) {
        result[0] = v0[0];
    } else {
        result[0] = v1[0];
    }
    if (v0[1] > v1[1]) {
        result[1] = v0[1];
    } else {
        result[1] = v1[1];
    }
    if (v0[2] > v1[2]) {
        result[2] = v0[2];
    } else {
        result[2] = v1[2];
    }
    if (v0[3] > v1[3]) {
        result[3] = v0[3];
    } else {
        result[3] = v1[3];
    }
    return result;
}

mint_t *vec4i_min(mint_t *result, const mint_t *v0, const mint_t *v1) {
    if (v0[0] < v1[0]) {
        result[0] = v0[0];
    } else {
        result[0] = v1[0];
    }
    if (v0[1] < v1[1]) {
        result[1] = v0[1];
    } else {
        result[1] = v1[1];
    }
    if (v0[2] < v1[2]) {
        result[2] = v0[2];
    } else {
        result[2] = v1[2];
    }
    if (v0[3] < v1[3]) {
        result[3] = v0[3];
    } else {
        result[3] = v1[3];
    }
    return result;
}

mint_t *vec4i_clamp(mint_t *result, const mint_t *v0, const mint_t *v1, const mint_t *v2) {
    vec4i_min(result, v0, v1);
    vec4i_max(result, v0, v2);
    return result;
}

bool vec2_is_zero(const mreal_t *v0) {
    return MFABS(v0[0]) < MREAL_EPSILON && MFABS(v0[1]) < MREAL_EPSILON;
}

bool vec2_is_equal(const mreal_t *v0, const mreal_t *v1) {
    return MFABS(v0[0] - v1[0]) < MREAL_EPSILON && MFABS(v0[1] - v1[1]) < MREAL_EPSILON;
}

mreal_t *vec2(mreal_t *result, mreal_t x, mreal_t y) {
    result[0] = x;
    result[1] = y;
    return result;
}

mreal_t *vec2_assign(mreal_t *result, const mreal_t *v0) {
    result[0] = v0[0];
    result[1] = v0[1];
    return result;
}

mreal_t *vec2_assign_vec2i(mreal_t *result, const mint_t *v0) {
    result[0] = v0[0];
    result[1] = v0[1];
    return result;
}

mreal_t *vec2_zero(mreal_t *result) {
    result[0] = MREAL_C(0.0);
    result[1] = MREAL_C(0.0);
    return result;
}

mreal_t *vec2_one(mreal_t *result) {
    result[0] = MREAL_C(1.0);
    result[1] = MREAL_C(1.0);
    return result;
}

mreal_t *vec2_sign(mreal_t *result, const mreal_t *v0) {
    if (v0[0] > MREAL_C(0.0)) {
        result[0] = MREAL_C(1.0);
    } else if (v0[0] < MREAL_C(0.0)) {
        result[0] = -MREAL_C(1.0);
    } else {
        result[0] = MREAL_C(0.0);
    }
    if (v0[1] > MREAL_C(0.0)) {
        result[1] = MREAL_C(1.0);
    } else if (v0[1] < MREAL_C(0.0)) {
        result[1] = -MREAL_C(1.0);
    } else {
        result[1] = MREAL_C(0.0);
    }
    return result;
}

mreal_t *vec2_add(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] + v1[0];
    result[1] = v0[1] + v1[1];
    return result;
}

mreal_t *vec2_add_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] + f;
    result[1] = v0[1] + f;
    return result;
}

mreal_t *vec2_subtract(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] - v1[0];
    result[1] = v0[1] - v1[1];
    return result;
}

mreal_t *vec2_subtract_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] - f;
    result[1] = v0[1] - f;
    return result;
}

mreal_t *vec2_multiply(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] * v1[0];
    result[1] = v0[1] * v1[1];
    return result;
}

mreal_t *vec2_multiply_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] * f;
    result[1] = v0[1] * f;
    return result;
}

mreal_t *vec2_multiply_mat2(mreal_t *result, const mreal_t *v0, const mreal_t *m0) {
    mreal_t x = v0[0];
    mreal_t y = v0[1];
    result[0] = m0[0] * x + m0[2] * y;
    result[1] = m0[1] * x + m0[3] * y;
    return result;
}

mreal_t *vec2_divide(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] / v1[0];
    result[1] = v0[1] / v1[1];
    return result;
}

mreal_t *vec2_divide_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] / f;
    result[1] = v0[1] / f;
    return result;
}

mreal_t *vec2_snap(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = MFLOOR(v0[0] / v1[0]) * v1[0];
    result[1] = MFLOOR(v0[1] / v1[1]) * v1[1];
    return result;
}

mreal_t *vec2_snap_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = MFLOOR(v0[0] / f) * f;
    result[1] = MFLOOR(v0[1] / f) * f;
    return result;
}

mreal_t *vec2_negative(mreal_t *result, const mreal_t *v0) {
    result[0] = -v0[0];
    result[1] = -v0[1];
    return result;
}

mreal_t *vec2_abs(mreal_t *result, const mreal_t *v0) {
    result[0] = MFABS(v0[0]);
    result[1] = MFABS(v0[1]);
    return result;
}

mreal_t *vec2_floor(mreal_t *result, const mreal_t *v0) {
    result[0] = MFLOOR(v0[0]);
    result[1] = MFLOOR(v0[1]);
    return result;
}

mreal_t *vec2_ceil(mreal_t *result, const mreal_t *v0) {
    result[0] = MCEIL(v0[0]);
    result[1] = MCEIL(v0[1]);
    return result;
}

mreal_t *vec2_round(mreal_t *result, const mreal_t *v0) {
    result[0] = MROUND(v0[0]);
    result[1] = MROUND(v0[1]);
    return result;
}

mreal_t *vec2_max(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = MFMAX(v0[0], v1[0]);
    result[1] = MFMAX(v0[1], v1[1]);
    return result;
}

mreal_t *vec2_min(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = MFMIN(v0[0], v1[0]);
    result[1] = MFMIN(v0[1], v1[1]);
    return result;
}

mreal_t *vec2_clamp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2) {
    vec2_min(result, v0, v1);
    vec2_max(result, v0, v2);
    return result;
}

mreal_t *vec2_normalize(mreal_t *result, const mreal_t *v0) {
    mreal_t l = MSQRT(v0[0] * v0[0] + v0[1] * v0[1]);
    result[0] = v0[0] / l;
    result[1] = v0[1] / l;
    return result;
}

mreal_t vec2_dot(const mreal_t *v0, const mreal_t *v1) {
    return v0[0] * v1[0] + v0[1] * v1[1];
}

mreal_t *vec2_project(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    mreal_t d = vec2_dot(v1, v1);
    mreal_t s = vec2_dot(v0, v1) / d;
    result[0] = v1[0] * s;
    result[1] = v1[1] * s;
    return result;
}

mreal_t *vec2_slide(mreal_t *result, const mreal_t *v0, const mreal_t *normal) {
    mreal_t d = vec2_dot(v0, normal);
    result[0] = v0[0] - normal[0] * d;
    result[1] = v0[1] - normal[1] * d;
    return result;
}

mreal_t *vec2_reflect(mreal_t *result, const mreal_t *v0, const mreal_t *normal) {
    mreal_t d = MREAL_C(2.0) * vec2_dot(v0, normal);
    result[0] = normal[0] * d - v0[0];
    result[1] = normal[1] * d - v0[1];
    return result;
}

mreal_t *vec2_tangent(mreal_t *result, const mreal_t *v0) {
    mreal_t a0 = v0[0];
    mreal_t a1 = v0[1];
    result[0] = a1;
    result[1] = -a0;
    return result;
}

mreal_t *vec2_rotate(mreal_t *result, const mreal_t *v0, mreal_t f) {
    mreal_t cs = MCOS(f);
    mreal_t sn = MSIN(f);
    mreal_t x = v0[0];
    mreal_t y = v0[1];
    result[0] = x * cs - y * sn;
    result[1] = x * sn + y * cs;
    return result;
}

mreal_t *vec2_lerp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, mreal_t f) {
    result[0] = v0[0] + (v1[0] - v0[0]) * f;
    result[1] = v0[1] + (v1[1] - v0[1]) * f;
    return result;
}

mreal_t *vec2_bezier3(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2, mreal_t f) {
    mreal_t tmp0[VEC2_SIZE];
    mreal_t tmp1[VEC2_SIZE];
    vec2_lerp(tmp0, v0, v1, f);
    vec2_lerp(tmp1, v1, v2, f);
    vec2_lerp(result, tmp0, tmp1, f);
    return result;
}

mreal_t *vec2_bezier4(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2, const mreal_t *v3, mreal_t f) {
    mreal_t tmp0[VEC2_SIZE];
    mreal_t tmp1[VEC2_SIZE];
    mreal_t tmp2[VEC2_SIZE];
    mreal_t tmp3[VEC2_SIZE];
    mreal_t tmp4[VEC2_SIZE];
    vec2_lerp(tmp0, v0, v1, f);
    vec2_lerp(tmp1, v1, v2, f);
    vec2_lerp(tmp2, v2, v3, f);
    vec2_lerp(tmp3, tmp0, tmp1, f);
    vec2_lerp(tmp4, tmp1, tmp2, f);
    vec2_lerp(result, tmp3, tmp4, f);
    return result;
}

mreal_t vec2_angle(const mreal_t *v0) {
    return MATAN2(v0[1], v0[0]);
}

mreal_t vec2_length(const mreal_t *v0) {
    return MSQRT(v0[0] * v0[0] + v0[1] * v0[1]);
}

mreal_t vec2_length_squared(const mreal_t *v0) {
    return v0[0] * v0[0] + v0[1] * v0[1];
}

mreal_t vec2_distance(const mreal_t *v0, const mreal_t *v1) {
    return MSQRT((v0[0] - v1[0]) * (v0[0] - v1[0]) + (v0[1] - v1[1]) * (v0[1] - v1[1]));
}

mreal_t vec2_distance_squared(const mreal_t *v0, const mreal_t *v1) {
    return (v0[0] - v1[0]) * (v0[0] - v1[0]) + (v0[1] - v1[1]) * (v0[1] - v1[1]);
}

bool vec2_linear_independent(const mreal_t *v0, const mreal_t *v1) {
    return (v0[0] * v1[1] - v1[0] * v0[1]) != 0;
}

mreal_t** vec2_orthonormalization(mreal_t result[2][2], mreal_t basis[2][2]) {
    mreal_t v0[2];
    mreal_t v1[2];

    for(int32_t i = 0; i < 2; ++i) {
        v0[i] = basis[0][i];
        v1[i] = basis[1][i];
    }

    if (!vec2_linear_independent(v0, v1)) {
        return (mreal_t**)result;
    }

    mreal_t proju1[2];
    mreal_t u0[2];
    mreal_t u1[2];

    for(int32_t i = 0; i < 2; ++i) {
        u0[i] = v0[i];
    }

    vec2_project(proju1, v1, v0);
    vec2_subtract(u1, v1, proju1);
    vec2_normalize(result[0], u0);
    vec2_normalize(result[1], u1);

    return (mreal_t**)result;
}

bool vec3_is_zero(const mreal_t *v0) {
    return MFABS(v0[0]) < MREAL_EPSILON && MFABS(v0[1]) < MREAL_EPSILON && MFABS(v0[2]) < MREAL_EPSILON;
}

bool vec3_is_equal(const mreal_t *v0, const mreal_t *v1) {
    return MFABS(v0[0] - v1[0]) < MREAL_EPSILON && MFABS(v0[1] - v1[1]) < MREAL_EPSILON && MFABS(v0[2] - v1[2]) < MREAL_EPSILON;
}

mreal_t *vec3(mreal_t *result, mreal_t x, mreal_t y, mreal_t z) {
    result[0] = x;
    result[1] = y;
    result[2] = z;
    return result;
}

mreal_t *vec3_assign(mreal_t *result, const mreal_t *v0) {
    memcpy(result,v0,sizeof(mreal_t)*VEC3_SIZE);
    return result;
}

mreal_t *vec3_assign_vec3i(mreal_t *result, const mint_t *v0) {
    result[0] = v0[0];
    result[1] = v0[1];
    result[2] = v0[2];
    return result;
}

mreal_t *vec3_zero(mreal_t *result) {
    result[0] = MREAL_C(0.0);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    return result;
}

mreal_t *vec3_one(mreal_t *result) {
    result[0] = MREAL_C(1.0);
    result[1] = MREAL_C(1.0);
    result[2] = MREAL_C(1.0);
    return result;
}

mreal_t *vec3_sign(mreal_t *result, const mreal_t *v0) {
    if (v0[0] > MREAL_C(0.0)) {
        result[0] = MREAL_C(1.0);
    } else if (v0[0] < 0) {
        result[0] = -MREAL_C(1.0);
    } else {
        result[0] = MREAL_C(0.0);
    }
    if (v0[1] > MREAL_C(0.0)) {
        result[1] = MREAL_C(1.0);
    } else if (v0[1] < 0) {
        result[1] = -MREAL_C(1.0);
    } else {
        result[1] = MREAL_C(0.0);
    }
    if (v0[2] > MREAL_C(0.0)) {
        result[2] = MREAL_C(1.0);
    } else if (v0[2] < 0) {
        result[2] = -MREAL_C(1.0);
    } else {
        result[2] = MREAL_C(0.0);
    }
    return result;
}

mreal_t *vec3_add(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] + v1[0];
    result[1] = v0[1] + v1[1];
    result[2] = v0[2] + v1[2];
    return result;
}

mreal_t *vec3_add_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] + f;
    result[1] = v0[1] + f;
    result[2] = v0[2] + f;
    return result;
}

mreal_t *vec3_subtract(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] - v1[0];
    result[1] = v0[1] - v1[1];
    result[2] = v0[2] - v1[2];
    return result;
}

mreal_t *vec3_subtract_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] - f;
    result[1] = v0[1] - f;
    result[2] = v0[2] - f;
    return result;
}

mreal_t *vec3_multiply(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] * v1[0];
    result[1] = v0[1] * v1[1];
    result[2] = v0[2] * v1[2];
    return result;
}

mreal_t *vec3_multiply_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] * f;
    result[1] = v0[1] * f;
    result[2] = v0[2] * f;
    return result;
}

mreal_t *vec3_multiply_mat3(mreal_t *result, const mreal_t *v0, const mreal_t *m0) {
    mreal_t x = v0[0];
    mreal_t y = v0[1];
    mreal_t z = v0[2];
    result[0] = m0[0] * x + m0[3] * y + m0[6] * z;
    result[1] = m0[1] * x + m0[4] * y + m0[7] * z;
    result[2] = m0[2] * x + m0[5] * y + m0[8] * z;
    return result;
}

mreal_t *vec3_divide(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] / v1[0];
    result[1] = v0[1] / v1[1];
    result[2] = v0[2] / v1[2];
    return result;
}

mreal_t *vec3_divide_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] / f;
    result[1] = v0[1] / f;
    result[2] = v0[2] / f;
    return result;
}

mreal_t *vec3_snap(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = MFLOOR(v0[0] / v1[0]) * v1[0];
    result[1] = MFLOOR(v0[1] / v1[1]) * v1[1];
    result[2] = MFLOOR(v0[2] / v1[2]) * v1[2];
    return result;
}

mreal_t *vec3_snap_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = MFLOOR(v0[0] / f) * f;
    result[1] = MFLOOR(v0[1] / f) * f;
    result[2] = MFLOOR(v0[2] / f) * f;
    return result;
}

mreal_t *vec3_negative(mreal_t *result, const mreal_t *v0) {
    result[0] = -v0[0];
    result[1] = -v0[1];
    result[2] = -v0[2];
    return result;
}

mreal_t *vec3_abs(mreal_t *result, const mreal_t *v0) {
    result[0] = MFABS(v0[0]);
    result[1] = MFABS(v0[1]);
    result[2] = MFABS(v0[2]);
    return result;
}

mreal_t *vec3_floor(mreal_t *result, const mreal_t *v0) {
    result[0] = MFLOOR(v0[0]);
    result[1] = MFLOOR(v0[1]);
    result[2] = MFLOOR(v0[2]);
    return result;
}

mreal_t *vec3_ceil(mreal_t *result, const mreal_t *v0) {
    result[0] = MCEIL(v0[0]);
    result[1] = MCEIL(v0[1]);
    result[2] = MCEIL(v0[2]);
    return result;
}

mreal_t *vec3_round(mreal_t *result, const mreal_t *v0) {
    result[0] = MROUND(v0[0]);
    result[1] = MROUND(v0[1]);
    result[2] = MROUND(v0[2]);
    return result;
}

mreal_t *vec3_max(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = MFMAX(v0[0], v1[0]);
    result[1] = MFMAX(v0[1], v1[1]);
    result[2] = MFMAX(v0[2], v1[2]);
    return result;
}

mreal_t *vec3_min(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = MFMIN(v0[0], v1[0]);
    result[1] = MFMIN(v0[1], v1[1]);
    result[2] = MFMIN(v0[2], v1[2]);
    return result;
}

mreal_t *vec3_clamp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2) {
    vec3_min(result, v0, v1);
    vec3_max(result, v0, v2);
    return result;
}

mreal_t *vec3_cross(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    mreal_t cross[VEC3_SIZE];
    cross[0] = v0[1] * v1[2] - v0[2] * v1[1];
    cross[1] = v0[2] * v1[0] - v0[0] * v1[2];
    cross[2] = v0[0] * v1[1] - v0[1] * v1[0];
    memcpy(result,cross,sizeof(mreal_t)*VEC3_SIZE);
    return result;
}

mreal_t *vec3_normalize(mreal_t *result, const mreal_t *v0) {
    mreal_t l = MSQRT(v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2]);
    result[0] = v0[0] / l;
    result[1] = v0[1] / l;
    result[2] = v0[2] / l;
    return result;
}

mreal_t vec3_dot(const mreal_t *v0, const mreal_t *v1) {
    return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

mreal_t *vec3_project(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    mreal_t d = vec3_dot(v1, v1);
    mreal_t s = vec3_dot(v0, v1) / d;
    result[0] = v1[0] * s;
    result[1] = v1[1] * s;
    result[2] = v1[2] * s;
    return result;
}

mreal_t *vec3_slide(mreal_t *result, const mreal_t *v0, const mreal_t *normal) {
    mreal_t d = vec3_dot(v0, normal);
    result[0] = v0[0] - normal[0] * d;
    result[1] = v0[1] - normal[1] * d;
    result[2] = v0[2] - normal[2] * d;
    return result;
}

mreal_t *vec3_reflect(mreal_t *result, const mreal_t *v0, const mreal_t *normal) {
    mreal_t d = MREAL_C(2.0) * vec3_dot(v0, normal);
    result[0] = normal[0] * d - v0[0];
    result[1] = normal[1] * d - v0[1];
    result[2] = normal[2] * d - v0[2];
    return result;
}

mreal_t *vec3_rotate(mreal_t *result, const mreal_t *v0, const mreal_t *ra, mreal_t f) {
    mreal_t cs;
    mreal_t sn;
    mreal_t x;
    mreal_t y;
    mreal_t z;
    mreal_t rx;
    mreal_t ry;
    mreal_t rz;
    cs = MCOS(f);
    sn = MSIN(f);
    x = v0[0];
    y = v0[1];
    z = v0[2];
    struct vec3 norm;
    vec3_normalize((mreal_t*)&norm, ra);
    rx = norm.x;
    ry = norm.y;
    rz = norm.z;
    result[0] = x * (cs + rx * rx * (1 - cs)) + y * (rx * ry * (1 - cs) - rz * sn) + z * (rx * rz * (1 - cs) + ry * sn);
    result[1] = x * (ry * rx * (1 - cs) + rz * sn) + y * (cs + ry * ry * (1 - cs)) + z * (ry * rz * (1 - cs) - rx * sn);
    result[2] = x * (rz * rx * (1 - cs) - ry * sn) + y * (rz * ry * (1 - cs) + rx * sn) + z * (cs + rz * rz * (1 - cs));
    return result;
}

mreal_t *vec3_lerp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, mreal_t f) {
    result[0] = v0[0] + (v1[0] - v0[0]) * f;
    result[1] = v0[1] + (v1[1] - v0[1]) * f;
    result[2] = v0[2] + (v1[2] - v0[2]) * f;
    return result;
}

mreal_t *vec3_bezier3(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2, mreal_t f) {
    mreal_t tmp0[VEC3_SIZE];
    mreal_t tmp1[VEC3_SIZE];
    vec3_lerp(tmp0, v0, v1, f);
    vec3_lerp(tmp1, v1, v2, f);
    vec3_lerp(result, tmp0, tmp1, f);
    return result;
}

mreal_t *vec3_bezier4(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2, const mreal_t *v3, mreal_t f) {
    mreal_t tmp0[VEC3_SIZE];
    mreal_t tmp1[VEC3_SIZE];
    mreal_t tmp2[VEC3_SIZE];
    mreal_t tmp3[VEC3_SIZE];
    mreal_t tmp4[VEC3_SIZE];
    vec3_lerp(tmp0, v0, v1, f);
    vec3_lerp(tmp1, v1, v2, f);
    vec3_lerp(tmp2, v2, v3, f);
    vec3_lerp(tmp3, tmp0, tmp1, f);
    vec3_lerp(tmp4, tmp1, tmp2, f);
    vec3_lerp(result, tmp3, tmp4, f);
    return result;
}

mreal_t vec3_length(const mreal_t *v0) {
    return MSQRT(v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2]);
}

mreal_t vec3_length_squared(const mreal_t *v0) {
    return v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2];
}

mreal_t vec3_distance(const mreal_t *v0, const mreal_t *v1) {
    return MSQRT((v0[0] - v1[0]) * (v0[0] - v1[0]) + (v0[1] - v1[1]) * (v0[1] - v1[1]) + (v0[2] - v1[2]) * (v0[2] - v1[2]));
}

mreal_t vec3_distance_squared(const mreal_t *v0, const mreal_t *v1) {
    return (v0[0] - v1[0]) * (v0[0] - v1[0]) + (v0[1] - v1[1]) * (v0[1] - v1[1]) + (v0[2] - v1[2]) * (v0[2] - v1[2]);
}

bool vec3_linear_independent(const mreal_t *v0, const mreal_t *v1, const mreal_t *v2) {
    return v0[0] * v1[1] * v2[2] + v0[1] * v1[2] * v2[0] + v0[2] * v1[0] * v2[1]
        - v0[2] * v1[1] * v2[0] - v0[1] * v1[0] * v2[2] - v0[0] * v1[2] * v2[1];
}

mreal_t** vec3_orthonormalization(mreal_t result[3][3], mreal_t basis[3][3]) {
    mreal_t v0[3];
    mreal_t v1[3];
    mreal_t v2[3];

    for(int32_t i = 0; i < 3; ++i) {
        v0[i] = basis[0][i];
        v1[i] = basis[1][i];
        v2[i] = basis[2][i];
    }

    if (!vec3_linear_independent(v0, v1, v2)) {
        return (mreal_t**)result;
    }

    mreal_t proj[3];
    mreal_t u0[3];
    mreal_t u1[3];
    mreal_t u2[3];

    for(int32_t i = 0; i < 3; ++i) {
        u0[i] = v0[i];
    }

    vec3_project(proj, v1, u0);
    vec3_subtract(u1, v1, proj);

    vec3_project(proj, v2, u0);
    vec3_subtract(u2, v2, proj);
    vec3_project(proj, v2, u1);
    vec3_subtract(u2, u2, proj);

    vec3_normalize(result[0], u0);
    vec3_normalize(result[1], u1);
    vec3_normalize(result[2], u2);

    return (mreal_t**)result;
}

bool vec4_is_zero(const mreal_t *v0) {
    return MFABS(v0[0]) < MREAL_EPSILON && MFABS(v0[1]) < MREAL_EPSILON && MFABS(v0[2]) < MREAL_EPSILON && MFABS(v0[3]) < MREAL_EPSILON;
}

bool vec4_is_equal(const mreal_t *v0, const mreal_t *v1) {
    return MFABS(v0[0] - v1[0]) < MREAL_EPSILON && MFABS(v0[1] - v1[1]) < MREAL_EPSILON && MFABS(v0[2] - v1[2]) < MREAL_EPSILON && MFABS(v0[3] - v1[3]) < MREAL_EPSILON;
}

mreal_t *vec4(mreal_t *result, mreal_t x, mreal_t y, mreal_t z, mreal_t w) {
    result[0] = x;
    result[1] = y;
    result[2] = z;
    result[3] = w;
    return result;
}

mreal_t *vec4_assign(mreal_t *result, const mreal_t *v0) {
    result[0] = v0[0];
    result[1] = v0[1];
    result[2] = v0[2];
    result[3] = v0[3];
    return result;
}

mreal_t *vec4_assign_vec4i(mreal_t *result, const mint_t *v0) {
    result[0] = v0[0];
    result[1] = v0[1];
    result[2] = v0[2];
    result[3] = v0[3];
    return result;
}

mreal_t *vec4_zero(mreal_t *result) {
    result[0] = MREAL_C(0.0);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    return result;
}

mreal_t *vec4_one(mreal_t *result) {
    result[0] = MREAL_C(1.0);
    result[1] = MREAL_C(1.0);
    result[2] = MREAL_C(1.0);
    result[3] = MREAL_C(1.0);
    return result;
}

mreal_t *vec4_sign(mreal_t *result, const mreal_t *v0) {
    if (v0[0] > MREAL_C(0.0)) {
        result[0] = MREAL_C(1.0);
    } else if (v0[0] < 0) {
        result[0] = -MREAL_C(1.0);
    } else {
        result[0] = MREAL_C(0.0);
    }
    if (v0[1] > MREAL_C(0.0)) {
        result[1] = MREAL_C(1.0);
    } else if (v0[1] < 0) {
        result[1] = -MREAL_C(1.0);
    } else {
        result[1] = MREAL_C(0.0);
    }
    if (v0[2] > MREAL_C(0.0)) {
        result[2] = MREAL_C(1.0);
    } else if (v0[2] < 0) {
        result[2] = -MREAL_C(1.0);
    } else {
        result[2] = MREAL_C(0.0);
    }
    if (v0[3] > MREAL_C(0.0)) {
        result[3] = MREAL_C(1.0);
    } else if (v0[3] < 0) {
        result[3] = -MREAL_C(1.0);
    } else {
        result[3] = MREAL_C(0.0);
    }
    return result;
}

mreal_t *vec4_add(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] + v1[0];
    result[1] = v0[1] + v1[1];
    result[2] = v0[2] + v1[2];
    result[3] = v0[3] + v1[3];
    return result;
}

mreal_t *vec4_add_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] + f;
    result[1] = v0[1] + f;
    result[2] = v0[2] + f;
    result[3] = v0[3] + f;
    return result;
}

mreal_t *vec4_subtract(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] - v1[0];
    result[1] = v0[1] - v1[1];
    result[2] = v0[2] - v1[2];
    result[3] = v0[3] - v1[3];
    return result;
}

mreal_t *vec4_subtract_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] - f;
    result[1] = v0[1] - f;
    result[2] = v0[2] - f;
    result[3] = v0[3] - f;
    return result;
}

mreal_t *vec4_multiply(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] * v1[0];
    result[1] = v0[1] * v1[1];
    result[2] = v0[2] * v1[2];
    result[3] = v0[3] * v1[3];
    return result;
}

mreal_t *vec4_multiply_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] * f;
    result[1] = v0[1] * f;
    result[2] = v0[2] * f;
    result[3] = v0[3] * f;
    return result;
}

mreal_t *vec4_multiply_mat4(mreal_t *result, const mreal_t *v0, const mreal_t *m0) {
    mreal_t x = v0[0];
    mreal_t y = v0[1];
    mreal_t z = v0[2];
    mreal_t w = v0[3];
    result[0] = m0[0] * x + m0[4] * y + m0[8] * z + m0[12] * w;
    result[1] = m0[1] * x + m0[5] * y + m0[9] * z + m0[13] * w;
    result[2] = m0[2] * x + m0[6] * y + m0[10] * z + m0[14] * w;
    result[3] = m0[3] * x + m0[7] * y + m0[11] * z + m0[15] * w;
    return result;
}

mreal_t *vec4_divide(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = v0[0] / v1[0];
    result[1] = v0[1] / v1[1];
    result[2] = v0[2] / v1[2];
    result[3] = v0[3] / v1[3];
    return result;
}

mreal_t *vec4_divide_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = v0[0] / f;
    result[1] = v0[1] / f;
    result[2] = v0[2] / f;
    result[3] = v0[3] / f;
    return result;
}

mreal_t *vec4_snap(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = MFLOOR(v0[0] / v1[0]) * v1[0];
    result[1] = MFLOOR(v0[1] / v1[1]) * v1[1];
    result[2] = MFLOOR(v0[2] / v1[2]) * v1[2];
    result[3] = MFLOOR(v0[3] / v1[3]) * v1[3];
    return result;
}

mreal_t *vec4_snap_f(mreal_t *result, const mreal_t *v0, mreal_t f) {
    result[0] = MFLOOR(v0[0] / f) * f;
    result[1] = MFLOOR(v0[1] / f) * f;
    result[2] = MFLOOR(v0[2] / f) * f;
    result[3] = MFLOOR(v0[3] / f) * f;
    return result;
}

mreal_t *vec4_negative(mreal_t *result, const mreal_t *v0) {
    result[0] = -v0[0];
    result[1] = -v0[1];
    result[2] = -v0[2];
    result[3] = -v0[3];
    return result;
}

mreal_t *vec4_abs(mreal_t *result, const mreal_t *v0) {
    result[0] = MFABS(v0[0]);
    result[1] = MFABS(v0[1]);
    result[2] = MFABS(v0[2]);
    result[3] = MFABS(v0[3]);
    return result;
}

mreal_t *vec4_floor(mreal_t *result, const mreal_t *v0) {
    result[0] = MFLOOR(v0[0]);
    result[1] = MFLOOR(v0[1]);
    result[2] = MFLOOR(v0[2]);
    result[3] = MFLOOR(v0[3]);
    return result;
}

mreal_t *vec4_ceil(mreal_t *result, const mreal_t *v0) {
    result[0] = MCEIL(v0[0]);
    result[1] = MCEIL(v0[1]);
    result[2] = MCEIL(v0[2]);
    result[3] = MCEIL(v0[3]);
    return result;
}

mreal_t *vec4_round(mreal_t *result, const mreal_t *v0) {
    result[0] = MROUND(v0[0]);
    result[1] = MROUND(v0[1]);
    result[2] = MROUND(v0[2]);
    result[3] = MROUND(v0[3]);
    return result;
}

mreal_t *vec4_max(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = MFMAX(v0[0], v1[0]);
    result[1] = MFMAX(v0[1], v1[1]);
    result[2] = MFMAX(v0[2], v1[2]);
    result[3] = MFMAX(v0[3], v1[3]);
    return result;
}

mreal_t *vec4_min(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    result[0] = MFMIN(v0[0], v1[0]);
    result[1] = MFMIN(v0[1], v1[1]);
    result[2] = MFMIN(v0[2], v1[2]);
    result[3] = MFMIN(v0[3], v1[3]);
    return result;
}

mreal_t *vec4_clamp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, const mreal_t *v2) {
    vec4_min(result, v0, v1);
    vec4_max(result, v0, v2);
    return result;
}

mreal_t *vec4_normalize(mreal_t *result, const mreal_t *v0) {
    mreal_t l = MSQRT(v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2] + v0[3] * v0[3]);
    result[0] = v0[0] / l;
    result[1] = v0[1] / l;
    result[2] = v0[2] / l;
    result[3] = v0[3] / l;
    return result;
}

mreal_t *vec4_lerp(mreal_t *result, const mreal_t *v0, const mreal_t *v1, mreal_t f) {
    result[0] = v0[0] + (v1[0] - v0[0]) * f;
    result[1] = v0[1] + (v1[1] - v0[1]) * f;
    result[2] = v0[2] + (v1[2] - v0[2]) * f;
    result[3] = v0[3] + (v1[3] - v0[3]) * f;
    return result;
}

bool quat_is_zero(const mreal_t *q0) {
    return MFABS(q0[0]) < MREAL_EPSILON && MFABS(q0[1]) < MREAL_EPSILON && MFABS(q0[2]) < MREAL_EPSILON && MFABS(q0[3]) < MREAL_EPSILON;
}

bool quat_is_equal(const mreal_t *q0, const mreal_t *q1) {
    return MFABS(q0[0] - q1[0]) < MREAL_EPSILON && MFABS(q0[1] - q1[1]) < MREAL_EPSILON && MFABS(q0[2] - q1[2]) < MREAL_EPSILON && MFABS(q0[3] - q1[3]) < MREAL_EPSILON;
}

mreal_t *quat(mreal_t *result, mreal_t x, mreal_t y, mreal_t z, mreal_t w) {
    result[0] = x;
    result[1] = y;
    result[2] = z;
    result[3] = w;
    return result;
}

mreal_t *quat_assign(mreal_t *result, const mreal_t *q0) {
    memcpy(result,q0,sizeof(mreal_t)*QUAT_SIZE);
    return result;
}

mreal_t *quat_zero(mreal_t *result) {
    result[0] = MREAL_C(0.0);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    return result;
}

mreal_t *quat_null(mreal_t *result) {
    result[0] = MREAL_C(0.0);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(1.0);
    return result;
}

mreal_t *quat_multiply(mreal_t *result, const mreal_t *q0, const mreal_t *q1) {
    result[0] = q0[3] * q1[0] + q0[0] * q1[3] + q0[1] * q1[2] - q0[2] * q1[1];
    result[1] = q0[3] * q1[1] + q0[1] * q1[3] + q0[2] * q1[0] - q0[0] * q1[2];
    result[2] = q0[3] * q1[2] + q0[2] * q1[3] + q0[0] * q1[1] - q0[1] * q1[0];
    result[3] = q0[3] * q1[3] - q0[0] * q1[0] - q0[1] * q1[1] - q0[2] * q1[2];
    return result;
}

mreal_t *quat_multiply_f(mreal_t *result, const mreal_t *q0, mreal_t f) {
    result[0] = q0[0] * f;
    result[1] = q0[1] * f;
    result[2] = q0[2] * f;
    result[3] = q0[3] * f;
    return result;
}

mreal_t *quat_divide(mreal_t *result, const mreal_t *q0, const mreal_t *q1) {
    mreal_t x = q0[0];
    mreal_t y = q0[1];
    mreal_t z = q0[2];
    mreal_t w = q0[3];
    mreal_t ls = q1[0] * q1[0] + q1[1] * q1[1] + q1[8] * q1[8] + q1[3] * q1[3];
    mreal_t normalized_x = -q1[0] / ls;
    mreal_t normalized_y = -q1[1] / ls;
    mreal_t normalized_z = -q1[8] / ls;
    mreal_t normalized_w = q1[3] / ls;
    result[0] = x * normalized_w + normalized_x * w + (y * normalized_z - z * normalized_y);
    result[1] = y * normalized_w + normalized_y * w + (z * normalized_x - x * normalized_z);
    result[2] = z * normalized_w + normalized_z * w + (x * normalized_y - y * normalized_x);
    result[3] = w * normalized_w - (x * normalized_x + y * normalized_y + z * normalized_z);
    return result;
}

mreal_t *quat_divide_f(mreal_t *result, const mreal_t *q0, mreal_t f) {
    result[0] = q0[0] / f;
    result[1] = q0[1] / f;
    result[2] = q0[2] / f;
    result[3] = q0[3] / f;
    return result;
}

mreal_t *quat_negative(mreal_t *result, const mreal_t *q0) {
    result[0] = -q0[0];
    result[1] = -q0[1];
    result[2] = -q0[2];
    result[3] = -q0[3];
    return result;
}

mreal_t *quat_conjugate(mreal_t *result, const mreal_t *q0) {
    result[0] = -q0[0];
    result[1] = -q0[1];
    result[2] = -q0[2];
    result[3] = q0[3];
    return result;
}

mreal_t *quat_inverse(mreal_t *result, const mreal_t *q0) {
    mreal_t l = MREAL_C(1.0) / (q0[0] * q0[0] + q0[1] * q0[1] + q0[2] * q0[2] + q0[3] * q0[3]);
    result[0] = -q0[0] * l;
    result[1] = -q0[1] * l;
    result[2] = -q0[2] * l;
    result[3] = q0[3] * l;
    return result;
}

mreal_t *quat_normalize(mreal_t *result, const mreal_t *q0) {
    mreal_t l = MREAL_C(1.0) / MSQRT(q0[0] * q0[0] + q0[1] * q0[1] + q0[2] * q0[2] + q0[3] * q0[3]);
    result[0] = q0[0] * l;
    result[1] = q0[1] * l;
    result[2] = q0[2] * l;
    result[3] = q0[3] * l;
    return result;
}

mreal_t quat_dot(const mreal_t *q0, const mreal_t *q1) {
    return q0[0] * q1[0] + q0[1] * q1[1] + q0[2] * q1[2] + q0[3] * q1[3];
}

mreal_t *quat_power(mreal_t *result, const mreal_t *q0, mreal_t exponent) {
    if (MFABS(q0[3]) < MREAL_C(1.0) - MREAL_EPSILON) {
        mreal_t alpha = MACOS(q0[3]);
        mreal_t new_alpha = alpha * exponent;
        mreal_t s = MSIN(new_alpha) / MSIN(alpha);
        result[0] = result[0] * s;
        result[1] = result[1] * s;
        result[2] = result[2] * s;
        result[3] = MCOS(new_alpha);
    } else {
        memcpy(result,q0,sizeof(mreal_t)*QUAT_SIZE);
    }
    return result;
}

mreal_t *quat_from_axis_angle(mreal_t *result, const mreal_t *v0, mreal_t angle) {
    mreal_t half = angle * MREAL_C(0.5);
    mreal_t s = MSIN(half);
    result[0] = v0[0] * s;
    result[1] = v0[1] * s;
    result[2] = v0[2] * s;
    result[3] = MCOS(half);
    return result;
}

mreal_t *quat_from_vec3(mreal_t *result, const mreal_t *v0, const mreal_t *v1) {
    mreal_t cross[VEC3_SIZE];
    mreal_t d = vec3_dot(v0, v1);
    mreal_t a_ls = vec3_length_squared(v0);
    mreal_t b_ls = vec3_length_squared(v0);
    vec3_cross(cross, v0, v1);
    quat(result, cross[0], cross[1], cross[1], d + MSQRT(a_ls * b_ls));
    quat_normalize(result, result);
    return result;
}

mreal_t *quat_from_mat4(mreal_t *result, const mreal_t *m0) {
    mreal_t scale = m0[0] + m0[5] + m0[10];
    if (scale > MREAL_C(0.0)) {
        mreal_t sr = MSQRT(scale + MREAL_C(1.0));
        result[3] = sr * MREAL_C(0.5);
        sr = MREAL_C(0.5) / sr;
        result[0] = (m0[9] - m0[6]) * sr;
        result[1] = (m0[2] - m0[8]) * sr;
        result[2] = (m0[4] - m0[1]) * sr;
    } else if ((m0[0] >= m0[5]) && (m0[0] >= m0[10])) {
        mreal_t sr = MSQRT(MREAL_C(1.0) + m0[0] - m0[5] - m0[10]);
        mreal_t half = MREAL_C(0.5) / sr;
        result[0] = MREAL_C(0.5) * sr;
        result[1] = (m0[4] + m0[1]) * half;
        result[2] = (m0[8] + m0[2]) * half;
        result[3] = (m0[9] - m0[6]) * half;
    } else if (m0[5] > m0[10]) {
        mreal_t sr = MSQRT(MREAL_C(1.0) + m0[5] - m0[0] - m0[10]);
        mreal_t half = MREAL_C(0.5) / sr;
        result[0] = (m0[1] + m0[4]) * half;
        result[1] = MREAL_C(0.5) * sr;
        result[2] = (m0[6] + m0[9]) * half;
        result[3] = (m0[2] - m0[8]) * half;
    } else {
        mreal_t sr = MSQRT(MREAL_C(1.0) + m0[10] - m0[0] - m0[5]);
        mreal_t half = MREAL_C(0.5) / sr;
        result[0] = (m0[2] + m0[8]) * half;
        result[1] = (m0[6] + m0[9]) * half;
        result[2] = MREAL_C(0.5) * sr;
        result[3] = (m0[4] - m0[1]) * half;
    }
    return result;
}

mreal_t *quat_lerp(mreal_t *result, const mreal_t *q0, const mreal_t *q1, mreal_t f) {
    result[0] = q0[0] + (q1[0] - q0[0]) * f;
    result[1] = q0[1] + (q1[1] - q0[1]) * f;
    result[2] = q0[2] + (q1[2] - q0[2]) * f;
    result[3] = q0[3] + (q1[3] - q0[3]) * f;
    return result;
}

mreal_t *quat_slerp(mreal_t *result, const mreal_t *q0, const mreal_t *q1, mreal_t f) {
    mreal_t tmp1[QUAT_SIZE];
    mreal_t d = quat_dot(q0, q1);
    mreal_t f0;
    mreal_t f1;
    quat_assign(tmp1, q1);
    if (d < MREAL_C(0.0)) {
        quat_negative(tmp1, tmp1);
        d = -d;
    }
    if (d > MREAL_C(0.9995)) {
        f0 = MREAL_C(1.0) - f;
        f1 = f;
    } else {
        mreal_t theta = MACOS(d);
        mreal_t sin_theta = MSIN(theta);
        f0 = MSIN((MREAL_C(1.0) - f) * theta) / sin_theta;
        f1 = MSIN(f * theta) / sin_theta;
    }
    result[0] = q0[0] * f0 + tmp1[0] * f1;
    result[1] = q0[1] * f0 + tmp1[1] * f1;
    result[2] = q0[2] * f0 + tmp1[2] * f1;
    result[3] = q0[3] * f0 + tmp1[3] * f1;
    return result;
}

mreal_t quat_length(const mreal_t *q0) {
    return MSQRT(q0[0] * q0[0] + q0[1] * q0[1] + q0[2] * q0[2] + q0[3] * q0[3]);
}

mreal_t quat_length_squared(const mreal_t *q0) {
    return q0[0] * q0[0] + q0[1] * q0[1] + q0[2] * q0[2] + q0[3] * q0[3];
}

mreal_t quat_angle(const mreal_t *q0, const mreal_t *q1) {
    mreal_t s = MSQRT(quat_length_squared(q0) * quat_length_squared(q1));
    s = MREAL_C(1.0) / s;
    return MACOS(quat_dot(q0, q1) * s);
}

mreal_t *mat2(mreal_t *result, mreal_t m11, mreal_t m12, mreal_t m21, mreal_t m22) {
    result[0] = m11;
    result[1] = m21;
    result[2] = m12;
    result[3] = m22;
    return result;
}

mreal_t *mat2_zero(mreal_t *result) {
    result[0] = MREAL_C(0.0);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    return result;
}

mreal_t *mat2_identity(mreal_t *result) {
    result[0] = MREAL_C(1.0);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(1.0);
    return result;
}

mreal_t mat2_determinant(const mreal_t *m0) {
    return m0[0] * m0[3] - m0[2] * m0[1];
}

mreal_t *mat2_assign(mreal_t *result, const mreal_t *m0) {
    result[0] = m0[0];
    result[1] = m0[1];
    result[2] = m0[2];
    result[3] = m0[3];
    return result;
}

mreal_t *mat2_negative(mreal_t *result, const mreal_t *m0) {
    result[0] = -m0[0];
    result[1] = -m0[1];
    result[2] = -m0[2];
    result[3] = -m0[3];
    return result;
}

mreal_t *mat2_transpose(mreal_t *result, const mreal_t *m0) {
    mreal_t transposed[MAT2_SIZE];
    transposed[0] = m0[0];
    transposed[1] = m0[2];
    transposed[2] = m0[1];
    transposed[3] = m0[3];
    result[0] = transposed[0];
    result[1] = transposed[1];
    result[2] = transposed[2];
    result[3] = transposed[3];
    return result;
}

mreal_t *mat2_cofactor(mreal_t *result, const mreal_t *m0) {
    mreal_t cofactor[MAT2_SIZE];
    cofactor[0] = m0[3];
    cofactor[1] = -m0[2];
    cofactor[2] = -m0[1];
    cofactor[3] = m0[0];
    result[0] = cofactor[0];
    result[1] = cofactor[1];
    result[2] = cofactor[2];
    result[3] = cofactor[3];
    return result;
}

mreal_t *mat2_adjugate(mreal_t *result, const mreal_t *m0) {
    mreal_t adjugate[MAT2_SIZE];
    adjugate[0] = m0[3];
    adjugate[1] = -m0[1];
    adjugate[2] = -m0[2];
    adjugate[3] = m0[0];
    result[0] = adjugate[0];
    result[1] = adjugate[1];
    result[2] = adjugate[2];
    result[3] = adjugate[3];
    return result;
}

mreal_t *mat2_multiply(mreal_t *result, const mreal_t *m0, const mreal_t *m1) {
    mreal_t multiplied[MAT3_SIZE];
    multiplied[0] = m0[0] * m1[0] + m0[2] * m1[1];
    multiplied[1] = m0[1] * m1[0] + m0[3] * m1[1];
    multiplied[2] = m0[0] * m1[2] + m0[2] * m1[3];
    multiplied[3] = m0[1] * m1[2] + m0[3] * m1[3];
    result[0] = multiplied[0];
    result[1] = multiplied[1];
    result[2] = multiplied[2];
    result[3] = multiplied[3];
    return result;
}

mreal_t *mat2_multiply_f(mreal_t *result, const mreal_t *m0, mreal_t f) {
    result[0] = m0[0] * f;
    result[1] = m0[1] * f;
    result[2] = m0[2] * f;
    result[3] = m0[3] * f;
    return result;
}

mreal_t *mat2_inverse(mreal_t *result, const mreal_t *m0) {
    mreal_t inverse[MAT2_SIZE];
    mreal_t det = mat2_determinant(m0);
    mat2_cofactor(inverse, m0);
    mat2_multiply_f(inverse, inverse, MREAL_C(1.0) / det);
    result[0] = inverse[0];
    result[1] = inverse[1];
    result[2] = inverse[2];
    result[3] = inverse[3];
    return result;
}

mreal_t *mat2_scaling(mreal_t *result, const mreal_t *v0) {
    result[0] = v0[0];
    result[3] = v0[1];
    return result;
}

mreal_t *mat2_scale(mreal_t *result, const mreal_t *m0, const mreal_t *v0) {
    result[0] = m0[0] * v0[0];
    result[3] = m0[3] * v0[1];
    return result;
}

mreal_t *mat2_rotation_z(mreal_t *result, mreal_t f) {
    mreal_t c = MCOS(f);
    mreal_t s = MSIN(f);
    result[0] = c;
    result[1] = s;
    result[2] = -s;
    result[3] = c;
    return result;
}

mreal_t *mat2_lerp(mreal_t *result, const mreal_t *m0, const mreal_t *m1, mreal_t f) {
    result[0] = m0[0] + (m1[0] - m0[0]) * f;
    result[1] = m0[1] + (m1[1] - m0[1]) * f;
    result[2] = m0[2] + (m1[2] - m0[2]) * f;
    result[3] = m0[3] + (m1[3] - m0[3]) * f;
    return result;
}

mreal_t *mat3(mreal_t *result, mreal_t m11, mreal_t m12, mreal_t m13, mreal_t m21, mreal_t m22, mreal_t m23, mreal_t m31, mreal_t m32, mreal_t m33) {
    result[0] = m11;
    result[1] = m21;
    result[2] = m31;
    result[3] = m12;
    result[4] = m22;
    result[5] = m32;
    result[6] = m13;
    result[7] = m23;
    result[8] = m33;
    return result;
}

mreal_t *mat3_zero(mreal_t *result) {
    result[0] = MREAL_C(0.0);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    result[4] = MREAL_C(0.0);
    result[5] = MREAL_C(0.0);
    result[6] = MREAL_C(0.0);
    result[7] = MREAL_C(0.0);
    result[8] = MREAL_C(0.0);
    return result;
}

mreal_t *mat3_identity(mreal_t *result) {
    result[0] = MREAL_C(1.0);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    result[4] = MREAL_C(1.0);
    result[5] = MREAL_C(0.0);
    result[6] = MREAL_C(0.0);
    result[7] = MREAL_C(0.0);
    result[8] = MREAL_C(1.0);
    return result;
}

mreal_t mat3_determinant(const mreal_t *m0) {
    mreal_t m11 = m0[0];
    mreal_t m21 = m0[1];
    mreal_t m31 = m0[2];
    mreal_t m12 = m0[3];
    mreal_t m22 = m0[4];
    mreal_t m32 = m0[5];
    mreal_t m13 = m0[6];
    mreal_t m23 = m0[7];
    mreal_t m33 = m0[8];
    mreal_t determinant = m11 * m22 * m33
        + m12 * m23 * m31
        + m13 * m21 * m32
        - m11 * m23 * m32
        - m12 * m21 * m33
        - m13 * m22 * m31;
    return determinant;
}

mreal_t *mat3_assign(mreal_t *result, const mreal_t *m0) {
    result[0] = m0[0];
    result[1] = m0[1];
    result[2] = m0[2];
    result[3] = m0[3];
    result[4] = m0[4];
    result[5] = m0[5];
    result[6] = m0[6];
    result[7] = m0[7];
    result[8] = m0[8];
    return result;
}

mreal_t *mat3_negative(mreal_t *result, const mreal_t *m0) {
    result[0] = -m0[0];
    result[1] = -m0[1];
    result[2] = -m0[2];
    result[3] = -m0[3];
    result[4] = -m0[4];
    result[5] = -m0[5];
    result[6] = -m0[6];
    result[7] = -m0[7];
    result[8] = -m0[8];
    return result;
}

mreal_t *mat3_transpose(mreal_t *result, const mreal_t *m0) {
    mreal_t transposed[MAT4_SIZE];
    transposed[0] = m0[0];
    transposed[1] = m0[3];
    transposed[2] = m0[6];
    transposed[3] = m0[1];
    transposed[4] = m0[4];
    transposed[5] = m0[7];
    transposed[6] = m0[2];
    transposed[7] = m0[5];
    transposed[8] = m0[8];
    result[0] = transposed[0];
    result[1] = transposed[1];
    result[2] = transposed[2];
    result[3] = transposed[3];
    result[4] = transposed[4];
    result[5] = transposed[5];
    result[6] = transposed[6];
    result[7] = transposed[7];
    result[8] = transposed[8];
    return result;
}

mreal_t *mat3_cofactor(mreal_t *result, const mreal_t *m0) {
    mreal_t cofactor[MAT3_SIZE];
    mreal_t minor[MAT2_SIZE];
    minor[0] = m0[4];
    minor[1] = m0[5];
    minor[2] = m0[7];
    minor[3] = m0[8];
    cofactor[0] = mat2_determinant(minor);
    minor[0] = m0[3];
    minor[1] = m0[5];
    minor[2] = m0[6];
    minor[3] = m0[8];
    cofactor[1] = -mat2_determinant(minor);
    minor[0] = m0[3];
    minor[1] = m0[4];
    minor[2] = m0[6];
    minor[3] = m0[7];
    cofactor[2] = mat2_determinant(minor);
    minor[0] = m0[1];
    minor[1] = m0[2];
    minor[2] = m0[7];
    minor[3] = m0[8];
    cofactor[3] = -mat2_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[2];
    minor[2] = m0[6];
    minor[3] = m0[8];
    cofactor[4] = mat2_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[1];
    minor[2] = m0[6];
    minor[3] = m0[7];
    cofactor[5] = -mat2_determinant(minor);
    minor[0] = m0[1];
    minor[1] = m0[2];
    minor[2] = m0[4];
    minor[3] = m0[5];
    cofactor[6] = mat2_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[2];
    minor[2] = m0[3];
    minor[3] = m0[5];
    cofactor[7] = -mat2_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[1];
    minor[2] = m0[3];
    minor[3] = m0[4];
    cofactor[8] = mat2_determinant(minor);
    result[0] = cofactor[0];
    result[1] = cofactor[1];
    result[2] = cofactor[2];
    result[3] = cofactor[3];
    result[4] = cofactor[4];
    result[5] = cofactor[5];
    result[6] = cofactor[6];
    result[7] = cofactor[7];
    result[8] = cofactor[8];
    return result;
}

mreal_t *mat3_multiply(mreal_t *result, const mreal_t *m0, const mreal_t *m1) {
    mreal_t multiplied[MAT3_SIZE];
    multiplied[0] = m0[0] * m1[0] + m0[3] * m1[1] + m0[6] * m1[2];
    multiplied[1] = m0[1] * m1[0] + m0[4] * m1[1] + m0[7] * m1[2];
    multiplied[2] = m0[2] * m1[0] + m0[5] * m1[1] + m0[8] * m1[2];
    multiplied[3] = m0[0] * m1[3] + m0[3] * m1[4] + m0[6] * m1[5];
    multiplied[4] = m0[1] * m1[3] + m0[4] * m1[4] + m0[7] * m1[5];
    multiplied[5] = m0[2] * m1[3] + m0[5] * m1[4] + m0[8] * m1[5];
    multiplied[6] = m0[0] * m1[6] + m0[3] * m1[7] + m0[6] * m1[8];
    multiplied[7] = m0[1] * m1[6] + m0[4] * m1[7] + m0[7] * m1[8];
    multiplied[8] = m0[2] * m1[6] + m0[5] * m1[7] + m0[8] * m1[8];
    result[0] = multiplied[0];
    result[1] = multiplied[1];
    result[2] = multiplied[2];
    result[3] = multiplied[3];
    result[4] = multiplied[4];
    result[5] = multiplied[5];
    result[6] = multiplied[6];
    result[7] = multiplied[7];
    result[8] = multiplied[8];
    return result;
}

mreal_t *mat3_multiply_f(mreal_t *result, const mreal_t *m0, mreal_t f) {
    result[0] = m0[0] * f;
    result[1] = m0[1] * f;
    result[2] = m0[2] * f;
    result[3] = m0[3] * f;
    result[4] = m0[4] * f;
    result[5] = m0[5] * f;
    result[6] = m0[6] * f;
    result[7] = m0[7] * f;
    result[8] = m0[8] * f;
    return result;
}

mreal_t *mat3_inverse(mreal_t *result, const mreal_t *m0) {
    mreal_t inverse[MAT3_SIZE];
    mreal_t inverted_determinant;
    mreal_t m11 = m0[0];
    mreal_t m21 = m0[1];
    mreal_t m31 = m0[2];
    mreal_t m12 = m0[3];
    mreal_t m22 = m0[4];
    mreal_t m32 = m0[5];
    mreal_t m13 = m0[6];
    mreal_t m23 = m0[7];
    mreal_t m33 = m0[8];
    inverse[0] = m22 * m33 - m32 * m23;
    inverse[3] = m13 * m32 - m12 * m33;
    inverse[6] = m12 * m23 - m13 * m22;
    inverse[1] = m23 * m31 - m21 * m33;
    inverse[4] = m11 * m33 - m13 * m31;
    inverse[7] = m21 * m13 - m11 * m23;
    inverse[2] = m21 * m32 - m31 * m22;
    inverse[5] = m31 * m12 - m11 * m32;
    inverse[8] = m11 * m22 - m21 * m12;
    inverted_determinant = MREAL_C(1.0) / (m11 * inverse[0] + m21 * inverse[3] + m31 * inverse[6]);
    result[0] = inverse[0] * inverted_determinant;
    result[1] = inverse[1] * inverted_determinant;
    result[2] = inverse[2] * inverted_determinant;
    result[3] = inverse[3] * inverted_determinant;
    result[4] = inverse[4] * inverted_determinant;
    result[5] = inverse[5] * inverted_determinant;
    result[6] = inverse[6] * inverted_determinant;
    result[7] = inverse[7] * inverted_determinant;
    result[8] = inverse[8] * inverted_determinant;
    return result;
}

mreal_t *mat3_scaling(mreal_t *result, const mreal_t *v0) {
    result[0] = v0[0];
    result[4] = v0[1];
    result[8] = v0[2];
    return result;
}

mreal_t *mat3_scale(mreal_t *result, const mreal_t *m0, const mreal_t *v0) {
    result[0] = m0[0] * v0[0];
    result[4] = m0[4] * v0[1];
    result[8] = m0[8] * v0[2];
    return result;
}

mreal_t *mat3_rotation_x(mreal_t *result, mreal_t f) {
    mreal_t c = MCOS(f);
    mreal_t s = MSIN(f);
    result[4] = c;
    result[5] = s;
    result[7] = -s;
    result[8] = c;
    return result;
}

mreal_t *mat3_rotation_y(mreal_t *result, mreal_t f) {
    mreal_t c = MCOS(f);
    mreal_t s = MSIN(f);
    result[0] = c;
    result[2] = -s;
    result[6] = s;
    result[8] = c;
    return result;
}

mreal_t *mat3_rotation_z(mreal_t *result, mreal_t f) {
    mreal_t c = MCOS(f);
    mreal_t s = MSIN(f);
    result[0] = c;
    result[1] = s;
    result[3] = -s;
    result[4] = c;
    return result;
}

mreal_t *mat3_rotation_axis(mreal_t *result, const mreal_t *v0, mreal_t f) {
#if defined(CGEOM_PRECISE_TRIGONOMETRY)
    sincos1cos sc1c = sncs1cs(f);
    mreal_t c = sc1c.cos, s = sc1c.sin, one_c = sc1c.omc;
#else
    mreal_t c = MCOS(f);
    mreal_t s = MSIN(f);
    mreal_t one_c = MREAL_C(1.0) - c;
#endif
    mreal_t x = v0[0];
    mreal_t y = v0[1];
    mreal_t z = v0[2];
    mreal_t xx = x * x;
    mreal_t xy = x * y;
    mreal_t xz = x * z;
    mreal_t yy = y * y;
    mreal_t yz = y * z;
    mreal_t zz = z * z;
    mreal_t l = xx + yy + zz;
    mreal_t sqrt_l = MSQRT(l);
    result[0] = (xx + (yy + zz) * c) / l;
    result[1] = (xy * one_c + v0[2] * sqrt_l * s) / l;
    result[2] = (xz * one_c - v0[1] * sqrt_l * s) / l;
    result[3] = (xy * one_c - v0[2] * sqrt_l * s) / l;
    result[4] = (yy + (xx + zz) * c) / l;
    result[5] = (yz * one_c + v0[0] * sqrt_l * s) / l;
    result[6] = (xz * one_c + v0[1] * sqrt_l * s) / l;
    result[7] = (yz * one_c - v0[0] * sqrt_l * s) / l;
    result[8] = (zz + (xx + yy) * c) / l;
    return result;
}

mreal_t *mat3_rotation_quat(mreal_t *result, const mreal_t *q0) {
    mreal_t xx = q0[0] * q0[0];
    mreal_t yy = q0[1] * q0[1];
    mreal_t zz = q0[2] * q0[2];
    mreal_t xy = q0[0] * q0[1];
    mreal_t zw = q0[2] * q0[3];
    mreal_t xz = q0[8] * q0[0];
    mreal_t yw = q0[1] * q0[3];
    mreal_t yz = q0[1] * q0[2];
    mreal_t xw = q0[0] * q0[3];
    result[0] = MREAL_C(1.0) - MREAL_C(2.0) * (yy - zz);
    result[1] = MREAL_C(2.0) * (xy + zw);
    result[2] = MREAL_C(2.0) * (xz - yw);
    result[3] = MREAL_C(2.0) * (xy - zw);
    result[4] = MREAL_C(1.0) - MREAL_C(2.0) * (xx - zz);
    result[5] = MREAL_C(2.0) * (yz + xw);
    result[6] = MREAL_C(2.0) * (xz + yw);
    result[7] = MREAL_C(2.0) * (yz - xw);
    result[8] = MREAL_C(1.0) - MREAL_C(2.0) * (xx - yy);
    return result;
}

mreal_t *mat3_lerp(mreal_t *result, const mreal_t *m0, const mreal_t *m1, mreal_t f) {
    result[0] = m0[0] + (m1[0] - m0[0]) * f;
    result[1] = m0[1] + (m1[1] - m0[1]) * f;
    result[2] = m0[2] + (m1[2] - m0[2]) * f;
    result[3] = m0[3] + (m1[3] - m0[3]) * f;
    result[4] = m0[4] + (m1[4] - m0[4]) * f;
    result[5] = m0[5] + (m1[5] - m0[5]) * f;
    result[6] = m0[6] + (m1[6] - m0[6]) * f;
    result[7] = m0[7] + (m1[7] - m0[7]) * f;
    result[8] = m0[8] + (m1[8] - m0[8]) * f;
    return result;
}

mreal_t *mat4(mreal_t *result, mreal_t m11, mreal_t m12, mreal_t m13, mreal_t m14, mreal_t m21, mreal_t m22, mreal_t m23, mreal_t m24, mreal_t m31, mreal_t m32, mreal_t m33, mreal_t m34, mreal_t m41, mreal_t m42, mreal_t m43, mreal_t m44) {
    result[0] = m11;
    result[1] = m21;
    result[2] = m31;
    result[3] = m41;
    result[4] = m12;
    result[5] = m22;
    result[6] = m32;
    result[7] = m42;
    result[8] = m13;
    result[9] = m23;
    result[10] = m33;
    result[11] = m43;
    result[12] = m14;
    result[13] = m24;
    result[14] = m34;
    result[15] = m44;
    return result;
}

mreal_t *mat4_zero(mreal_t *result) {
    result[0] = MREAL_C(0.0);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    result[4] = MREAL_C(0.0);
    result[5] = MREAL_C(0.0);
    result[6] = MREAL_C(0.0);
    result[7] = MREAL_C(0.0);
    result[8] = MREAL_C(0.0);
    result[9] = MREAL_C(0.0);
    result[10] = MREAL_C(0.0);
    result[11] = MREAL_C(0.0);
    result[12] = MREAL_C(0.0);
    result[13] = MREAL_C(0.0);
    result[14] = MREAL_C(0.0);
    result[15] = MREAL_C(0.0);
    return result;
}

mreal_t *mat4_identity(mreal_t *result) {
    result[0] = MREAL_C(1.0);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    result[4] = MREAL_C(0.0);
    result[5] = MREAL_C(1.0);
    result[6] = MREAL_C(0.0);
    result[7] = MREAL_C(0.0);
    result[8] = MREAL_C(0.0);
    result[9] = MREAL_C(0.0);
    result[10] = MREAL_C(1.0);
    result[11] = MREAL_C(0.0);
    result[12] = MREAL_C(0.0);
    result[13] = MREAL_C(0.0);
    result[14] = MREAL_C(0.0);
    result[15] = MREAL_C(1.0);
    return result;
}

mreal_t mat4_determinant(const mreal_t *m0) {
    mreal_t m11 = m0[0];
    mreal_t m21 = m0[1];
    mreal_t m31 = m0[2];
    mreal_t m41 = m0[3];
    mreal_t m12 = m0[4];
    mreal_t m22 = m0[5];
    mreal_t m32 = m0[6];
    mreal_t m42 = m0[7];
    mreal_t m13 = m0[8];
    mreal_t m23 = m0[9];
    mreal_t m33 = m0[10];
    mreal_t m43 = m0[11];
    mreal_t m14 = m0[12];
    mreal_t m24 = m0[13];
    mreal_t m34 = m0[14];
    mreal_t m44 = m0[15];
    mreal_t determinant = m14 * m23 * m32 * m41 - m13 * m24 * m32 * m41
        - m14 * m22 * m33 * m41 + m12 * m24 * m33 * m41
        + m13 * m22 * m34 * m41 - m12 * m23 * m34 * m41
        - m14 * m23 * m31 * m42 + m13 * m24 * m31 * m42
        + m14 * m21 * m33 * m42 - m11 * m24 * m33 * m42
        - m13 * m21 * m34 * m42 + m11 * m23 * m34 * m42
        + m14 * m22 * m31 * m43 - m12 * m24 * m31 * m43
        - m14 * m21 * m32 * m43 + m11 * m24 * m32 * m43
        + m12 * m21 * m34 * m43 - m11 * m22 * m34 * m43
        - m13 * m22 * m31 * m44 + m12 * m23 * m31 * m44
        + m13 * m21 * m32 * m44 - m11 * m23 * m32 * m44
        - m12 * m21 * m33 * m44 + m11 * m22 * m33 * m44;
    return determinant;
}

mreal_t *mat4_assign(mreal_t *result, const mreal_t *m0) {
    result[0] = m0[0];
    result[1] = m0[1];
    result[2] = m0[2];
    result[3] = m0[3];
    result[4] = m0[4];
    result[5] = m0[5];
    result[6] = m0[6];
    result[7] = m0[7];
    result[8] = m0[8];
    result[9] = m0[9];
    result[10] = m0[10];
    result[11] = m0[11];
    result[12] = m0[12];
    result[13] = m0[13];
    result[14] = m0[14];
    result[15] = m0[15];
    return result;
}

mreal_t *mat4_negative(mreal_t *result, const mreal_t *m0) {
    result[0] = -m0[0];
    result[1] = -m0[1];
    result[2] = -m0[2];
    result[3] = -m0[3];
    result[4] = -m0[4];
    result[5] = -m0[5];
    result[6] = -m0[6];
    result[7] = -m0[7];
    result[8] = -m0[8];
    result[9] = -m0[9];
    result[10] = -m0[10];
    result[11] = -m0[11];
    result[12] = -m0[12];
    result[13] = -m0[13];
    result[14] = -m0[14];
    result[15] = -m0[15];
    return result;
}

mreal_t *mat4_transpose(mreal_t *result, const mreal_t *m0) {
    mreal_t transposed[MAT4_SIZE];
    transposed[0] = m0[0];
    transposed[1] = m0[4];
    transposed[2] = m0[8];
    transposed[3] = m0[12];
    transposed[4] = m0[1];
    transposed[5] = m0[5];
    transposed[6] = m0[9];
    transposed[7] = m0[13];
    transposed[8] = m0[2];
    transposed[9] = m0[6];
    transposed[10] = m0[10];
    transposed[11] = m0[14];
    transposed[12] = m0[3];
    transposed[13] = m0[7];
    transposed[14] = m0[11];
    transposed[15] = m0[15];
    result[0] = transposed[0];
    result[1] = transposed[1];
    result[2] = transposed[2];
    result[3] = transposed[3];
    result[4] = transposed[4];
    result[5] = transposed[5];
    result[6] = transposed[6];
    result[7] = transposed[7];
    result[8] = transposed[8];
    result[9] = transposed[9];
    result[10] = transposed[10];
    result[11] = transposed[11];
    result[12] = transposed[12];
    result[13] = transposed[13];
    result[14] = transposed[14];
    result[15] = transposed[15];
    return result;
}

mreal_t *mat4_cofactor(mreal_t *result, const mreal_t *m0) {
    mreal_t cofactor[MAT4_SIZE];
    mreal_t minor[MAT3_SIZE];
    minor[0] = m0[5];
    minor[1] = m0[6];
    minor[2] = m0[7];
    minor[3] = m0[9];
    minor[4] = m0[10];
    minor[5] = m0[11];
    minor[6] = m0[13];
    minor[7] = m0[14];
    minor[8] = m0[15];
    cofactor[0] = mat3_determinant(minor);
    minor[0] = m0[4];
    minor[1] = m0[6];
    minor[2] = m0[7];
    minor[3] = m0[8];
    minor[4] = m0[10];
    minor[5] = m0[11];
    minor[6] = m0[12];
    minor[7] = m0[14];
    minor[8] = m0[15];
    cofactor[1] = -mat3_determinant(minor);
    minor[0] = m0[4];
    minor[1] = m0[5];
    minor[2] = m0[7];
    minor[3] = m0[8];
    minor[4] = m0[9];
    minor[5] = m0[11];
    minor[6] = m0[12];
    minor[7] = m0[13];
    minor[8] = m0[15];
    cofactor[2] = mat3_determinant(minor);
    minor[0] = m0[4];
    minor[1] = m0[5];
    minor[2] = m0[6];
    minor[3] = m0[8];
    minor[4] = m0[9];
    minor[5] = m0[10];
    minor[6] = m0[12];
    minor[7] = m0[13];
    minor[8] = m0[14];
    cofactor[3] = -mat3_determinant(minor);
    minor[0] = m0[1];
    minor[1] = m0[2];
    minor[2] = m0[3];
    minor[3] = m0[9];
    minor[4] = m0[10];
    minor[5] = m0[11];
    minor[6] = m0[13];
    minor[7] = m0[14];
    minor[8] = m0[15];
    cofactor[4] = -mat3_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[2];
    minor[2] = m0[3];
    minor[3] = m0[8];
    minor[4] = m0[10];
    minor[5] = m0[11];
    minor[6] = m0[12];
    minor[7] = m0[14];
    minor[8] = m0[15];
    cofactor[5] = mat3_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[1];
    minor[2] = m0[3];
    minor[3] = m0[8];
    minor[4] = m0[9];
    minor[5] = m0[11];
    minor[6] = m0[12];
    minor[7] = m0[13];
    minor[8] = m0[15];
    cofactor[6] = -mat3_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[1];
    minor[2] = m0[2];
    minor[3] = m0[8];
    minor[4] = m0[9];
    minor[5] = m0[10];
    minor[6] = m0[12];
    minor[7] = m0[13];
    minor[8] = m0[14];
    cofactor[7] = mat3_determinant(minor);
    minor[0] = m0[1];
    minor[1] = m0[2];
    minor[2] = m0[3];
    minor[3] = m0[5];
    minor[4] = m0[6];
    minor[5] = m0[7];
    minor[6] = m0[13];
    minor[7] = m0[14];
    minor[8] = m0[15];
    cofactor[8] = mat3_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[2];
    minor[2] = m0[3];
    minor[3] = m0[4];
    minor[4] = m0[6];
    minor[5] = m0[7];
    minor[6] = m0[12];
    minor[7] = m0[14];
    minor[8] = m0[15];
    cofactor[9] = -mat3_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[1];
    minor[2] = m0[3];
    minor[3] = m0[4];
    minor[4] = m0[5];
    minor[5] = m0[7];
    minor[6] = m0[12];
    minor[7] = m0[13];
    minor[8] = m0[15];
    cofactor[10] = mat3_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[1];
    minor[2] = m0[2];
    minor[3] = m0[4];
    minor[4] = m0[5];
    minor[5] = m0[6];
    minor[6] = m0[12];
    minor[7] = m0[13];
    minor[8] = m0[14];
    cofactor[11] = -mat3_determinant(minor);
    minor[0] = m0[1];
    minor[1] = m0[2];
    minor[2] = m0[3];
    minor[3] = m0[5];
    minor[4] = m0[6];
    minor[5] = m0[7];
    minor[6] = m0[9];
    minor[7] = m0[10];
    minor[8] = m0[11];
    cofactor[12] = -mat3_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[2];
    minor[2] = m0[3];
    minor[3] = m0[4];
    minor[4] = m0[6];
    minor[5] = m0[7];
    minor[6] = m0[8];
    minor[7] = m0[10];
    minor[8] = m0[11];
    cofactor[13] = mat3_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[1];
    minor[2] = m0[3];
    minor[3] = m0[4];
    minor[4] = m0[5];
    minor[5] = m0[7];
    minor[6] = m0[8];
    minor[7] = m0[9];
    minor[8] = m0[11];
    cofactor[14] = -mat3_determinant(minor);
    minor[0] = m0[0];
    minor[1] = m0[1];
    minor[2] = m0[2];
    minor[3] = m0[4];
    minor[4] = m0[5];
    minor[5] = m0[6];
    minor[6] = m0[8];
    minor[7] = m0[9];
    minor[8] = m0[10];
    cofactor[15] = mat3_determinant(minor);
    result[0] = cofactor[0];
    result[1] = cofactor[1];
    result[2] = cofactor[2];
    result[3] = cofactor[3];
    result[4] = cofactor[4];
    result[5] = cofactor[5];
    result[6] = cofactor[6];
    result[7] = cofactor[7];
    result[8] = cofactor[8];
    result[9] = cofactor[9];
    result[10] = cofactor[10];
    result[11] = cofactor[11];
    result[12] = cofactor[12];
    result[13] = cofactor[13];
    result[14] = cofactor[14];
    result[15] = cofactor[15];
    return result;
}

mreal_t *mat4_rotation_x(mreal_t *result, mreal_t f) {
    mreal_t c = MCOS(f);
    mreal_t s = MSIN(f);
    result[5] = c;
    result[6] = s;
    result[9] = -s;
    result[10] = c;
    return result;
}

mreal_t *mat4_rotation_y(mreal_t *result, mreal_t f) {
    mreal_t c = MCOS(f);
    mreal_t s = MSIN(f);
    result[0] = c;
    result[2] = -s;
    result[8] = s;
    result[10] = c;
    return result;
}

mreal_t *mat4_rotation_z(mreal_t *result, mreal_t f) {
    mreal_t c = MCOS(f);
    mreal_t s = MSIN(f);
    result[0] = c;
    result[1] = s;
    result[4] = -s;
    result[5] = c;
    return result;
}

mreal_t *mat4_rotation_axis(mreal_t *result, const mreal_t *v0, mreal_t f) {
#if defined(CGEOM_PRECISE_TRIGONOMETRY)
    sincos1cos sc1c = sncs1cs(f);
    mreal_t c = sc1c.cos, s = sc1c.sin, one_c = sc1c.omc;
#else
    mreal_t c = MCOS(f);
    mreal_t s = MSIN(f);
    mreal_t one_c = MREAL_C(1.0) - c;
#endif
    mreal_t x = v0[0];
    mreal_t y = v0[1];
    mreal_t z = v0[2];
    mreal_t xx = x * x;
    mreal_t xy = x * y;
    mreal_t xz = x * z;
    mreal_t yy = y * y;
    mreal_t yz = y * z;
    mreal_t zz = z * z;
    mreal_t l = xx + yy + zz;
    mreal_t sqrt_l = MSQRT(l);
    result[0] = (xx + (yy + zz) * c) / l;
    result[1] = (xy * one_c + v0[2] * sqrt_l * s) / l;
    result[2] = (xz * one_c - v0[1] * sqrt_l * s) / l;
    result[3] = MREAL_C(0.0);
    result[4] = (xy * one_c - v0[2] * sqrt_l * s) / l;
    result[5] = (yy + (xx + zz) * c) / l;
    result[6] = (yz * one_c + v0[0] * sqrt_l * s) / l;
    result[7] = MREAL_C(0.0);
    result[8] = (xz * one_c + v0[1] * sqrt_l * s) / l;
    result[9] = (yz * one_c - v0[0] * sqrt_l * s) / l;
    result[10] = (zz + (xx + yy) * c) / l;
    result[11] = MREAL_C(0.0);
    result[12] = MREAL_C(0.0);
    result[13] = MREAL_C(0.0);
    result[14] = MREAL_C(0.0);
    result[15] = MREAL_C(1.0);
    return result;
}

mreal_t *mat4_rotation_quat(mreal_t *result, const mreal_t *q0) {
    mreal_t xx = q0[0] * q0[0];
    mreal_t yy = q0[1] * q0[1];
    mreal_t zz = q0[2] * q0[2];
    mreal_t xy = q0[0] * q0[1];
    mreal_t zw = q0[2] * q0[3];
    mreal_t xz = q0[0] * q0[2];
    mreal_t yw = q0[1] * q0[3];
    mreal_t yz = q0[1] * q0[2];
    mreal_t xw = q0[0] * q0[3];
    result[0] = MREAL_C(1.0) - MREAL_C(2.0) * (yy + zz);
    result[1] = MREAL_C(2.0) * (xy + zw);
    result[2] = MREAL_C(2.0) * (xz - yw);
    result[3] = MREAL_C(0.0);
    result[4] = MREAL_C(2.0) * (xy - zw);
    result[5] = MREAL_C(1.0) - MREAL_C(2.0) * (xx + zz);
    result[6] = MREAL_C(2.0) * (yz + xw);
    result[7] = MREAL_C(0.0);
    result[8] = MREAL_C(2.0) * (xz + yw);
    result[9] = MREAL_C(2.0) * (yz - xw);
    result[10] = MREAL_C(1.0) - MREAL_C(2.0) * (xx + yy);
    result[11] = MREAL_C(0.0);
    result[12] = MREAL_C(0.0);
    result[13] = MREAL_C(0.0);
    result[14] = MREAL_C(0.0);
    result[15] = MREAL_C(1.0);
    return result;
}

mreal_t *mat4_translation(mreal_t *result, const mreal_t *m0, const mreal_t *v0) {
    result[0] = m0[0];
    result[1] = m0[1];
    result[2] = m0[2];
    result[3] = m0[3];
    result[4] = m0[4];
    result[5] = m0[5];
    result[6] = m0[6];
    result[7] = m0[7];
    result[8] = m0[8];
    result[9] = m0[9];
    result[10] = m0[10];
    result[11] = m0[11];
    result[12] = v0[0];
    result[13] = v0[1];
    result[14] = v0[2];
    result[15] = m0[15];
    return result;
}

mreal_t *mat4_translate(mreal_t *result, const mreal_t *m0, const mreal_t *v0) {
    result[0] = m0[0];
    result[1] = m0[1];
    result[2] = m0[2];
    result[3] = m0[3];
    result[4] = m0[4];
    result[5] = m0[5];
    result[6] = m0[6];
    result[7] = m0[7];
    result[8] = m0[8];
    result[9] = m0[9];
    result[10] = m0[10];
    result[11] = m0[11];
    result[12] = m0[12] + v0[0];
    result[13] = m0[13] + v0[1];
    result[14] = m0[14] + v0[2];
    result[15] = m0[15];
    return result;
}

mreal_t *mat4_scaling(mreal_t *result, const mreal_t *m0, const mreal_t *v0) {
    result[0] = v0[0];
    result[1] = m0[1];
    result[2] = m0[2];
    result[3] = m0[3];
    result[4] = m0[4];
    result[5] = v0[1];
    result[6] = m0[6];
    result[7] = m0[7];
    result[8] = m0[8];
    result[9] = m0[9];
    result[10] = v0[2];
    result[11] = m0[11];
    result[12] = m0[12];
    result[13] = m0[13];
    result[14] = m0[14];
    result[15] = m0[15];
    return result;
}

mreal_t *mat4_scale(mreal_t *result, const mreal_t *m0, const mreal_t *v0) {
    result[0] = m0[0] * v0[0];
    result[1] = m0[1];
    result[2] = m0[2];
    result[3] = m0[3];
    result[4] = m0[4];
    result[5] = m0[5] * v0[1];
    result[6] = m0[6];
    result[7] = m0[7];
    result[8] = m0[8];
    result[9] = m0[9];
    result[10] = m0[10] * v0[2];
    result[11] = m0[11];
    result[12] = m0[12];
    result[13] = m0[13];
    result[14] = m0[14];
    result[15] = m0[15];
    return result;
}

mreal_t *mat4_multiply(mreal_t *result, const mreal_t *m0, const mreal_t *m1) {
    mreal_t multiplied[MAT4_SIZE];
    multiplied[0] = m0[0] * m1[0] + m0[4] * m1[1] + m0[8] * m1[2] + m0[12] * m1[3];
    multiplied[1] = m0[1] * m1[0] + m0[5] * m1[1] + m0[9] * m1[2] + m0[13] * m1[3];
    multiplied[2] = m0[2] * m1[0] + m0[6] * m1[1] + m0[10] * m1[2] + m0[14] * m1[3];
    multiplied[3] = m0[3] * m1[0] + m0[7] * m1[1] + m0[11] * m1[2] + m0[15] * m1[3];
    multiplied[4] = m0[0] * m1[4] + m0[4] * m1[5] + m0[8] * m1[6] + m0[12] * m1[7];
    multiplied[5] = m0[1] * m1[4] + m0[5] * m1[5] + m0[9] * m1[6] + m0[13] * m1[7];
    multiplied[6] = m0[2] * m1[4] + m0[6] * m1[5] + m0[10] * m1[6] + m0[14] * m1[7];
    multiplied[7] = m0[3] * m1[4] + m0[7] * m1[5] + m0[11] * m1[6] + m0[15] * m1[7];
    multiplied[8] = m0[0] * m1[8] + m0[4] * m1[9] + m0[8] * m1[10] + m0[12] * m1[11];
    multiplied[9] = m0[1] * m1[8] + m0[5] * m1[9] + m0[9] * m1[10] + m0[13] * m1[11];
    multiplied[10] = m0[2] * m1[8] + m0[6] * m1[9] + m0[10] * m1[10] + m0[14] * m1[11];
    multiplied[11] = m0[3] * m1[8] + m0[7] * m1[9] + m0[11] * m1[10] + m0[15] * m1[11];
    multiplied[12] = m0[0] * m1[12] + m0[4] * m1[13] + m0[8] * m1[14] + m0[12] * m1[15];
    multiplied[13] = m0[1] * m1[12] + m0[5] * m1[13] + m0[9] * m1[14] + m0[13] * m1[15];
    multiplied[14] = m0[2] * m1[12] + m0[6] * m1[13] + m0[10] * m1[14] + m0[14] * m1[15];
    multiplied[15] = m0[3] * m1[12] + m0[7] * m1[13] + m0[11] * m1[14] + m0[15] * m1[15];
    result[0] = multiplied[0];
    result[1] = multiplied[1];
    result[2] = multiplied[2];
    result[3] = multiplied[3];
    result[4] = multiplied[4];
    result[5] = multiplied[5];
    result[6] = multiplied[6];
    result[7] = multiplied[7];
    result[8] = multiplied[8];
    result[9] = multiplied[9];
    result[10] = multiplied[10];
    result[11] = multiplied[11];
    result[12] = multiplied[12];
    result[13] = multiplied[13];
    result[14] = multiplied[14];
    result[15] = multiplied[15];
    return result;
}

mreal_t *mat4_multiply_f(mreal_t *result, const mreal_t *m0, mreal_t f) {
    result[0] = m0[0] * f;
    result[1] = m0[1] * f;
    result[2] = m0[2] * f;
    result[3] = m0[3] * f;
    result[4] = m0[4] * f;
    result[5] = m0[5] * f;
    result[6] = m0[6] * f;
    result[7] = m0[7] * f;
    result[8] = m0[8] * f;
    result[9] = m0[9] * f;
    result[10] = m0[10] * f;
    result[11] = m0[11] * f;
    result[12] = m0[12] * f;
    result[13] = m0[13] * f;
    result[14] = m0[14] * f;
    result[15] = m0[15] * f;
    return result;
}

mreal_t *mat4_inverse(mreal_t *result, const mreal_t *m0) {
    mreal_t inverse[MAT4_SIZE];
    mreal_t inverted_determinant;
    mreal_t m11 = m0[0];
    mreal_t m21 = m0[1];
    mreal_t m31 = m0[2];
    mreal_t m41 = m0[3];
    mreal_t m12 = m0[4];
    mreal_t m22 = m0[5];
    mreal_t m32 = m0[6];
    mreal_t m42 = m0[7];
    mreal_t m13 = m0[8];
    mreal_t m23 = m0[9];
    mreal_t m33 = m0[10];
    mreal_t m43 = m0[11];
    mreal_t m14 = m0[12];
    mreal_t m24 = m0[13];
    mreal_t m34 = m0[14];
    mreal_t m44 = m0[15];
    inverse[0] = m22 * m33 * m44
        - m22 * m43 * m34
        - m23 * m32 * m44
        + m23 * m42 * m34
        + m24 * m32 * m43
        - m24 * m42 * m33;
    inverse[4] = -m12 * m33 * m44
        + m12 * m43 * m34
        + m13 * m32 * m44
        - m13 * m42 * m34
        - m14 * m32 * m43
        + m14 * m42 * m33;
    inverse[8] = m12 * m23 * m44
        - m12 * m43 * m24
        - m13 * m22 * m44
        + m13 * m42 * m24
        + m14 * m22 * m43
        - m14 * m42 * m23;
    inverse[12] = -m12 * m23 * m34
        + m12 * m33 * m24
        + m13 * m22 * m34
        - m13 * m32 * m24
        - m14 * m22 * m33
        + m14 * m32 * m23;
    inverse[1] = -m21 * m33 * m44
        + m21 * m43 * m34
        + m23 * m31 * m44
        - m23 * m41 * m34
        - m24 * m31 * m43
        + m24 * m41 * m33;
    inverse[5] =m11 * m33 * m44
        -m11 * m43 * m34
        - m13 * m31 * m44
        + m13 * m41 * m34
        + m14 * m31 * m43
        - m14 * m41 * m33;
    inverse[9] = -m11 * m23 * m44
        +m11 * m43 * m24
        + m13 * m21 * m44
        - m13 * m41 * m24
        - m14 * m21 * m43
        + m14 * m41 * m23;
    inverse[13] =m11 * m23 * m34
        -m11 * m33 * m24
        - m13 * m21 * m34
        + m13 * m31 * m24
        + m14 * m21 * m33
        - m14 * m31 * m23;
    inverse[2] = m21 * m32 * m44
        - m21 * m42 * m34
        - m22 * m31 * m44
        + m22 * m41 * m34
        + m24 * m31 * m42
        - m24 * m41 * m32;
    inverse[6] = -m11 * m32 * m44
        +m11 * m42 * m34
        + m12 * m31 * m44
        - m12 * m41 * m34
        - m14 * m31 * m42
        + m14 * m41 * m32;
    inverse[10] =m11 * m22 * m44
        -m11 * m42 * m24
        - m12 * m21 * m44
        + m12 * m41 * m24
        + m14 * m21 * m42
        - m14 * m41 * m22;
    inverse[14] = -m11 * m22 * m34
        +m11 * m32 * m24
        + m12 * m21 * m34
        - m12 * m31 * m24
        - m14 * m21 * m32
        + m14 * m31 * m22;
    inverse[3] = -m21 * m32 * m43
        + m21 * m42 * m33
        + m22 * m31 * m43
        - m22 * m41 * m33
        - m23 * m31 * m42
        + m23 * m41 * m32;
    inverse[7] =m11 * m32 * m43
        -m11 * m42 * m33
        - m12 * m31 * m43
        + m12 * m41 * m33
        + m13 * m31 * m42
        - m13 * m41 * m32;
    inverse[11] = -m11 * m22 * m43
        +m11 * m42 * m23
        + m12 * m21 * m43
        - m12 * m41 * m23
        - m13 * m21 * m42
        + m13 * m41 * m22;
    inverse[15] =m11 * m22 * m33
        -m11 * m32 * m23
        - m12 * m21 * m33
        + m12 * m31 * m23
        + m13 * m21 * m32
        - m13 * m31 * m22;
    inverted_determinant = MREAL_C(1.0) / (m11 * inverse[0] + m21 * inverse[4] + m31 * inverse[8] + m41 * inverse[12]);
    result[0] = inverse[0] * inverted_determinant;
    result[1] = inverse[1] * inverted_determinant;
    result[2] = inverse[2] * inverted_determinant;
    result[3] = inverse[3] * inverted_determinant;
    result[4] = inverse[4] * inverted_determinant;
    result[5] = inverse[5] * inverted_determinant;
    result[6] = inverse[6] * inverted_determinant;
    result[7] = inverse[7] * inverted_determinant;
    result[8] = inverse[8] * inverted_determinant;
    result[9] = inverse[9] * inverted_determinant;
    result[10] = inverse[10] * inverted_determinant;
    result[11] = inverse[11] * inverted_determinant;
    result[12] = inverse[12] * inverted_determinant;
    result[13] = inverse[13] * inverted_determinant;
    result[14] = inverse[14] * inverted_determinant;
    result[15] = inverse[15] * inverted_determinant;
    return result;
}

mreal_t *mat4_lerp(mreal_t *result, const mreal_t *m0, const mreal_t *m1, mreal_t f) {
    result[0] = m0[0] + (m1[0] - m0[0]) * f;
    result[1] = m0[1] + (m1[1] - m0[1]) * f;
    result[2] = m0[2] + (m1[2] - m0[2]) * f;
    result[3] = m0[3] + (m1[3] - m0[3]) * f;
    result[4] = m0[4] + (m1[4] - m0[4]) * f;
    result[5] = m0[5] + (m1[5] - m0[5]) * f;
    result[6] = m0[6] + (m1[6] - m0[6]) * f;
    result[7] = m0[7] + (m1[7] - m0[7]) * f;
    result[8] = m0[8] + (m1[8] - m0[8]) * f;
    result[9] = m0[9] + (m1[9] - m0[9]) * f;
    result[10] = m0[10] + (m1[10] - m0[10]) * f;
    result[11] = m0[11] + (m1[11] - m0[11]) * f;
    result[12] = m0[12] + (m1[12] - m0[12]) * f;
    result[13] = m0[13] + (m1[13] - m0[13]) * f;
    result[14] = m0[14] + (m1[14] - m0[14]) * f;
    result[15] = m0[15] + (m1[15] - m0[15]) * f;
    return result;
}

mreal_t *mat4_look_at(mreal_t *result, const mreal_t *position, const mreal_t *target, const mreal_t *up) {
    mreal_t tmp_forward[VEC3_SIZE];
    mreal_t tmp_side[VEC3_SIZE];
    mreal_t tmp_up[VEC3_SIZE];
    vec3_subtract(tmp_forward, target, position);
    vec3_normalize(tmp_forward, tmp_forward);
    vec3_cross(tmp_side, tmp_forward, up);
    vec3_normalize(tmp_side, tmp_side);
    vec3_cross(tmp_up, tmp_side, tmp_forward);
    result[0] = tmp_side[0];
    result[1] = tmp_up[0];
    result[2] = -tmp_forward[0];
    result[3] = MREAL_C(0.0);
    result[4] = tmp_side[1];
    result[5] = tmp_up[1];
    result[6] = -tmp_forward[1];
    result[7] = MREAL_C(0.0);
    result[8] = tmp_side[2];
    result[9] = tmp_up[2];
    result[10] = -tmp_forward[2];
    result[11] = MREAL_C(0.0);
    result[12] = -vec3_dot(tmp_side, position);
    result[13] = -vec3_dot(tmp_up, position);
    result[14] = vec3_dot(tmp_forward, position);
    result[15] = MREAL_C(1.0);
    return result;
}

mreal_t *mat4_ortho(mreal_t *result, mreal_t l, mreal_t r, mreal_t b, mreal_t t, mreal_t n, mreal_t f) {
    result[0] = MREAL_C(2.0) / (r - l);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    result[4] = MREAL_C(0.0);
    result[5] = MREAL_C(2.0) / (t - b);
    result[6] = MREAL_C(0.0);
    result[7] = MREAL_C(0.0);
    result[8] = MREAL_C(0.0);
    result[9] = MREAL_C(0.0);
    result[10] = -MREAL_C(2.0) / (f - n);
    result[11] = MREAL_C(0.0);
    result[12] = -((r + l) / (r - l));
    result[13] = -((t + b) / (t - b));
    result[14] = -((f + n) / (f - n));
    result[15] = MREAL_C(1.0);
    return result;
}

mreal_t *mat4_perspective(mreal_t *result, mreal_t fov_y, mreal_t aspect, mreal_t n, mreal_t f) {
    mreal_t tan_half_fov_y = MREAL_C(1.0) / MTAN(fov_y * MREAL_C(0.5));
    result[0] = MREAL_C(1.0) / aspect * tan_half_fov_y;
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    result[4] = MREAL_C(0.0);
    result[5] = MREAL_C(1.0) / tan_half_fov_y;
    result[6] = MREAL_C(0.0);
    result[7] = MREAL_C(0.0);
    result[8] = MREAL_C(0.0);
    result[9] = MREAL_C(0.0);
    result[10] = f / (n - f);
    result[11] = -MREAL_C(1.0);
    result[12] = MREAL_C(0.0);
    result[13] = MREAL_C(0.0);
    result[14] = -(f * n) / (f - n);
    result[15] = MREAL_C(0.0);
    return result;
}

mreal_t *mat4_perspective_fov(mreal_t *result, mreal_t fov, mreal_t w, mreal_t h, mreal_t n, mreal_t f) {
    mreal_t h2 = MCOS(fov * MREAL_C(0.5)) / MSIN(fov * MREAL_C(0.5));
    mreal_t w2 = h2 * h / w;
    result[0] = w2;
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    result[4] = MREAL_C(0.0);
    result[5] = h2;
    result[6] = MREAL_C(0.0);
    result[7] = MREAL_C(0.0);
    result[8] = MREAL_C(0.0);
    result[9] = MREAL_C(0.0);
    result[10] = f / (n - f);
    result[11] = -MREAL_C(1.0);
    result[12] = MREAL_C(0.0);
    result[13] = MREAL_C(0.0);
    result[14] = -(f * n) / (f - n);
    result[15] = MREAL_C(0.0);
    return result;
}

mreal_t *mat4_perspective_infinite(mreal_t *result, mreal_t fov_y, mreal_t aspect, mreal_t n) {
    mreal_t range = MTAN(fov_y * MREAL_C(0.5)) * n;
    mreal_t left = -range * aspect;
    mreal_t right = range * aspect;
    mreal_t top = range;
    mreal_t bottom = -range;
    result[0] = MREAL_C(2.0) * n / (right - left);
    result[1] = MREAL_C(0.0);
    result[2] = MREAL_C(0.0);
    result[3] = MREAL_C(0.0);
    result[4] = MREAL_C(0.0);
    result[5] = MREAL_C(2.0) * n / (top - bottom);
    result[6] = MREAL_C(0.0);
    result[7] = MREAL_C(0.0);
    result[8] = MREAL_C(0.0);
    result[9] = MREAL_C(0.0);
    result[10] = -MREAL_C(1.0);
    result[11] = -MREAL_C(1.0);
    result[12] = MREAL_C(0.0);
    result[13] = MREAL_C(0.0);
    result[14] = -MREAL_C(2.0) * n;
    result[15] = MREAL_C(0.0);
    return result;
}

bool psvec2i_is_zero(const struct vec2i *v0) {
    return vec2i_is_zero((const mint_t *)v0);
}

bool psvec2i_is_equal(const struct vec2i *v0, const struct vec2i *v1) {
    return vec2i_is_equal((const mint_t *)v0, (const mint_t *)v1);
}

struct vec2i *psvec2i(struct vec2i *result, mint_t x, mint_t y) {
    return (struct vec2i *)vec2i((mint_t *)result, x, y);
}

struct vec2i *psvec2i_assign(struct vec2i *result, const struct vec2i *v0) {
    return (struct vec2i *)vec2i_assign((mint_t *)result, (const mint_t *)v0);
}

struct vec2i *psvec2i_assign_vec2(struct vec2i *result, struct vec2 *v0) {
    return (struct vec2i *)vec2i_assign_vec2((mint_t *)result, (const mreal_t *)v0);
}

struct vec2i *psvec2i_zero(struct vec2i *result) {
    return (struct vec2i *)vec2i_zero((mint_t *)result);
}

struct vec2i *psvec2i_one(struct vec2i *result) {
    return (struct vec2i *)vec2i_one((mint_t *)result);
}

struct vec2i *psvec2i_sign(struct vec2i *result, const struct vec2i *v0) {
    return (struct vec2i *)vec2i_sign((mint_t *)result, (const mint_t *)v0);
}

struct vec2i *psvec2i_add(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1) {
    return (struct vec2i *)vec2i_add((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec2i *psvec2i_add_i(struct vec2i *result, const struct vec2i *v0, mint_t i) {
    return (struct vec2i *)vec2i_add_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec2i *psvec2i_subtract(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1) {
    return (struct vec2i *)vec2i_subtract((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec2i *psvec2i_subtract_i(struct vec2i *result, const struct vec2i *v0, mint_t i) {
    return (struct vec2i *)vec2i_subtract_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec2i *psvec2i_multiply(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1) {
    return (struct vec2i *)vec2i_multiply((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec2i *psvec2i_multiply_i(struct vec2i *result, const struct vec2i *v0, mint_t i) {
    return (struct vec2i *)vec2i_multiply_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec2i *psvec2i_divide(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1) {
    return (struct vec2i *)vec2i_divide((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec2i *psvec2i_divide_i(struct vec2i *result, const struct vec2i *v0, mint_t i) {
    return (struct vec2i *)vec2i_divide_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec2i *psvec2i_snap(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1) {
    return (struct vec2i *)vec2i_snap((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec2i *psvec2i_snap_i(struct vec2i *result, const struct vec2i *v0, mint_t i) {
    return (struct vec2i *)vec2i_snap_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec2i *psvec2i_negative(struct vec2i *result, const struct vec2i *v0) {
    return (struct vec2i *)vec2i_negative((mint_t *)result, (const mint_t *)v0);
}

struct vec2i *psvec2i_abs(struct vec2i *result, const struct vec2i *v0) {
    return (struct vec2i *)vec2i_abs((mint_t *)result, (const mint_t *)v0);
}

struct vec2i *psvec2i_max(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1) {
    return (struct vec2i *)vec2i_max((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec2i *psvec2i_min(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1) {
    return (struct vec2i *)vec2i_min((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec2i *psvec2i_clamp(struct vec2i *result, const struct vec2i *v0, const struct vec2i *v1, struct vec2i *v2) {
    return (struct vec2i *)vec2i_clamp((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1, (const mint_t *)v2);
}

struct vec2i *psvec2i_tangent(struct vec2i *result, const struct vec2i *v0) {
    return (struct vec2i *)vec2i_tangent((mint_t *)result, (const mint_t *)v0);
}

bool psvec3i_is_zero(const struct vec3i *v0) {
    return vec3i_is_zero((const mint_t *)v0);
}

bool psvec3i_is_equal(const struct vec3i *v0, const struct vec3i *v1) {
    return vec3i_is_equal((const mint_t *)v0, (const mint_t *)v1);
}

struct vec3i *psvec3i(struct vec3i *result, mint_t x, mint_t y, mint_t z) {
    return (struct vec3i *)vec3i((mint_t *)result, x, y, z);
}

struct vec3i *psvec3i_assign(struct vec3i *result, const struct vec3i *v0) {
    return (struct vec3i *)vec3i_assign((mint_t *)result, (const mint_t *)v0);
}

struct vec3i *psvec3i_assign_vec3(struct vec3i *result, const struct vec3 *v0) {
    return (struct vec3i *)vec3i_assign_vec3((mint_t *)result, (const mreal_t *)v0);
}

struct vec3i *psvec3i_zero(struct vec3i *result) {
    return (struct vec3i *)vec3i_zero((mint_t *)result);
}

struct vec3i *psvec3i_one(struct vec3i *result) {
    return (struct vec3i *)vec3i_one((mint_t *)result);
}

struct vec3i *psvec3i_sign(struct vec3i *result, const struct vec3i *v0) {
    return (struct vec3i *)vec3i_sign((mint_t *)result, (const mint_t *)v0);
}

struct vec3i *psvec3i_add(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1) {
    return (struct vec3i *)vec3i_add((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec3i *psvec3i_add_i(struct vec3i *result, const struct vec3i *v0, mint_t i) {
    return (struct vec3i *)vec3i_add_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec3i *psvec3i_subtract(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1) {
    return (struct vec3i *)vec3i_subtract((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec3i *psvec3i_subtract_i(struct vec3i *result, const struct vec3i *v0, mint_t i) {
    return (struct vec3i *)vec3i_subtract_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec3i *psvec3i_multiply(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1) {
    return (struct vec3i *)vec3i_multiply((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec3i *psvec3i_multiply_i(struct vec3i *result, const struct vec3i *v0, mint_t i) {
    return (struct vec3i *)vec3i_multiply_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec3i *psvec3i_divide(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1) {
    return (struct vec3i *)vec3i_divide((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec3i *psvec3i_divide_i(struct vec3i *result, const struct vec3i *v0, mint_t i) {
    return (struct vec3i *)vec3i_divide_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec3i *psvec3i_snap(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1) {
    return (struct vec3i *)vec3i_snap((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec3i *psvec3i_snap_i(struct vec3i *result, const struct vec3i *v0, mint_t i) {
    return (struct vec3i *)vec3i_snap_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec3i *psvec3i_cross(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1) {
    return (struct vec3i *)vec3i_cross((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec3i *psvec3i_negative(struct vec3i *result, const struct vec3i *v0) {
    return (struct vec3i *)vec3i_negative((mint_t *)result, (const mint_t *)v0);
}

struct vec3i *psvec3i_abs(struct vec3i *result, const struct vec3i *v0) {
    return (struct vec3i *)vec3i_abs((mint_t *)result, (const mint_t *)v0);
}

struct vec3i *psvec3i_max(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1) {
    return (struct vec3i *)vec3i_max((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec3i *psvec3i_min(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1) {
    return (struct vec3i *)vec3i_min((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec3i *psvec3i_clamp(struct vec3i *result, const struct vec3i *v0, const struct vec3i *v1, const struct vec3i *v2) {
    return (struct vec3i *)vec3i_clamp((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1, (const mint_t *)v2);
}

bool psvec4i_is_zero(struct vec4i *v0) {
    return vec4i_is_zero((const mint_t *)v0);
}

bool psvec4i_is_equal(struct vec4i *v0, struct vec4i *v1) {
    return vec4i_is_equal((const mint_t *)v0, (const mint_t *)v1);
}

struct vec4i *psvec4i(struct vec4i *result, mint_t x, mint_t y, mint_t z, mint_t w) {
    return (struct vec4i *)vec4i((mint_t *)result, x, y, z, w);
}

struct vec4i *psvec4i_assign(struct vec4i *result, struct vec4i *v0) {
    return (struct vec4i *)vec4i_assign((mint_t *)result, (const mint_t *)v0);
}

struct vec4i *psvec4i_assign_vec4(struct vec4i *result, struct vec4 *v0) {
    return (struct vec4i *)vec4i_assign_vec4((mint_t *)result, (const mreal_t *)v0);
}

struct vec4i *psvec4i_zero(struct vec4i *result) {
    return (struct vec4i *)vec4i_zero((mint_t *)result);
}

struct vec4i *psvec4i_one(struct vec4i *result) {
    return (struct vec4i *)vec4i_one((mint_t *)result);
}

struct vec4i *psvec4i_sign(struct vec4i *result, struct vec4i *v0) {
    return (struct vec4i *)vec4i_sign((mint_t *)result, (const mint_t *)v0);
}

struct vec4i *psvec4i_add(struct vec4i *result, struct vec4i *v0, struct vec4i *v1) {
    return (struct vec4i *)vec4i_add((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec4i *psvec4i_add_i(struct vec4i *result, struct vec4i *v0, mint_t i) {
    return (struct vec4i *)vec4i_add_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec4i *psvec4i_subtract(struct vec4i *result, struct vec4i *v0, struct vec4i *v1) {
    return (struct vec4i *)vec4i_subtract((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec4i *psvec4i_subtract_i(struct vec4i *result, struct vec4i *v0, mint_t i) {
    return (struct vec4i *)vec4i_subtract_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec4i *psvec4i_multiply(struct vec4i *result, struct vec4i *v0, struct vec4i *v1) {
    return (struct vec4i *)vec4i_multiply((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec4i *psvec4i_multiply_i(struct vec4i *result, struct vec4i *v0, mint_t i) {
    return (struct vec4i *)vec4i_multiply_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec4i *psvec4i_divide(struct vec4i *result, struct vec4i *v0, struct vec4i *v1) {
    return (struct vec4i *)vec4i_divide((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec4i *psvec4i_divide_i(struct vec4i *result, struct vec4i *v0, mint_t i) {
    return (struct vec4i *)vec4i_divide_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec4i *psvec4i_snap(struct vec4i *result, struct vec4i *v0, struct vec4i *v1) {
    return (struct vec4i *)vec4i_snap((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec4i *psvec4i_snap_i(struct vec4i *result, struct vec4i *v0, mint_t i) {
    return (struct vec4i *)vec4i_snap_i((mint_t *)result, (const mint_t *)v0, i);
}

struct vec4i *psvec4i_negative(struct vec4i *result, struct vec4i *v0) {
    return (struct vec4i *)vec4i_negative((mint_t *)result, (const mint_t *)v0);
}

struct vec4i *psvec4i_abs(struct vec4i *result, struct vec4i *v0) {
    return (struct vec4i *)vec4i_abs((mint_t *)result, (const mint_t *)v0);
}

struct vec4i *psvec4i_max(struct vec4i *result, struct vec4i *v0, struct vec4i *v1) {
    return (struct vec4i *)vec4i_max((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec4i *psvec4i_min(struct vec4i *result, struct vec4i *v0, struct vec4i *v1) {
    return (struct vec4i *)vec4i_min((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1);
}

struct vec4i *psvec4i_clamp(struct vec4i *result, struct vec4i *v0, struct vec4i *v1, struct vec4i *v2) {
    return (struct vec4i *)vec4i_clamp((mint_t *)result, (const mint_t *)v0, (const mint_t *)v1, (const mint_t *)v2);
}

bool psvec2_is_zero(struct vec2 *v0) {
    return vec2_is_zero((const mreal_t *)v0);
}

bool psvec2_is_equal(struct vec2 *v0, struct vec2 *v1) {
    return vec2_is_equal((const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec2 *psvec2(struct vec2 *result, mreal_t x, mreal_t y) {
    return (struct vec2 *)vec2((mreal_t *)result, x, y);
}

struct vec2 *psvec2_assign(struct vec2 *result, struct vec2 *v0) {
    return (struct vec2 *)vec2_assign((mreal_t *)result, (const mreal_t *)v0);
}

struct vec2 *psvec2_assign_vec2i(struct vec2 *result, const struct vec2i *v0) {
    return (struct vec2 *)vec2_assign_vec2i((mreal_t *)result, (const mint_t *)v0);
}

struct vec2 *psvec2_zero(struct vec2 *result) {
    return (struct vec2 *)vec2_zero((mreal_t *)result);
}

struct vec2 *psvec2_one(struct vec2 *result) {
    return (struct vec2 *)vec2_one((mreal_t *)result);
}

struct vec2 *psvec2_sign(struct vec2 *result, struct vec2 *v0) {
    return (struct vec2 *)vec2_sign((mreal_t *)result, (const mreal_t *)v0);
}

struct vec2 *psvec2_add(struct vec2 *result, struct vec2 *v0, struct vec2 *v1) {
    return (struct vec2 *)vec2_add((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec2 *psvec2_add_f(struct vec2 *result, struct vec2 *v0, mreal_t f) {
    return (struct vec2 *)vec2_add_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec2 *psvec2_subtract(struct vec2 *result, struct vec2 *v0, struct vec2 *v1) {
    return (struct vec2 *)vec2_subtract((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec2 *psvec2_subtract_f(struct vec2 *result, struct vec2 *v0, mreal_t f) {
    return (struct vec2 *)vec2_subtract_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec2 *psvec2_multiply(struct vec2 *result, struct vec2 *v0, struct vec2 *v1) {
    return (struct vec2 *)vec2_multiply((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec2 *psvec2_multiply_f(struct vec2 *result, struct vec2 *v0, mreal_t f) {
    return (struct vec2 *)vec2_multiply_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec2 *psvec2_multiply_mat2(struct vec2 *result, struct vec2 *v0, const struct mat2 *m0) {
    return (struct vec2 *)vec2_multiply_mat2((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)m0);
}

struct vec2 *psvec2_divide(struct vec2 *result, struct vec2 *v0, struct vec2 *v1) {
    return (struct vec2 *)vec2_divide((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec2 *psvec2_divide_f(struct vec2 *result, struct vec2 *v0, mreal_t f) {
    return (struct vec2 *)vec2_divide_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec2 *psvec2_snap(struct vec2 *result, struct vec2 *v0, struct vec2 *v1) {
    return (struct vec2 *)vec2_snap((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec2 *psvec2_snap_f(struct vec2 *result, struct vec2 *v0, mreal_t f) {
    return (struct vec2 *)vec2_snap_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec2 *psvec2_negative(struct vec2 *result, struct vec2 *v0) {
    return (struct vec2 *)vec2_negative((mreal_t *)result, (const mreal_t *)v0);
}

struct vec2 *psvec2_abs(struct vec2 *result, struct vec2 *v0) {
    return (struct vec2 *)vec2_abs((mreal_t *)result, (const mreal_t *)v0);
}

struct vec2 *psvec2_floor(struct vec2 *result, struct vec2 *v0) {
    return (struct vec2 *)vec2_floor((mreal_t *)result, (const mreal_t *)v0);
}

struct vec2 *psvec2_ceil(struct vec2 *result, struct vec2 *v0) {
    return (struct vec2 *)vec2_ceil((mreal_t *)result, (const mreal_t *)v0);
}

struct vec2 *psvec2_round(struct vec2 *result, struct vec2 *v0) {
    return (struct vec2 *)vec2_round((mreal_t *)result, (const mreal_t *)v0);
}

struct vec2 *psvec2_max(struct vec2 *result, struct vec2 *v0, struct vec2 *v1) {
    return (struct vec2 *)vec2_max((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec2 *psvec2_min(struct vec2 *result, struct vec2 *v0, struct vec2 *v1) {
    return (struct vec2 *)vec2_min((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec2 *psvec2_clamp(struct vec2 *result, struct vec2 *v0, struct vec2 *v1, struct vec2 *v2) {
    return (struct vec2 *)vec2_clamp((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1, (const mreal_t *)v2);
}

struct vec2 *psvec2_normalize(struct vec2 *result, struct vec2 *v0) {
    return (struct vec2 *)vec2_normalize((mreal_t *)result, (const mreal_t *)v0);
}

mreal_t psvec2_dot(struct vec2 *v0, struct vec2 *v1) {
    return vec2_dot((const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec2 *psvec2_project(struct vec2 *result, struct vec2 *v0, struct vec2 *v1) {
    return (struct vec2 *)vec2_project((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec2 *psvec2_slide(struct vec2 *result, struct vec2 *v0, struct vec2 *normal) {
    return (struct vec2 *)vec2_slide((mreal_t *)result, (const mreal_t *)v0, (mreal_t *)normal);
}

struct vec2 *psvec2_reflect(struct vec2 *result, struct vec2 *v0, struct vec2 *normal) {
    return (struct vec2 *)vec2_reflect((mreal_t *)result, (const mreal_t *)v0, (mreal_t *)normal);
}

struct vec2 *psvec2_tangent(struct vec2 *result, struct vec2 *v0) {
    return (struct vec2 *)vec2_tangent((mreal_t *)result, (const mreal_t *)v0);
}

struct vec2 *psvec2_rotate(struct vec2 *result, struct vec2 *v0, mreal_t f) {
    return (struct vec2 *)vec2_rotate((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec2 *psvec2_lerp(struct vec2 *result, struct vec2 *v0, struct vec2 *v1, mreal_t f) {
    return (struct vec2 *)vec2_lerp((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1, f);
}

struct vec2 *psvec2_bezier3(struct vec2 *result, struct vec2 *v0, struct vec2 *v1, struct vec2 *v2, mreal_t f) {
    return (struct vec2 *)vec2_bezier3((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1, (const mreal_t *)v2, f);
}

struct vec2 *psvec2_bezier4(struct vec2 *result, struct vec2 *v0, struct vec2 *v1, struct vec2 *v2, struct vec2 *v3, mreal_t f) {
    return (struct vec2 *)vec2_bezier4((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1, (const mreal_t *)v2, (const mreal_t *)v3, f);
}

mreal_t psvec2_angle(struct vec2 *v0) {
    return vec2_angle((const mreal_t *)v0);
}

mreal_t psvec2_length(struct vec2 *v0) {
    return vec2_length((const mreal_t *)v0);
}

mreal_t psvec2_length_squared(struct vec2 *v0) {
    return vec2_length_squared((const mreal_t *)v0);
}

mreal_t psvec2_distance(struct vec2 *v0, struct vec2 *v1) {
    return vec2_distance((const mreal_t *)v0, (const mreal_t *)v1);
}

mreal_t psvec2_distance_squared(struct vec2 *v0, struct vec2 *v1) {
    return vec2_distance_squared((const mreal_t *)v0, (const mreal_t *)v1);
}

bool psvec3_is_zero(const struct vec3 *v0) {
    return vec3_is_zero((const mreal_t *)v0);
}

bool psvec3_is_equal(const struct vec3 *v0, struct vec3 *v1) {
    return vec3_is_equal((const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3(struct vec3 *result, mreal_t x, mreal_t y, mreal_t z) {
    return (struct vec3 *)vec3((mreal_t *)result, x, y, z);
}

struct vec3 *psvec3_assign(struct vec3 *result, const struct vec3 *v0) {
    return (struct vec3 *)vec3_assign((mreal_t *)result, (const mreal_t *)v0);
}

struct vec3 *psvec3_assign_vec3i(struct vec3 *result, const struct vec3i *v0) {
    return (struct vec3 *)vec3_assign_vec3i((mreal_t *)result, (const mint_t *)v0);
}

struct vec3 *psvec3_zero(struct vec3 *result) {
    return (struct vec3 *)vec3_zero((mreal_t *)result);
}

struct vec3 *psvec3_one(struct vec3 *result) {
    return (struct vec3 *)vec3_one((mreal_t *)result);
}

struct vec3 *psvec3_sign(struct vec3 *result, const struct vec3 *v0) {
    return (struct vec3 *)vec3_sign((mreal_t *)result, (const mreal_t *)v0);
}

struct vec3 *psvec3_add(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1) {
    return (struct vec3 *)vec3_add((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3_add_f(struct vec3 *result, const struct vec3 *v0, mreal_t f) {
    return (struct vec3 *)vec3_add_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec3 *psvec3_subtract(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1) {
    return (struct vec3 *)vec3_subtract((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3_subtract_f(struct vec3 *result, const struct vec3 *v0, mreal_t f) {
    return (struct vec3 *)vec3_subtract_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec3 *psvec3_multiply(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1) {
    return (struct vec3 *)vec3_multiply((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3_multiply_f(struct vec3 *result, const struct vec3 *v0, mreal_t f) {
    return (struct vec3 *)vec3_multiply_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec3 *psvec3_multiply_mat3(struct vec3 *result, const struct vec3 *v0, const struct mat3 *m0) {
    return (struct vec3 *)vec3_multiply_mat3((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)m0);
}

struct vec3 *psvec3_divide(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1) {
    return (struct vec3 *)vec3_divide((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3_divide_f(struct vec3 *result, const struct vec3 *v0, mreal_t f) {
    return (struct vec3 *)vec3_divide_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec3 *psvec3_snap(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1) {
    return (struct vec3 *)vec3_snap((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3_snap_f(struct vec3 *result, const struct vec3 *v0, mreal_t f) {
    return (struct vec3 *)vec3_snap_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec3 *psvec3_negative(struct vec3 *result, const struct vec3 *v0) {
    return (struct vec3 *)vec3_negative((mreal_t *)result, (const mreal_t *)v0);
}

struct vec3 *psvec3_abs(struct vec3 *result, const struct vec3 *v0) {
    return (struct vec3 *)vec3_abs((mreal_t *)result, (const mreal_t *)v0);
}

struct vec3 *psvec3_floor(struct vec3 *result, const struct vec3 *v0) {
    return (struct vec3 *)vec3_floor((mreal_t *)result, (const mreal_t *)v0);
}

struct vec3 *psvec3_ceil(struct vec3 *result, const struct vec3 *v0) {
    return (struct vec3 *)vec3_ceil((mreal_t *)result, (const mreal_t *)v0);
}

struct vec3 *psvec3_round(struct vec3 *result, const struct vec3 *v0) {
    return (struct vec3 *)vec3_round((mreal_t *)result, (const mreal_t *)v0);
}

struct vec3 *psvec3_max(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1) {
    return (struct vec3 *)vec3_max((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3_min(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1) {
    return (struct vec3 *)vec3_min((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3_clamp(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1, struct vec3 *v2) {
    return (struct vec3 *)vec3_clamp((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1, (const mreal_t *)v2);
}

struct vec3 *psvec3_cross(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1) {
    return (struct vec3 *)vec3_cross((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3_normalize(struct vec3 *result, const struct vec3 *v0) {
    return (struct vec3 *)vec3_normalize((mreal_t *)result, (const mreal_t *)v0);
}

mreal_t psvec3_dot(const struct vec3 *v0, struct vec3 *v1) {
    return vec3_dot((const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3_project(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1) {
    return (struct vec3 *)vec3_project((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec3 *psvec3_slide(struct vec3 *result, const struct vec3 *v0, struct vec3 *normal) {
    return (struct vec3 *)vec3_slide((mreal_t *)result, (const mreal_t *)v0, (mreal_t *)normal);
}

struct vec3 *psvec3_reflect(struct vec3 *result, const struct vec3 *v0, struct vec3 *normal) {
    return (struct vec3 *)vec3_reflect((mreal_t *)result, (const mreal_t *)v0, (mreal_t *)normal);
}

struct vec3 *psvec3_rotate(struct vec3 *result, const struct vec3 *v0, struct vec3 *ra, mreal_t f) {
    return (struct vec3 *)vec3_lerp((mreal_t *)result, (const mreal_t *)v0, (mreal_t *)ra, f);
}

struct vec3 *psvec3_lerp(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1, mreal_t f) {
    return (struct vec3 *)vec3_lerp((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1, f);
}

struct vec3 *psvec3_bezier3(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1, struct vec3 *v2, mreal_t f) {
    return (struct vec3 *)vec3_bezier3((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1, (const mreal_t *)v2, f);
}

struct vec3 *psvec3_bezier4(struct vec3 *result, const struct vec3 *v0, struct vec3 *v1, struct vec3 *v2, struct vec3 *v3, mreal_t f) {
    return (struct vec3 *)vec3_bezier4((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1, (const mreal_t *)v2, (const mreal_t *)v3, f);
}

mreal_t psvec3_length(const struct vec3 *v0) {
    return vec3_length((const mreal_t *)v0);
}

mreal_t psvec3_length_squared(const struct vec3 *v0) {
    return vec3_length_squared((const mreal_t *)v0);
}

mreal_t psvec3_distance(const struct vec3 *v0, struct vec3 *v1) {
    return vec3_distance((const mreal_t *)v0, (const mreal_t *)v1);
}

mreal_t psvec3_distance_squared(const struct vec3 *v0, struct vec3 *v1) {
    return vec3_distance_squared((const mreal_t *)v0, (const mreal_t *)v1);
}

bool psvec4_is_zero(struct vec4 *v0) {
    return vec4_is_zero((const mreal_t *)v0);
}

bool psvec4_is_equal(struct vec4 *v0, struct vec4 *v1) {
    return vec4_is_equal((const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec4 *psvec4(struct vec4 *result, mreal_t x, mreal_t y, mreal_t z, mreal_t w) {
    return (struct vec4 *)vec4((mreal_t *)result, x, y, z, w);
}

struct vec4 *psvec4_assign(struct vec4 *result, struct vec4 *v0) {
    return (struct vec4 *)vec4_assign((mreal_t *)result, (const mreal_t *)v0);
}

struct vec4 *psvec4_assign_vec4i(struct vec4 *result, struct vec4i *v0) {
    return (struct vec4 *)vec4_assign_vec4i((mreal_t *)result, (const mint_t *)v0);
}

struct vec4 *psvec4_zero(struct vec4 *result) {
    return (struct vec4 *)vec4_zero((mreal_t *)result);
}

struct vec4 *psvec4_one(struct vec4 *result) {
    return (struct vec4 *)vec4_one((mreal_t *)result);
}

struct vec4 *psvec4_sign(struct vec4 *result, struct vec4 *v0) {
    return (struct vec4 *)vec4_sign((mreal_t *)result, (const mreal_t *)v0);
}

struct vec4 *psvec4_add(struct vec4 *result, struct vec4 *v0, struct vec4 *v1) {
    return (struct vec4 *)vec4_add((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec4 *psvec4_add_f(struct vec4 *result, struct vec4 *v0, mreal_t f) {
    return (struct vec4 *)vec4_add_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec4 *psvec4_subtract(struct vec4 *result, struct vec4 *v0, struct vec4 *v1) {
    return (struct vec4 *)vec4_subtract((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec4 *psvec4_subtract_f(struct vec4 *result, struct vec4 *v0, mreal_t f) {
    return (struct vec4 *)vec4_subtract_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec4 *psvec4_multiply(struct vec4 *result, struct vec4 *v0, struct vec4 *v1) {
    return (struct vec4 *)vec4_multiply((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec4 *psvec4_multiply_f(struct vec4 *result, struct vec4 *v0, mreal_t f) {
    return (struct vec4 *)vec4_multiply_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec4 *psvec4_multiply_mat4(struct vec4 *result, struct vec4 *v0, const struct mat4 *m0) {
    return (struct vec4 *)vec4_multiply_mat4((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)m0);
}

struct vec4 *psvec4_divide(struct vec4 *result, struct vec4 *v0, struct vec4 *v1) {
    return (struct vec4 *)vec4_divide((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec4 *psvec4_divide_f(struct vec4 *result, struct vec4 *v0, mreal_t f) {
    return (struct vec4 *)vec4_divide_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec4 *psvec4_snap(struct vec4 *result, struct vec4 *v0, struct vec4 *v1) {
    return (struct vec4 *)vec4_snap((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec4 *psvec4_snap_f(struct vec4 *result, struct vec4 *v0, mreal_t f) {
    return (struct vec4 *)vec4_snap_f((mreal_t *)result, (const mreal_t *)v0, f);
}

struct vec4 *psvec4_negative(struct vec4 *result, struct vec4 *v0) {
    return (struct vec4 *)vec4_negative((mreal_t *)result, (const mreal_t *)v0);
}

struct vec4 *psvec4_abs(struct vec4 *result, struct vec4 *v0) {
    return (struct vec4 *)vec4_abs((mreal_t *)result, (const mreal_t *)v0);
}

struct vec4 *psvec4_floor(struct vec4 *result, struct vec4 *v0) {
    return (struct vec4 *)vec4_floor((mreal_t *)result, (const mreal_t *)v0);
}

struct vec4 *psvec4_ceil(struct vec4 *result, struct vec4 *v0) {
    return (struct vec4 *)vec4_ceil((mreal_t *)result, (const mreal_t *)v0);
}

struct vec4 *psvec4_round(struct vec4 *result, struct vec4 *v0) {
    return (struct vec4 *)vec4_round((mreal_t *)result, (const mreal_t *)v0);
}

struct vec4 *psvec4_max(struct vec4 *result, struct vec4 *v0, struct vec4 *v1) {
    return (struct vec4 *)vec4_max((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec4 *psvec4_min(struct vec4 *result, struct vec4 *v0, struct vec4 *v1) {
    return (struct vec4 *)vec4_min((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct vec4 *psvec4_clamp(struct vec4 *result, struct vec4 *v0, struct vec4 *v1, struct vec4 *v2) {
    return (struct vec4 *)vec4_clamp((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1, (const mreal_t *)v2);
}

struct vec4 *psvec4_normalize(struct vec4 *result, struct vec4 *v0) {
    return (struct vec4 *)vec4_normalize((mreal_t *)result, (const mreal_t *)v0);
}

struct vec4 *psvec4_lerp(struct vec4 *result, struct vec4 *v0, struct vec4 *v1, mreal_t f) {
    return (struct vec4 *)vec4_lerp((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1, f);
}

bool psquat_is_zero(const struct quat *q0) {
    return quat_is_zero((const mreal_t *)q0);
}

bool psquat_is_equal(const struct quat *q0, const struct quat *q1) {
    return quat_is_equal((const mreal_t *)q0, (const mreal_t *)q1);
}

struct quat *psquat(struct quat *result, mreal_t x, mreal_t y, mreal_t z, mreal_t w) {
    return (struct quat *)quat((mreal_t *)result, x, y, z, w);
}

struct quat *psquat_assign(struct quat *result, const struct quat *q0) {
    return (struct quat *)quat_assign((mreal_t *)result, (const mreal_t *)q0);
}

struct quat *psquat_zero(struct quat *result) {
    return (struct quat *)quat_zero((mreal_t *)result);
}

struct quat *psquat_null(struct quat *result) {
    return (struct quat *)quat_null((mreal_t *)result);
}

struct quat *psquat_multiply(struct quat *result, const struct quat *q0, const struct quat *q1) {
    return (struct quat *)quat_multiply((mreal_t *)result, (const mreal_t *)q0, (const mreal_t *)q1);
}

struct quat *psquat_multiply_f(struct quat *result, const struct quat *q0, mreal_t f) {
    return (struct quat *)quat_multiply_f((mreal_t *)result, (const mreal_t *)q0, f);
}

struct quat *psquat_divide(struct quat *result, const struct quat *q0, const struct quat *q1) {
    return (struct quat *)quat_divide((mreal_t *)result, (const mreal_t *)q0, (const mreal_t *)q1);
}

struct quat *psquat_divide_f(struct quat *result, const struct quat *q0, mreal_t f) {
    return (struct quat *)quat_divide_f((mreal_t *)result, (const mreal_t *)q0, f);
}

struct quat *psquat_negative(struct quat *result, const struct quat *q0) {
    return (struct quat *)quat_negative((mreal_t *)result, (const mreal_t *)q0);
}

struct quat *psquat_conjugate(struct quat *result, const struct quat *q0) {
    return (struct quat *)quat_conjugate((mreal_t *)result, (const mreal_t *)q0);
}

struct quat *psquat_inverse(struct quat *result, const struct quat *q0) {
    return (struct quat *)quat_inverse((mreal_t *)result, (const mreal_t *)q0);
}

struct quat *psquat_normalize(struct quat *result, const struct quat *q0) {
    return (struct quat *)quat_normalize((mreal_t *)result, (const mreal_t *)q0);
}

mreal_t psquat_dot(const struct quat *q0, const struct quat *q1) {
    return quat_dot((const mreal_t *)q0, (const mreal_t *)q1);
}

struct quat *psquat_power(struct quat *result, const struct quat *q0, mreal_t exponent) {
    return (struct quat *)quat_power((mreal_t *)result, (const mreal_t *)q0, exponent);
}

struct quat *psquat_from_axis_angle(struct quat *result, const struct vec3 *v0, mreal_t angle) {
    return (struct quat *)quat_from_axis_angle((mreal_t *)result, (const mreal_t *)v0, angle);
}

struct quat *psquat_from_vec3(struct quat *result, const struct vec3 *v0, struct vec3 *v1) {
    return (struct quat *)quat_from_vec3((mreal_t *)result, (const mreal_t *)v0, (const mreal_t *)v1);
}

struct quat *psquat_from_mat4(struct quat *result, const struct mat4 *m0) {
    return (struct quat *)quat_from_mat4((mreal_t *)result, (const mreal_t *)m0);
}

struct quat *psquat_lerp(struct quat *result, const struct quat *q0, const struct quat *q1, mreal_t f) {
    return (struct quat *)quat_lerp((mreal_t *)result, (const mreal_t *)q0, (const mreal_t *)q1, f);
}

struct quat *psquat_slerp(struct quat *result, const struct quat *q0, const struct quat *q1, mreal_t f) {
    return (struct quat *)quat_slerp((mreal_t *)result, (const mreal_t *)q0, (const mreal_t *)q1, f);
}

mreal_t psquat_length(const struct quat *q0) {
    return quat_length((const mreal_t *)q0);
}

mreal_t psquat_length_squared(const struct quat *q0) {
    return quat_length_squared((const mreal_t *)q0);
}

mreal_t psquat_angle(const struct quat *q0, const struct quat *q1) {
    return quat_angle((const mreal_t *)q0, (const mreal_t *)q1);
}

struct mat2 *psmat2(struct mat2 *result, mreal_t m11, mreal_t m12, mreal_t m21, mreal_t m22) {
    return (struct mat2 *)mat2((mreal_t *)result, m11, m12, m21, m22);
}

struct mat2 *psmat2_zero(struct mat2 *result) {
    return (struct mat2 *)mat2_zero((mreal_t *)result);
}

struct mat2 *psmat2_identity(struct mat2 *result) {
    return (struct mat2 *)mat2_identity((mreal_t *)result);
}

mreal_t psmat2_determinant(const struct mat2 *m0) {
    return mat2_determinant((const mreal_t *)m0);
}

struct mat2 *psmat2_assign(struct mat2 *result, const struct mat2 *m0) {
    return (struct mat2 *)mat2_assign((mreal_t *)result, (const mreal_t *)m0);
}

struct mat2 *psmat2_negative(struct mat2 *result, const struct mat2 *m0) {
    return (struct mat2 *)mat2_negative((mreal_t *)result, (const mreal_t *)m0);
}

struct mat2 *psmat2_transpose(struct mat2 *result, const struct mat2 *m0) {
    return (struct mat2 *)mat2_transpose((mreal_t *)result, (const mreal_t *)m0);
}

struct mat2 *psmat2_cofactor(struct mat2 *result, const struct mat2 *m0) {
    return (struct mat2 *)mat2_cofactor((mreal_t *)result, (const mreal_t *)m0);
}

struct mat2 *psmat2_adjugate(struct mat2 *result, const struct mat2 *m0) {
    return (struct mat2 *)mat2_adjugate((mreal_t *)result, (const mreal_t *)m0);
}

struct mat2 *psmat2_multiply(struct mat2 *result, const struct mat2 *m0, const struct mat2 *m1) {
    return (struct mat2 *)mat2_multiply((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)m1);
}

struct mat2 *psmat2_multiply_f(struct mat2 *result, const struct mat2 *m0, mreal_t f) {
    return (struct mat2 *)mat2_multiply_f((mreal_t *)result, (const mreal_t *)m0, f);
}

struct mat2 *psmat2_inverse(struct mat2 *result, const struct mat2 *m0) {
    return (struct mat2 *)mat2_inverse((mreal_t *)result, (const mreal_t *)m0);
}

struct mat2 *psmat2_scaling(struct mat2 *result, struct vec2 *v0) {
    return (struct mat2 *)mat2_scaling((mreal_t *)result, (const mreal_t *)v0);
}

struct mat2 *psmat2_scale(struct mat2 *result, const struct mat2 *m0, struct vec2 *v0) {
    return (struct mat2 *)mat2_scale((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)v0);
}

struct mat2 *psmat2_rotation_z(struct mat2 *result, mreal_t f) {
    return (struct mat2 *)mat2_rotation_z((mreal_t *)result, f);
}

struct mat2 *psmat2_lerp(struct mat2 *result, const struct mat2 *m0, const struct mat2 *m1, mreal_t f) {
    return (struct mat2 *)mat2_lerp((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)m1, f);
}

struct mat3 *psmat3(struct mat3 *result, mreal_t m11, mreal_t m12, mreal_t m13, mreal_t m21, mreal_t m22, mreal_t m23, mreal_t m31, mreal_t m32, mreal_t m33) {
    return (struct mat3 *)mat3((mreal_t *)result, m11, m12, m13, m21, m22, m23, m31, m32, m33);
}

struct mat3 *psmat3_zero(struct mat3 *result) {
    return (struct mat3 *)mat3_zero((mreal_t *)result);
}

struct mat3 *psmat3_identity(struct mat3 *result) {
    return (struct mat3 *)mat3_identity((mreal_t *)result);
}

mreal_t psmat3_determinant(const struct mat3 *m0) {
    return mat3_determinant((const mreal_t *)m0);
}

struct mat3 *psmat3_assign(struct mat3 *result, const struct mat3 *m0) {
    return (struct mat3 *)mat3_assign((mreal_t *)result, (const mreal_t *)m0);
}

struct mat3 *psmat3_negative(struct mat3 *result, const struct mat3 *m0) {
    return (struct mat3 *)mat3_negative((mreal_t *)result, (const mreal_t *)m0);
}

struct mat3 *psmat3_transpose(struct mat3 *result, const struct mat3 *m0) {
    return (struct mat3 *)mat3_transpose((mreal_t *)result, (const mreal_t *)m0);
}

struct mat3 *psmat3_cofactor(struct mat3 *result, const struct mat3 *m0) {
    return (struct mat3 *)mat3_cofactor((mreal_t *)result, (const mreal_t *)m0);
}

struct mat3 *psmat3_multiply(struct mat3 *result, const struct mat3 *m0, const struct mat3 *m1) {
    return (struct mat3 *)mat3_multiply((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)m1);
}

struct mat3 *psmat3_multiply_f(struct mat3 *result, const struct mat3 *m0, mreal_t f) {
    return (struct mat3 *)mat3_multiply_f((mreal_t *)result, (const mreal_t *)m0, f);
}

struct mat3 *psmat3_inverse(struct mat3 *result, const struct mat3 *m0) {
    return (struct mat3 *)mat3_inverse((mreal_t *)result, (const mreal_t *)m0);
}

struct mat3 *psmat3_scaling(struct mat3 *result, const struct vec3 *v0) {
    return (struct mat3 *)mat3_scaling((mreal_t *)result, (const mreal_t *)v0);
}

struct mat3 *psmat3_scale(struct mat3 *result, const struct mat3 *m0, const struct vec3 *v0) {
    return (struct mat3 *)mat3_scale((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)v0);
}

struct mat3 *psmat3_rotation_x(struct mat3 *result, mreal_t f) {
    return (struct mat3 *)mat3_rotation_x((mreal_t *)result, f);
}

struct mat3 *psmat3_rotation_y(struct mat3 *result, mreal_t f) {
    return (struct mat3 *)mat3_rotation_y((mreal_t *)result, f);
}

struct mat3 *psmat3_rotation_z(struct mat3 *result, mreal_t f) {
    return (struct mat3 *)mat3_rotation_z((mreal_t *)result, f);
}

struct mat3 *psmat3_rotation_axis(struct mat3 *result, const struct vec3 *v0, mreal_t f) {
    return (struct mat3 *)mat3_rotation_axis((mreal_t *)result, (const mreal_t *)v0, f);
}

struct mat3 *psmat3_rotation_quat(struct mat3 *result, const struct quat *q0) {
    return (struct mat3 *)mat3_rotation_quat((mreal_t *)result, (const mreal_t *)q0);
}

struct mat3 *psmat3_lerp(struct mat3 *result, const struct mat3 *m0, const struct mat3 *m1, mreal_t f) {
    return (struct mat3 *)mat3_lerp((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)m1, f);
}

struct mat4 *psmat4(struct mat4 *result, mreal_t m11, mreal_t m12, mreal_t m13, mreal_t m14, mreal_t m21, mreal_t m22, mreal_t m23, mreal_t m24, mreal_t m31, mreal_t m32, mreal_t m33, mreal_t m34, mreal_t m41, mreal_t m42, mreal_t m43, mreal_t m44) {
    return (struct mat4 *)mat4((mreal_t *)result, m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
}

struct mat4 *psmat4_zero(struct mat4 *result) {
    return (struct mat4 *)mat4_zero((mreal_t *)result);
}

struct mat4 *psmat4_identity(struct mat4 *result) {
    return (struct mat4 *)mat4_identity((mreal_t *)result);
}

mreal_t psmat4_determinant(const struct mat4 *m0) {
    return mat4_determinant((const mreal_t *)m0);
}

struct mat4 *psmat4_assign(struct mat4 *result, const struct mat4 *m0) {
    return (struct mat4 *)mat4_assign((mreal_t *)result, (const mreal_t *)m0);
}

struct mat4 *psmat4_negative(struct mat4 *result, const struct mat4 *m0) {
    return (struct mat4 *)mat4_negative((mreal_t *)result, (const mreal_t *)m0);
}

struct mat4 *psmat4_transpose(struct mat4 *result, const struct mat4 *m0) {
    return (struct mat4 *)mat4_transpose((mreal_t *)result, (const mreal_t *)m0);
}

struct mat4 *psmat4_cofactor(struct mat4 *result, const struct mat4 *m0) {
    return (struct mat4 *)mat4_cofactor((mreal_t *)result, (const mreal_t *)m0);
}

struct mat4 *psmat4_rotation_x(struct mat4 *result, mreal_t f) {
    return (struct mat4 *)mat4_rotation_x((mreal_t *)result, f);
}

struct mat4 *psmat4_rotation_y(struct mat4 *result, mreal_t f) {
    return (struct mat4 *)mat4_rotation_y((mreal_t *)result, f);
}

struct mat4 *psmat4_rotation_z(struct mat4 *result, mreal_t f) {
    return (struct mat4 *)mat4_rotation_z((mreal_t *)result, f);
}

struct mat4 *psmat4_rotation_axis(struct mat4 *result, const struct vec3 *v0, mreal_t f) {
    return (struct mat4 *)mat4_rotation_axis((mreal_t *)result, (const mreal_t *)v0, f);
}

struct mat4 *psmat4_rotation_quat(struct mat4 *result, const struct quat *q0) {
    return (struct mat4 *)mat4_rotation_quat((mreal_t *)result, (const mreal_t *)q0);
}

struct mat4 *psmat4_translation(struct mat4 *result, const struct mat4 *m0, const struct vec3 *v0) {
    return (struct mat4 *)mat4_translation((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)v0);
}

struct mat4 *psmat4_translate(struct mat4 *result, const struct mat4 *m0, const struct vec3 *v0) {
    return (struct mat4 *)mat4_translate((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)v0);
}

struct mat4 *psmat4_scaling(struct mat4 *result, const struct mat4 *m0, const struct vec3 *v0) {
    return (struct mat4 *)mat4_scaling((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)v0);
}

struct mat4 *psmat4_scale(struct mat4 *result, const struct mat4 *m0, const struct vec3 *v0) {
    return (struct mat4 *)mat4_scale((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)v0);
}

struct mat4 *psmat4_multiply(struct mat4 *result, const struct mat4 *m0, const struct mat4 *m1) {
    return (struct mat4 *)mat4_multiply((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)m1);
}

struct mat4 *psmat4_multiply_f(struct mat4 *result, const struct mat4 *m0, mreal_t f) {
    return (struct mat4 *)mat4_multiply_f((mreal_t *)result, (const mreal_t *)m0, f);
}

struct mat4 *psmat4_inverse(struct mat4 *result, const struct mat4 *m0) {
    return (struct mat4 *)mat4_inverse((mreal_t *)result, (const mreal_t *)m0);
}

struct mat4 *psmat4_lerp(struct mat4 *result, const struct mat4 *m0, const struct mat4 *m1, mreal_t f) {
    return (struct mat4 *)mat4_lerp((mreal_t *)result, (const mreal_t *)m0, (const mreal_t *)m1, f);
}

struct mat4 *psmat4_look_at(struct mat4 *result,const struct vec3 *position,const struct vec3 *target,const struct vec3 *up) {
    return (struct mat4 *)mat4_look_at((mreal_t *)result, (const mreal_t *)position, (const mreal_t *)target, (const mreal_t *)up);
}

struct mat4 *psmat4_ortho(struct mat4 *result, mreal_t l, mreal_t r, mreal_t b, mreal_t t, mreal_t n, mreal_t f) {
    return (struct mat4 *)mat4_ortho((mreal_t *)result, l, r, b, t, n, f);
}

struct mat4 *psmat4_perspective(struct mat4 *result, mreal_t fov_y, mreal_t aspect, mreal_t n, mreal_t f) {
    return (struct mat4 *)mat4_perspective((mreal_t *)result, fov_y, aspect, n, f);
}

struct mat4 *psmat4_perspective_fov(struct mat4 *result, mreal_t fov, mreal_t w, mreal_t h, mreal_t n, mreal_t f) {
    return (struct mat4 *)mat4_perspective_fov((mreal_t *)result, fov, w, h, n, f);
}

struct mat4 *psmat4_perspective_infinite(struct mat4 *result, mreal_t fov_y, mreal_t aspect, mreal_t n) {
    return (struct mat4 *)mat4_perspective_infinite((mreal_t *)result, fov_y, aspect, n);
}

mreal_t quadratic_ease_out(mreal_t f) {
    return -f * (f - MREAL_C(2.0));
}

mreal_t quadratic_ease_in(mreal_t f) {
    return f * f;
}

mreal_t quadratic_ease_in_out(mreal_t f) {
    mreal_t a = MREAL_C(0.0);
    if (f < MREAL_C(0.5)) {
        a = MREAL_C(2.0) * f * f;
    } else {
        a = -MREAL_C(2.0) * f * f + MREAL_C(4.0) * f - MREAL_C(1.0);
    }
    return a;
}

mreal_t cubic_ease_out(mreal_t f) {
    mreal_t a = f - MREAL_C(1.0);
    return a * a * a + MREAL_C(1.0);
}

mreal_t cubic_ease_in(mreal_t f) {
    return f * f * f;
}

mreal_t cubic_ease_in_out(mreal_t f) {
    mreal_t a = MREAL_C(0.0);
    if (f < MREAL_C(0.5)) {
        a = MREAL_C(4.0) * f * f * f;
    } else {
        a = MREAL_C(2.0) * f - MREAL_C(2.0);
        a = MREAL_C(0.5) * a * a * a + MREAL_C(1.0);
    }
    return a;
}

mreal_t quartic_ease_out(mreal_t f) {
    mreal_t a = f - MREAL_C(1.0);
    return a * a * a * (MREAL_C(1.0) - f) + MREAL_C(1.0);
}

mreal_t quartic_ease_in(mreal_t f) {
    return f * f * f * f;
}

mreal_t quartic_ease_in_out(mreal_t f) {
    mreal_t a = MREAL_C(0.0);
    if (f < MREAL_C(0.5)) {
        a = MREAL_C(8.0) * f * f * f * f;
    } else {
        a = f - MREAL_C(1.0);
        a = -MREAL_C(8.0) * a * a * a * a + MREAL_C(1.0);
    }
    return a;
}

mreal_t quintic_ease_out(mreal_t f) {
    mreal_t a = f - MREAL_C(1.0);
    return a * a * a * a * a + MREAL_C(1.0);
}

mreal_t quintic_ease_in(mreal_t f) {
    return f * f * f * f * f;
}

mreal_t quintic_ease_in_out(mreal_t f) {
    mreal_t a = MREAL_C(0.0);
    if (f < MREAL_C(0.5)) {
        a = MREAL_C(16.0) * f * f * f * f * f;
    } else {
        a = MREAL_C(2.0) * f - MREAL_C(2.0);
        a = MREAL_C(0.5) * a * a * a * a * a + MREAL_C(1.0);
    }
    return a;
}

mreal_t sine_ease_out(mreal_t f) {
    return MSIN(f * MPI_2);
}

mreal_t sine_ease_in(mreal_t f) {
    return MSIN((f - MREAL_C(1.0)) * MPI_2) + MREAL_C(1.0);
}

mreal_t sine_ease_in_out(mreal_t f) {
#if defined(CGEOM_PRECISE_TRIGONOMETRY)
    sincos1cos sc1c = sncs1cs(f * MPI);
    return MREAL_C(0.5) * sc1c.omc;
#else
    return MREAL_C(0.5) * (MREAL_C(1.0) - MCOS(f * MPI));
#endif
}

mreal_t circular_ease_out(mreal_t f) {
    return MSQRT((MREAL_C(2.0) - f) * f);
}

mreal_t circular_ease_in(mreal_t f) {
    return MREAL_C(1.0) - MSQRT(MREAL_C(1.0) - (f * f));
}

mreal_t circular_ease_in_out(mreal_t f) {
    mreal_t a = MREAL_C(0.0);
    if (f < MREAL_C(0.5)) {
        a = MREAL_C(0.5) * (MREAL_C(1.0) - MSQRT(MREAL_C(1.0) - MREAL_C(4.0) * f * f));
    } else {
        a = MREAL_C(0.5) * (MSQRT(-(MREAL_C(2.0) * f - MREAL_C(3.0)) * (MREAL_C(2.0) * f - MREAL_C(1.0))) + MREAL_C(1.0));
    }
    return a;
}

mreal_t exponential_ease_out(mreal_t f) {
    mreal_t a = f;
    if (MFABS(a) > MREAL_EPSILON) {
        a = MREAL_C(1.0) - MPOW(MREAL_C(2.0), -MREAL_C(10.0) * f);
    }
    return a;
}

mreal_t exponential_ease_in(mreal_t f) {
    mreal_t a = f;
    if (MFABS(a) > MREAL_EPSILON) {
        a = MPOW(MREAL_C(2.0), MREAL_C(10.0) * (f - MREAL_C(1.0)));
    }
    return a;
}

mreal_t exponential_ease_in_out(mreal_t f) {
    mreal_t a = f;
    if (f < MREAL_C(0.5)) {
        a = MREAL_C(0.5) * MPOW(MREAL_C(2.0), (MREAL_C(20.0) * f) - MREAL_C(10.0));
    } else {
        a = -MREAL_C(0.5) * MPOW(MREAL_C(2.0), -MREAL_C(20.0) * f + MREAL_C(10.0)) + MREAL_C(1.0);
    }
    return a;
}

mreal_t elastic_ease_out(mreal_t f) {
    return MSIN(-MREAL_C(13.0) * MPI_2 * (f + MREAL_C(1.0))) * MPOW(MREAL_C(2.0), -MREAL_C(10.0) * f) + MREAL_C(1.0);
}

mreal_t elastic_ease_in(mreal_t f) {
    return MSIN(MREAL_C(13.0) * MPI_2 * f) * MPOW(MREAL_C(2.0), MREAL_C(10.0) * (f - MREAL_C(1.0)));
}

mreal_t elastic_ease_in_out(mreal_t f) {
    mreal_t a = MREAL_C(0.0);
    if (f < MREAL_C(0.5)) {
        a = MREAL_C(0.5) * MSIN(MREAL_C(13.0) * MPI_2 * (MREAL_C(2.0) * f)) * MPOW(MREAL_C(2.0), MREAL_C(10.0) * ((MREAL_C(2.0) * f) - MREAL_C(1.0)));
    } else {
        a = MREAL_C(0.5) * (MSIN(-MREAL_C(13.0) * MPI_2 * ((MREAL_C(2.0) * f - MREAL_C(1.0)) + MREAL_C(1.0))) * MPOW(MREAL_C(2.0), -MREAL_C(10.0) * (MREAL_C(2.0) * f - MREAL_C(1.0))) + MREAL_C(2.0));
    }
    return a;
}

mreal_t back_ease_out(mreal_t f) {
    mreal_t a = MREAL_C(1.0) - f;
    return MREAL_C(1.0) - (a * a * a - a * MSIN(a * MPI));
}

mreal_t back_ease_in(mreal_t f) {
    return f * f * f - f * MSIN(f * MPI);
}

mreal_t back_ease_in_out(mreal_t f) {
    mreal_t a = MREAL_C(0.0);
    if (f < MREAL_C(0.5)) {
        a = MREAL_C(2.0) * f;
        a = MREAL_C(0.5) * (a * a * a - a * MSIN(a * MPI));
    } else {
        a = (MREAL_C(1.0) - (MREAL_C(2.0) * f - MREAL_C(1.0)));
        a = MREAL_C(0.5) * (MREAL_C(1.0) - (a * a * a - a * MSIN(f * MPI))) + MREAL_C(0.5);
    }
    return a;
}

mreal_t bounce_ease_out(mreal_t f) {
    mreal_t a = MREAL_C(0.0);
    if (f < MREAL_C(4.0) / MREAL_C(11.0)) {
        a = (MREAL_C(121.0) * f * f) / MREAL_C(16.0);
    } else if (f < MREAL_C(8.0) / MREAL_C(11.0)) {
        a = (MREAL_C(363.0) / MREAL_C(40.0) * f * f) - (MREAL_C(99.0) / MREAL_C(10.0) * f) + MREAL_C(17.0) / MREAL_C(5.0);
    } else if (f < MREAL_C(9.0) / MREAL_C(10.0)) {
        a = (MREAL_C(4356.0) / MREAL_C(361.0) * f * f) - (MREAL_C(35442.0) / MREAL_C(1805.0) * f) + MREAL_C(16061.0) / MREAL_C(1805.0);
    } else {
        a = (MREAL_C(54.0) / MREAL_C(5.0) * f * f) - (MREAL_C(513.0) / MREAL_C(25.0) * f) + MREAL_C(268.0) / MREAL_C(25.0);
    }
    return a;
}

mreal_t bounce_ease_in(mreal_t f) {
    return MREAL_C(1.0) - bounce_ease_out(MREAL_C(1.0) - f);
}

mreal_t bounce_ease_in_out(mreal_t f) {
    mreal_t a = MREAL_C(0.0);
    if (f < MREAL_C(0.5)) {
        a = MREAL_C(0.5) * bounce_ease_in(f * MREAL_C(2.0));
    } else {
        a = MREAL_C(0.5) * bounce_ease_out(f * MREAL_C(2.0) - MREAL_C(1.0)) + MREAL_C(0.5);
    }
    return a;
}
