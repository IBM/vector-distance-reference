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

#ifndef INNER_PRODUCT_POWERPC_H
#define INNER_PRODUCT_POWERPC_H

#include <cstdint>
#include <cstdio>

namespace powerpc {

/// inner product
float
fvec_inner_product_ref_ppc(const float* x, const float* y, size_t d);

/// Special version of inner product that computes 4 distances
/// between x and yi, which is performance oriented.
void
fvec_inner_product_batch_4_ref_ppc(const float* x, const float* y0,
                                   const float* y1, const float* y2,
                                   const float* y3, const size_t d,
                                   float& dis0, float& dis1, float& dis2,
                                   float& dis3);

int32_t
ivec_inner_product_ref_ppc(const int8_t* x, const int8_t* y, size_t d);

}  // namespace powerpc 

#endif /* INNER_PRODUCT_POWERPC_H */
