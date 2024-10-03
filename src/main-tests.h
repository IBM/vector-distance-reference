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

#include <stdio.h>
#include <ctime>
#include <ios>
#include <iostream>

#include "distances/intrinsic/euclidean_l2_distance.h"
#include "distances/optimized/euclidean_l2_distance.h"
#include "distances/base/euclidean_l2_distance.h"

#include "distances/intrinsic/innerproduct.h"
#include "distances/optimized/innerproduct.h"
#include "distances/base/innerproduct.h"

#include "distances/intrinsic/manhattan_l1_distance.h"
#include "distances/optimized/manhattan_l1_distance.h"
#include "distances/base/manhattan_l1_distance.h"

#include "distances/intrinsic/cosine_distance.h"
#include "distances/optimized/cosine_distance.h"
#include "distances/base/cosine_distance.h"

#include "distances/intrinsic/hamming_distance.h"
#include "distances/optimized/hamming_distance.h"
#include "distances/base/hamming_distance.h"

#include "distances/intrinsic/jaccard_distance.h"
#include "distances/optimized/jaccard_distance.h"
#include "distances/base/jaccard_distance.h"

#define NAME_LEN 60
#define MAX_ARRAY_SIZES 20

#define RESULT_FLOAT 0
#define RESULT_INT   1

#define CODE_VER_ORIG       0   /* Used as index into results arrays.  Orig
                                   version must be index 0.  */
#define CODE_OPTIMIZED_PPC  1
#define CODE_INTRINSIC_PPC  2
#define NUM_CODE_VERSIONS   3

#define RUN_OPTIMIZED_CODE  1
#define RUN_INTRINSIC_CODE  2

struct results_data_t {
    char function_name[NAME_LEN];
    unsigned long long int execution_time[MAX_ARRAY_SIZES][NUM_CODE_VERSIONS];
    int result_type = -1;
    int test_group = -1;           /* In group of euclidean, innerproduct..*/
    long int result_i[MAX_ARRAY_SIZES][NUM_CODE_VERSIONS];
    float result_f[MAX_ARRAY_SIZES][NUM_CODE_VERSIONS];
};

enum func_id {
    FVEC_L2SQR_REF = 0,
    FVEC_NORM_L2SQR_REF,
    FVEC_L2SQR_NY_TRANSPOSED_REF,
    FVEC_L2SQR_BATCH_4_REF,
    IVEC_L2SQR_REF,
    FVEC_INNER_PRODUCT_REF,
    FVEC_INNER_PRODUCT_BATCH_4_REF,
    IVEC_INNER_PRODUCT_REF,
    FVEC_L1_REF,
    COSINE_DISTANCE_REF,
    HAMMING_DISTANCE_REF,
    JACCARD_DISTANCE_REF,
    FUNC_ID_MAX,
};

void
check_array_index (unsigned int array_index);

void
record_time(unsigned int fun_id, unsigned int array_index,
            unsigned int code_ver,
            unsigned long long int start_time,
            unsigned long long int stop_time,
            struct results_data_t* result);

void
record_float_result(unsigned int fun_id,  unsigned int array_index,
                    unsigned int code_ver,
                    float result_f, struct results_data_t *result);

void 
record_int_result (unsigned int fun_id, unsigned int array_index,
                   unsigned int code_ver, size_t result_i,
                   struct results_data_t*results);

int
test_fvec_L2sqr_ref (struct results_data_t* result,
                     unsigned int fun_id, unsigned int array_index,
                     unsigned int num_runs,
                     bool run_code_version[NUM_CODE_VERSIONS],
                     const float* x, const float* y, size_t array_size);

int
test_fvec_norm_L2sqr_ref (struct results_data_t* result,
                          unsigned int fun_id, unsigned int array_index,
                          unsigned int num_runs,
                          bool run_code_version[NUM_CODE_VERSIONS],
                          const float* x, size_t array_size);

int
test_fvec_L2sqr_ny_ref (struct results_data_t* result,
                        unsigned int fun_id, unsigned int array_index,
                        unsigned int num_runs,
                        bool run_code_version[NUM_CODE_VERSIONS], float* dis,
                        const float* x, const float* y, size_t d, size_t ny);

int
test_fvec_L2sqr_ny_transposed_ref (struct results_data_t* result,
                                   unsigned int fun_id,
                                   unsigned int array_index,
                                   unsigned int num_runs,
                                   bool run_code_version[NUM_CODE_VERSIONS],
                                   float * dis, const float* x,
                                   const float* y0, const float* y1, size_t d,
                                   size_t d_offset, size_t ny);

int
test_fvec_L2sqr_batch_4_ref (struct results_data_t* result,
                             unsigned int fun_id, unsigned int array_index,
                             unsigned int num_runs,
                             bool run_code_version[NUM_CODE_VERSIONS],
                             const float* x, const float* y0, const float* y1,
                             const float* y2, const float* y3, const size_t d,
                             float& dis0, float& dis1, float& dis2,
                             float& dis3);

int
test_ivec_L2sqr_ref(struct results_data_t* result,
                    unsigned int fun_id, unsigned int array_index,
                    unsigned int num_runs,
                    bool run_code_version[NUM_CODE_VERSIONS],
                    const int8_t* x, const int8_t* y, size_t d);

int
test_fvec_inner_product_ref (struct results_data_t* result,
                             unsigned int fun_id, unsigned int array_index,
                             unsigned int num_runs,
                             bool run_code_version[NUM_CODE_VERSIONS],
                             const float* x, const float* y,
                             size_t array_size);

int
test_fvec_inner_product_batch_4_ref (struct results_data_t* distance_results,
                                     unsigned int fun_id,
                                     unsigned int array_index,
                                     unsigned int num_runs,
                                     bool run_code_version[NUM_CODE_VERSIONS],
                                     const float* x, const float* y0,
                                     const float* y1, const float* y2,
                                     const float* y3, const size_t d,
                                     float& dp0, float& dp1, float& dp2,
                                     float& dp3);

int
test_ivec_inner_product_ref (struct results_data_t* distance_results,
                             unsigned int fun_id, unsigned int array_index,
                             unsigned int num_runs,
                             bool run_code_version[NUM_CODE_VERSIONS],
                             const int8_t* x, const int8_t* y, size_t d);


int 
test_fvec_L1_ref (struct results_data_t* distance_results,
                  unsigned int fun_id, unsigned int array_index,
                  unsigned int num_runs,
                  bool run_code_version[NUM_CODE_VERSIONS], const float* x,
                  const float* y, size_t d);


int 
test_cosine_distance_ref (struct results_data_t* distance_results,
                          unsigned int fun_id, unsigned int array_index,
                          unsigned int num_runs,
                          bool run_code_version[NUM_CODE_VERSIONS],
                          const float* x, const float* y, size_t d);

int  
test_hamming_distance_ref (struct results_data_t* distance_results,
                           unsigned int fun_id, unsigned int array_index,
                           unsigned int num_runs,
                           bool run_code_version[NUM_CODE_VERSIONS],
                           const uint8_t* vec1, const uint8_t* vec2,
                           size_t size);

int 
test_jaccard_distance_ref (struct results_data_t* distance_results,
                           unsigned int fun_id, unsigned int array_index,
                           unsigned int num_runs,
                           bool run_code_version[NUM_CODE_VERSIONS],
                           const float* x, const float* y, size_t d);


