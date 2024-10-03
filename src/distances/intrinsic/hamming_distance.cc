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

#include <altivec.h>
#include <cstddef>
#include <iostream>
#include <vector>
#include "../../main-supported.h"   /* Contains #define VEC_POPCNT_SUPPORTED */

#define CHAR_VEC_SIZE 16

namespace powerpc {

#if VEC_POPCNT_SUPPORTED

size_t hamming_distance_ref_ippc (const uint8_t* vec1, const uint8_t* vec2,
                                  size_t size) {
    size_t distance = 0;
    size_t base;

    base = (size / CHAR_VEC_SIZE) * CHAR_VEC_SIZE;
    // Process 16 bytes (128 bits) at a time using vector registers
    for (size_t i = 0; i < base;  i += CHAR_VEC_SIZE) {
        vector unsigned char v1 = vec_xl((long)(i*sizeof(uint8_t)),
                                         (uint8_t*) vec1);
        vector unsigned char v2 = vec_xl((long)(i*sizeof(uint8_t)),
                                         (uint8_t*) vec2);

        __vector unsigned char xor_result = vec_xor(v1, v2);
        // Count the number of 1 bits in each byte
        __vector unsigned char popcount = vec_popcnt(xor_result);

        // Manually sum the popcounts from each lane
        for (int j = 0; j < 16; ++j) {
            distance += popcount[j];
        }
    }

    // Handle any remaining elements (less than 16 bytes)
    for (size_t i = base; i < size; i++) {
        uint8_t xor_result = vec1[i] ^ vec2[i];
        // Built-in popcount function for the remaining bytes
        distance += __builtin_popcount(xor_result);
    }

    return distance;
}

#else
    /* The test function will call the base code version of the function.
       Just need a function definition here for compiling.  */

size_t hamming_distance_ref_ippc(const uint8_t* vec1, const uint8_t* vec2,
                                 size_t size) {
    size_t distance = 0;
    return distance;
}
#endif

} //namespace powerpc

#endif
