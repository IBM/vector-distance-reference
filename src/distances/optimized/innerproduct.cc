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

#include "innerproduct.h"

#include <cmath>

#define FLOAT_VEC_SIZE 4
#define INT32_VEC_SIZE 4
#define INT8_VEC_SIZE  16

namespace powerpc {

float
fvec_inner_product_ref_ppc(const float* x, const float* y, size_t d) {
    size_t i;
    float res = 0;
    /* PowerPC, vectorize the function using PowerPC GCC built-in calls.
       Original code:

       for (i = 0; i < d; i++) {
           res += x[i] * y[i];
       }
       return res;
    */
    /* Vector implmentaion uses vector size of FLOAT_VEC_SIZE.  If the input
       array size is not a power of FLOAT_VEC_SIZE, do the remaining elements
       in scalar mode.  */
    size_t base;

    vector float *vx, *vy;
    vector float vres = {0, 0, 0, 0};

    base = (d / FLOAT_VEC_SIZE) * FLOAT_VEC_SIZE;

    for (size_t i = 0; i < base; i = i + FLOAT_VEC_SIZE) {
        vx = (vector float *)(&x[i]);
        vy = (vector float *)(&y[i]);

        vres += vx[0] * vy[0];
    }

    /* Handle any remaining data elements */
    for (i = base; i < d; i++) {
        res += x[i] * y[i];
    }
    return res + vres[0] + vres[1] + vres[2] + vres[3];
}

void
fvec_inner_product_batch_4_ref_ppc(const float* __restrict x,
                                   const float* __restrict y0,
                                   const float* __restrict y1,
                                   const float* __restrict y2,
                                   const float* __restrict y3,
                                   const size_t d, float& dis0, float& dis1,
                                   float& dis2, float& dis3) {
    /* PowerPC, vectorize the function using PowerPC GCC built-in calls.
       Original code:

       float d0 = 0;
       float d1 = 0;
       float d2 = 0;
       float d3 = 0;
       for (size_t i = 0; i < d; ++i) {
           d0 += x[i] * y0[i];
           d1 += x[i] * y1[i];
           d2 += x[i] * y2[i];
           d3 += x[i] * y3[i];
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
    vector float *vx, *vy0, *vy1, *vy2, *vy3;
    vector float vd0 = {0.0, 0.0, 0.0, 0.0};
    vector float vd1 = {0.0, 0.0, 0.0, 0.0};
    vector float vd2 = {0.0, 0.0, 0.0, 0.0};
    vector float vd3 = {0.0, 0.0, 0.0, 0.0};

    base = (d / FLOAT_VEC_SIZE) * FLOAT_VEC_SIZE;
    remainder = d % FLOAT_VEC_SIZE;

    for (size_t i = 0; i < base; i = i + FLOAT_VEC_SIZE) {
        vx = (vector float *)(&x[i]);
        vy0 = (vector float *)(&y0[i]);
        vy1 = (vector float *)(&y1[i]);
        vy2 = (vector float *)(&y2[i]);
        vy3 = (vector float *)(&y3[i]);

        vd0 += vx[0] * vy0[0];
        vd1 += vx[0] * vy1[0];
        vd2 += vx[0] * vy2[0];
        vd3 += vx[0] * vy3[0];
    }

    dis0 = vd0[0] + vd0[1] + vd0[2] + vd0[3];
    dis1 = vd1[0] + vd1[1] + vd1[2] + vd1[3];
    dis2 = vd2[0] + vd2[1] + vd2[2] + vd2[3];
    dis3 = vd3[0] + vd3[1] + vd3[2] + vd3[3];

    /* Handle any remaining data elements */
    if (remainder != 0) {
        float d0 = 0;
        float d1 = 0;
        float d2 = 0;
        float d3 = 0;

        for (size_t i = base; i < d; i++) {
            d0 += x[i] * y0[i];
            d1 += x[i] * y1[i];
            d2 += x[i] * y2[i];
            d3 += x[i] * y3[i];
        }

        dis0 += d0;
        dis1 += d1;
        dis2 += d2;
        dis3 += d3;
    }
}

int32_t
ivec_inner_product_ref_ppc(const int8_t* x, const int8_t* y, size_t d) {
    size_t i;
    int32_t res = 0;

    /* Attempts to mannually vectorize and manually unroll the loop
       do not seem to improve the performance. */
    for (i = 0; i < d; i++) {
        res += (int32_t)x[i] * y[i];
    }
    return res;
}

} // namespace powerpc 

#endif
