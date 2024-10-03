/**
 * © Copyright IBM Corporation 2024. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if defined(__powerpc__)

#include <altivec.h>   /* Required for the Power GCC built-ins  */

#include "euclidean_l2_distance.h"

#include <cmath>

namespace powerpc {

float
fvec_L2sqr_ref_ippc (const float* x, const float* y, size_t d) {
    size_t i;

    float res = 0;
    /* PowerPC, vectorize the function using PowerPC GCC built-in calls.
       Original code:

       for (i = 0; i < d; i++) {
           const float tmp = x[i] - y[i];
           res += tmp * tmp;
       }
       return res;
    */

    /* Vector implmentaion uses vector size of FLOAT_VEC_SIZE.  If the input
       array size is not a power of FLOAT_VEC_SIZE, do the remaining elements
       in scalar mode.  */
    size_t base;

    vector float vx, vy;
    vector float vtmp = {0, 0, 0, 0};
    vector float vres = {0, 0, 0, 0};

    base = (d / FLOAT_VEC_SIZE) * FLOAT_VEC_SIZE;

    for (size_t i = 0; i < base; i = i + FLOAT_VEC_SIZE) {
        /* Load up the data vectors */
        vx = vec_xl ((long)(i*sizeof(float)), (float *)x);
        vy = vec_xl ((long)(i*sizeof(float)), (float *)y);

        vtmp = vec_sub(vx, vy);
        vres = vec_madd(vtmp, vtmp, vres);
    }

    /* Handle any remaining data elements */
    for (i = base; i < d; i++) {
        const float tmp = x[i] - y[i];
        res += tmp * tmp;
    }

    return res + vres[0] + vres[1] + vres[2] + vres[3];
}

float
fvec_norm_L2sqr_ref_ippc (const float* x, size_t d) {
    size_t i;
    double res = 0;
    /* PowerPC, vectorize the function using PowerPC GCC built-in calls.
       Note, the original code calculated res as a double precision value,
       than returned the result as a float.
       Original code:

       for (i = 0; i < d; i++) {
           res += x[i] * x[i];
       }
       return res;
    */
    /* Vector implmentaion uses vector size of FLOAT_VEC_SIZE.  Do the
       operation as double, then return result as a float.  If the input array
       size is not a power of FLOAT_VEC_SIZE, do the remaining elements in
       scalar mode.  */
    size_t base;

    vector float vx;
    vector double vxde, vxdo;
    vector double vtmpo = {0, 0}, vtmpe = {0, 0};
    vector double vreso = {0, 0}, vrese = {0, 0};

    base = (d / FLOAT_VEC_SIZE) * FLOAT_VEC_SIZE;

    for (size_t i = 0; i < base; i = i + FLOAT_VEC_SIZE) {
        vx = vec_xl ((long)(i*sizeof(float)), (float *)x);

        /* Convert even/odd floats to double then square elements. */
        vxdo = vec_doubleo (vx);
        vtmpo = vec_mul (vxdo, vxdo);
        vreso = vec_add (vreso, vtmpo);
        vxde = vec_doublee (vx);
        vtmpe = vec_mul (vxde, vxde);
        vrese = vec_add (vrese, vtmpe);
    }

    /* Handle any remaining data elements */
    for (i = base; i < d; i++) {
        res += x[i] * x[i];
    }
    return res + vreso[0] + vreso[1] + vrese[0] + vrese[1];
}

/// compute ny square L2 distance between x and a set of transposed contiguous
/// y vectors. squared lengths of y should be provided as well
void
fvec_L2sqr_ny_transposed_ref_ippc (float* __restrict dis,
                                   const float* __restrict x,
                                   const float* __restrict y,
                                   const float* __restrict y_sqlen,
                                   size_t d, size_t d_offset, size_t ny) {
    /* PowerPC, vectorize the function using PowerPC GCC built-in calls.
       Original code:

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
    */
    /* Vector implmentaion uses vector size of FLOAT_VEC_SIZE.  If the input
       array size is not a power of FLOAT_VEC_SIZE, do the remaining elements
       in scalar mode.  */
    size_t base;

    float x_sqlen = 0;
    vector float vx, vy, vy_sqlen, vdp = {0, 0, 0, 0};
    vector float vx_sqlen = {0, 0, 0, 0};
    vector float vres = {0, 0, 0, 0};
    vector float vzero = {0, 0, 0, 0};

    base = (d / FLOAT_VEC_SIZE) * FLOAT_VEC_SIZE;

    for (size_t i = 0; i < base; i = i + FLOAT_VEC_SIZE) {
        vx = vec_xl ((long)(i*sizeof(float)), (float *) x);
        vx_sqlen = vec_madd(vx, vx, vx_sqlen);
    }

    x_sqlen = vx_sqlen[0] + vx_sqlen[1] + vx_sqlen[2] + vx_sqlen[3];

    /* Handle any remaining x data elements, in scalar mode. */
    for (size_t j = base; j < d; j++) {
        x_sqlen += x[j + base] * x[j + base];
    }

    for (size_t i = 0; i < ny; i++ ) {
        float dp = 0;
        vdp = vzero;

        /* Unrolling gives better performance then trying to vectorize.  */
        base = (d / 16) * 16;
        for (size_t j = 0; j < base; j = j + 16) {
            dp += x[j] * y[i + j * d_offset];
            dp += x[j + 1] * y[i + (j + 1) * d_offset];
            dp += x[j + 2] * y[i + (j + 2) * d_offset];
            dp += x[j + 3] * y[i + (j + 3) * d_offset];
            dp += x[j + 4] * y[i + (j + 4) * d_offset];
            dp += x[j + 5] * y[i + (j + 5) * d_offset];
            dp += x[j + 6] * y[i + (j + 6) * d_offset];
            dp += x[j + 7] * y[i + (j + 7) * d_offset];
            dp += x[j + 8] * y[i + (j + 8) * d_offset];
            dp += x[j + 9] * y[i + (j + 9) * d_offset];
            dp += x[j + 10] * y[i + (j + 10) * d_offset];
            dp += x[j + 11] * y[i + (j + 11) * d_offset];
            dp += x[j + 12] * y[i + (j + 12) * d_offset];
            dp += x[j + 13] * y[i + (j + 13) * d_offset];
            dp += x[j + 14] * y[i + (j + 14) * d_offset];
            dp += x[j + 15] * y[i + (j + 15) * d_offset];
        }

      for (size_t j = base; j < d; j++) {
	  dp += x[j] * y[i + j * d_offset];
      }

      dis[i] = x_sqlen +  y_sqlen[i] - 2 * dp;
    }
}

void
fvec_L2sqr_batch_4_ref_ippc (const float* x, const float* y0, const float* y1,
                             const float* y2, const float* y3, const size_t d,
                             float& dis0, float& dis1, float& dis2,
                             float& dis3) {

    /* PowerPC, vectorize the function using PowerPC GCC built-in calls.
      Original code:

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
    */
    /* Vector implmentaion uses vector size of FLOAT_VEC_SIZE.  If the input
       array size is not a power of FLOAT_VEC_SIZE, do the remaining elements
       in scalar mode.  */
    size_t base, remainder;

    vector float vx, vy0, vy1, vy2, vy3;
    vector float vd0 = {0, 0, 0, 0};
    vector float vd1 = {0, 0, 0, 0};
    vector float vd2 = {0, 0, 0, 0};
    vector float vd3 = {0, 0, 0, 0};
    vector float vq0, vq1, vq2, vq3;
    float d0 = 0;
    float d1 = 0;
    float d2 = 0;
    float d3 = 0;

    base = (d / FLOAT_VEC_SIZE) * FLOAT_VEC_SIZE;
    remainder = d % FLOAT_VEC_SIZE;

    for (size_t i = 0; i < base; i = i + FLOAT_VEC_SIZE) {
        /* Load up the data vectors */
        vx = vec_xl ((long)(i*sizeof(float)), (float *) x);
        vy0 = vec_xl ((long)(i*sizeof(float)), (float *) y0);
        vy1 = vec_xl ((long)(i*sizeof(float)), (float *) y1);
        vy2 = vec_xl ((long)(i*sizeof(float)), (float *) y2);
        vy3 = vec_xl ((long)(i*sizeof(float)), (float *) y3);

        /* Replace scalar subtract with vector subtract built-in.  */
        vq0 = vec_sub(vx, vy0);
	vq1 = vec_sub(vx, vy1);
	vq2 = vec_sub(vx, vy2);
	vq3 = vec_sub(vx, vy3);

	/* Replace scalar multiply add with vector multiply add built-in.  */
	vd0 = vec_madd (vq0, vq0, vd0);
	vd1 = vec_madd (vq1, vq1, vd1);
	vd2 = vec_madd (vq2, vq2, vd2);
	vd3 = vec_madd (vq3, vq3, vd3);
    }

    /* Handle the remainder of the elments in scalar mode.  */
    for (size_t i = base; i < d ; ++i) {
        const float q0 = x[i] - y0[i];
        const float q1 = x[i] - y1[i];
        const float q2 = x[i] - y2[i];
        const float q3 = x[i] - y3[i];

        d0 += q0 * q0;
        d1 += q1 * q1;
        d2 += q2 * q2;
        d3 += q3 * q3;
    }

    /* Replace result assignment of the scalar result with sum of the
       corresponding vector elements to get the equivalent result.  */
    dis0 = vd0[0] + vd0[1] + vd0[2] + vd0[3] + d0;
    dis1 = vd1[0] + vd1[1] + vd1[2] + vd1[3] + d1;
    dis2 = vd2[0] + vd2[1] + vd2[2] + vd2[3] + d2;
    dis3 = vd3[0] + vd3[1] + vd3[2] + vd3[3] + d3;
}

int32_t
ivec_L2sqr_ref_ippc(const int8_t* x, const int8_t* y, size_t d) {
    size_t i;
    int32_t res = 0;

    /* Attempts to mannually vectorize and manually unroll the loop
       do not seem to improve the performance. */
    for (i = 0; i < d; i++) {
        const int32_t tmp = (int32_t)x[i] - (int32_t)y[i];
	res += tmp * tmp;
    }
    return res;
}

} // namespace powerpc

#endif
