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

#define FLOAT_VEC_SIZE 4
#define INT32_VEC_SIZE 4
#define INT8_VEC_SIZE  16

namespace powerpc {

float
fvec_L1_ref_ppc(const float* x, const float* y, size_t d) {
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

    vector float *vx, *vy;
    vector float vtmp = {0, 0, 0, 0};
    vector float vres = {0, 0, 0, 0};

    base = (d / FLOAT_VEC_SIZE) * FLOAT_VEC_SIZE;

    for (size_t i = 0; i < base; i = i + FLOAT_VEC_SIZE) {
        vx = (vector float *)(&x[i]);
        vy = (vector float *)(&y[i]);

        vres += vec_abs(vx[0] - vy[0]);
    }

    /* Handle any remaining data elements */
    for (i = base; i < d; i++) {
        const float tmp = x[i] - y[i];
        res += std::fabs(tmp);
    }

    return res + vres[0] + vres[1] + vres[2] + vres[3];
}

}  // namespace powerpc 

#endif
