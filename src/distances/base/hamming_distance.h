/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cstdint>
#include <cstdio>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <vector>

#define FLOAT_VEC_SIZE 4
#define INT32_VEC_SIZE 4
#define INT8_VEC_SIZE  16

namespace base {
	size_t hamming_distance_builtin (const std::vector<uint8_t>& vec1,
                                     const std::vector<uint8_t>& vec2);
	size_t hamming_distance_ref (const uint8_t* vec1, const uint8_t* vec2,
                                 size_t size);
}// namespace base 
