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

#include "jaccard_distance.h"

#include <cmath>

namespace powerpc {

float jaccard_distance_ippc (const float* x, const float* y, size_t d)
{
    float accu_num = 0.0, accu_den = 0.0;
    size_t base,i;
    vector float vx, vy;
    vector float vaccu_num = {0.0, 0.0, 0.0, 0.0};
    vector float vaccu_den = {0.0, 0.0, 0.0, 0.0};

    base = (d / FLOAT_VEC_SIZE) * FLOAT_VEC_SIZE;
    for (size_t i = 0; i < base; i = i + FLOAT_VEC_SIZE) 
        {
            vx = vec_xl ((long)(i*sizeof(float)), (float*) x);
            vy = vec_xl ((long)(i*sizeof(float)), (float*) y);
            vaccu_num = vec_add(vaccu_num, vec_min(vx, vy));
            vaccu_den = vec_add(vaccu_den, vec_max(vx, vy));
        }            
    accu_num = vaccu_num[0] + vaccu_num[1] + vaccu_num[2] + vaccu_num[3];
    accu_den = vaccu_den[0] + vaccu_den[1] + vaccu_den[2] + vaccu_den[3];
    /* Handle any remaining data elements */
    for (i = base; i < d; i++) 
    {
        accu_num += fmin(x[i], y[i]);
        accu_den += fmax(x[i], y[i]);     
    }
    return 1.0f - (accu_num / accu_den);;

}
}
#endif
