/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */


#include <cstdint>
#include <cstdio>

namespace base {

/// L1 distance
float
fvec_L1_ref(const float* x, const float* y, size_t d);

/// infinity distance
float
fvec_Linf_ref(const float* x, const float* y, size_t d);

void
fvec_madd_ref(size_t n, const float* a, float bf, const float* b, float* c);

int
fvec_madd_and_argmin_ref(size_t n, const float* a, float bf, const float* b,
                         float* c);

}  // namespace base 

