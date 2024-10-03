/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef INNER_PRODUCT_BASE_H
#define INNER_PRODUCT_BASE_H

#include <cstdint>
#include <cstdio>

namespace base {

/// inner product
float
fvec_inner_product_ref(const float* x, const float* y, size_t d);

/// Special version of inner product that computes 4 distances
/// between x and yi, which is performance oriented.
void
fvec_inner_product_batch_4_ref(const float* x, const float* y0,
                               const float* y1, const float* y2,
                               const float* y3, const size_t d, float& dis0,
                               float& dis1, float& dis2, float& dis3);

int32_t
ivec_inner_product_ref(const int8_t* x, const int8_t* y, size_t d);

}  // namespace base

#endif /* INNER_PRODUCT_BASE_H */
