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

#ifndef MANHATTAN_INTRINSIC_POWERPC_H
#define MANHATTAN_INTRINSIC_POWERPC_H

#define FLOAT_VEC_SIZE 4

#include <cstdint>
#include <cstdio>

namespace powerpc {

/// L1 distance
float
fvec_L1_ref_ippc(const float* x, const float* y, size_t d);

/// infinity distance
float
fvec_Linf_ref_ippc(const float* x, const float* y, size_t d);

void
fvec_madd_ref_ippc(size_t n, const float* a, float bf, const float* b,
                   float* c);

int
fvec_madd_and_argmin_ref_ippc(size_t n, const float* a, float bf,
                              const float* b, float* c);

}  // namespace powerpc 

#endif /* MANHATTAN_INTRINSIC_POWERPC_H */
