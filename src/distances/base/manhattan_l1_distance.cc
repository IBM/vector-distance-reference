/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "manhattan_l1_distance.h"
#include <cmath>
#include <ios>
#include <iostream>

#define FLOAT_VEC_SIZE 4
#define INT32_VEC_SIZE 4
#define INT8_VEC_SIZE  16

namespace base {

float
fvec_L1_ref(const float* x, const float* y, size_t d) {
      	size_t i;
	float res = 0;
       
       for (i = 0; i < d; i++) {
	 
           const float tmp = x[i] - y[i];
           res += std::fabs(tmp);
       }
       return res;

}

float
fvec_Linf_ref(const float* x, const float* y, size_t d) {
	size_t i;
 	float res = 0;
       for (i = 0; i < d; i++) {
         res = std::fmax(res, std::fabs(x[i] - y[i]));
       }
       return res;
}

}  // namespace base 

