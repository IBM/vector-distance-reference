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

#ifndef DISTANCES_INTRINSIC_POWERPC_H
#define DISTANCES_INTRINSIC_POWERPC_H

#define FLOAT_VEC_SIZE 4

#include <cstdint>
#include <cstdio>

namespace powerpc {

/// Squared L2 distance between two vectors
float
fvec_L2sqr_ref_ippc (const float* x, const float* y, size_t d);

/// squared norm of a vector
float
fvec_norm_L2sqr_ref_ippc (const float* x, size_t d);

/// compute ny square L2 distance between x and a set of transposed contiguous
/// y vectors. squared lengths of y should be provided as well
void
fvec_L2sqr_ny_transposed_ref_ippc (float* dis, const float* x, const float* y,
                                   const float* y_sqlen, size_t d,
                                   size_t d_offset, size_t ny);

/// Special version of L2sqr that computes 4 distances
/// between x and yi, which is performance oriented.
void
fvec_L2sqr_batch_4_ref_ippc (const float* x, const float* y0, const float* y1,
                             const float* y2, const float* y3, const size_t d,
                             float& dis0, float& dis1, float& dis2,
                             float& dis3);

int32_t
ivec_L2sqr_ref_ippc (const int8_t* x, const int8_t* y, size_t d);

}  // namespace powerpc 

#endif /* DISTANCES_INSTRINSIC_REF_H */
