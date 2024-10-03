/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#if defined(__powerpc__)

#include <altivec.h>   /* Required for the Power GCC built-ins  */

#include "euclidean_l2_distance.h"

#include <cmath>

namespace base {
float
fvec_L2sqr_ref(const float* x, const float* y, size_t d) {
    size_t i;
    float res = 0;
    for (i = 0; i < d; i++) {
        const float tmp = x[i] - y[i];
        res += tmp * tmp;
    }
    return res;
}

float
fvec_norm_L2sqr_ref(const float* x, size_t d) {
    size_t i;
    double res = 0;
    for (i = 0; i < d; i++) {
        res += x[i] * x[i];
    }
    return res;
}

/// compute ny square L2 distance between x and a set of transposed contiguous
/// y vectors. squared lengths of y should be provided as well
void
fvec_L2sqr_ny_transposed_ref(float* __restrict dis, const float* __restrict x,
                             const float* __restrict y,
                             const float* __restrict y_sqlen, size_t d,
                             size_t d_offset, size_t ny) {
    float x_sqlen = 0;
    for (size_t j = 0; j < d; j++) {
        x_sqlen += x[j] * x[j];
    }

    for (size_t i = 0; i < ny; i++) {
        float dp = 0;
        for (size_t j = 0; j < d; j++) {
            dp += x[j] * y[i + j * d_offset];
        }

        dis[i] = x_sqlen + y_sqlen[i] - 2 * dp;
    }
}

void
fvec_L2sqr_batch_4_ref(const float* x, const float* y0, const float* y1,
                       const float* y2, const float* y3, const size_t d,
                       float& dis0, float& dis1, float& dis2, float& dis3) {
    float d0 = 0;
    float d1 = 0;
    float d2 = 0;
    float d3 = 0;
    for (size_t i = 0; i < d; ++i) {
        const float q0 = x[i] - y0[i];
        const float q1 = x[i] - y1[i];
        const float q2 = x[i] - y2[i];
        const float q3 = x[i] - y3[i];
        d0 += q0 * q0;
        d1 += q1 * q1;
        d2 += q2 * q2;
        d3 += q3 * q3;
    }

    dis0 = d0;
    dis1 = d1;
    dis2 = d2;
    dis3 = d3;
}

int32_t
ivec_L2sqr_ref(const int8_t* x, const int8_t* y, size_t d) {
    size_t i;
    int32_t res = 0;
    for (i = 0; i < d; i++) {
        const int32_t tmp = (int32_t)x[i] - (int32_t)y[i];
        res += tmp * tmp;
    }
    return res;
}

}

#endif

