/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cstdint>
#include <cstdio>

namespace base {

/// Squared L2 distance between two vectors
float
fvec_L2sqr_ref(const float* x, const float* y, size_t d);

/// squared norm of a vector
float
fvec_norm_L2sqr_ref(const float* x, size_t d);

/// compute ny square L2 distance between x and a set of transposed contiguous
/// y vectors. squared lengths of y should be provided as well
void
fvec_L2sqr_ny_transposed_ref(float* dis, const float* x, const float* y,
                             const float* y_sqlen, size_t d, size_t d_offset,
                             size_t ny);

/// Special version of L2sqr that computes 4 distances
/// between x and yi, which is performance oriented.
void
fvec_L2sqr_batch_4_ref(const float* x, const float* y0, const float* y1,
                       const float* y2, const float* y3, const size_t d,
                       float& dis0, float& dis1, float& dis2, float& dis3);

int32_t
ivec_L2sqr_ref(const int8_t* x, const int8_t* y, size_t d);

}  // namespace base 
