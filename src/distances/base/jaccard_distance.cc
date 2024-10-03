/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "jaccard_distance.h"

#include <cmath>

namespace base {

float 
jaccard_distance_ref (const float* x, const float* y, size_t d)
{
    float accu_num = 0.0, accu_den = 0.0;
    size_t i;
    for (size_t i = 0; i < d; i++) {
        accu_num += fmin(x[i], y[i]);
        accu_den += fmax(x[i], y[i]);
    }
    return 1.0f - accu_num / accu_den;
}

} // namespace base 

