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
#include "manhattan_l1_distance.h"
#include <cmath>

namespace powerpc {

float
fvec_L1_ref_ippc (const float* x, const float* y, size_t d) {
    size_t i;
    float res = 0;
    /* PowerPC, vectorize the function using PowerPC GCC built-in calls.
       Original code:

       for (i = 0; i < d; i++) {
           const float tmp = x[i] - y[i];
           res += std::fabs(tmp);
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
        vx = vec_xl ((long)(i*sizeof(float)), (float *) x);
        vy = vec_xl ((long)(i*sizeof(float)), (float *) y);

        vtmp = vec_sub(vx, vy);
        vtmp = vec_abs(vtmp);
        vres = vec_add(vtmp, vres);
    }

    /* Handle any remaining data elements */
    for (i = base; i < d; i++) {
        const float tmp = x[i] - y[i];
        res += std::fabs(tmp);
    }

    return res + vres[0] + vres[1] + vres[2] + vres[3];
}

float
fvec_Linf_ref_ippc (const float* x, const float* y, size_t d) {
    size_t i;
    float res = 0;
    /* PowerPC, vectorize the function using PowerPC GCC built-in calls.
       Original code:

       for (i = 0; i < d; i++) {
         res = std::fmax(res, std::fabs(x[i] - y[i]));
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
        vx = vec_xl ((long)(i*sizeof(float)), (float *) x);
        vy = vec_xl ((long)(i*sizeof(float)), (float *) y);

        vtmp = vec_sub(vx, vy);
        vtmp = vec_abs(vtmp);
        res = std::fmax(res, vtmp[0]);
        res = std::fmax(res, vtmp[1]);
        res = std::fmax(res, vtmp[2]);
        res = std::fmax(res, vtmp[3]);
    }

    /* Handle any remaining data elements */
    for (i = base; i < d; i++) {
        res = std::fmax(res, std::fabs(x[i] - y[i]));
    }

    return res;
}

void
fvec_madd_ref_ippc (size_t n, const float* a, float bf, const float* b,
                    float* c) {
    /* PowerPC, vectorize the function using PowerPC GCC built-in calls.
       Original code:

       for (size_t i = 0; i < n; i++) {
           c[i] = a[i] + bf * b[i];
       }
   */
    /* Vector implmentaion uses vector size of FLOAT_VEC_SIZE.  If the input
       array size is not a power of FLOAT_VEC_SIZE, do the remaining elements
       in scalar mode.  */
    size_t base;
    vector float va, vb, vc, vbf = {bf, bf, bf, bf};

    base = (n / FLOAT_VEC_SIZE) * FLOAT_VEC_SIZE;

    for (size_t i = 0; i < base; i = i + FLOAT_VEC_SIZE) {
        va = vec_xl ((long)(i*sizeof(float)), (float *) a);
        vb = vec_xl ((long)(i*sizeof(float)), (float *) b);

        vc = vec_madd (vb, vbf, va);
        vec_xst (vc, (long)(i*sizeof(float)), c);
    }

    /* Handle any remaining data elements */
    for (size_t i = base; i < n; i++) {
        c[i] = a[i] + bf * b[i];
    }
}

int
fvec_madd_and_argmin_ref_ippc (size_t n, const float* a, float bf,
                               const float* b, float* c) {
    /* PowerPC, vectorize the function using PowerPC GCC built-in calls.
       Original code:

       float vmin = 1e20;
       int imin = -1;

       for (size_t i = 0; i < n; i++) {
           c[i] = a[i] + bf * b[i];
           if (c[i] < vmin) {
               vmin = c[i];
               imin = i;
            }
       }
       return imin;
    */
    /* Vector implmentaion uses vector size of FLOAT_VEC_SIZE.  If the input
       array size is not a power of FLOAT_VEC_SIZE, do the remaining elements
       in scalar mode.  */
    vector float va, vb, vc;
    vector float vbf = {bf, bf, bf, bf};
    float vmin = 1.0e20;
    int imin = -1;
    size_t base;

    base = (n / FLOAT_VEC_SIZE) * FLOAT_VEC_SIZE;

    for (size_t i = 0; i < base; i = i + FLOAT_VEC_SIZE) {
        va = vec_xl ((long)(i*sizeof(float)), (float *) a);
        vb = vec_xl ((long)(i*sizeof(float)), (float *) b);

	vc = vec_madd(vbf, vb, va);

        /* Checke each vector element */
        for (int j = 0; j < FLOAT_VEC_SIZE; j++) {
            if (vc[j] < vmin) {
                vmin = c[i + j];
                imin = i + j;
             }
        }
    }

    /* Handle any remaining data elements */
    for (size_t i = base; i < n; i++) {
        if (c[i] < vmin) {
            vmin = c[i];
            imin = i;
        }
    }
    return imin;
}

}  // namespace powerpc 

#endif
