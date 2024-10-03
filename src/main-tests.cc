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

#include "main-tests.h"
#include "main-supported.h"

void
check_fun_id (unsigned int fun_id)
{
    using namespace std;

    if (fun_id >= FUNC_ID_MAX)
    {
        cout << "ERROR, check_fun_id: invalid func_id " << ", exiting.\n";
        exit(-1);
    }
}

void
check_array_index (unsigned int array_index)
{
    using namespace std;

    if (array_index >= MAX_ARRAY_SIZES)
    {
        cout << "ERROR, check_array_index: index out of range " << array_index
             <<", exiting.\n";
        exit(-1);
    }
}

void
check_code_ver (unsigned int code_ver)
{
    using namespace std;

    if (code_ver >= NUM_CODE_VERSIONS)
    {
        cout << "ERROR, check_code_ver: index out of range " << code_ver
             <<", exiting.\n";
        exit(-1);
    }
}

void
record_time (unsigned int fun_id, unsigned int array_index,
             unsigned int code_ver,
             unsigned long long int  start_time,
             unsigned long long int  stop_time,
             struct results_data_t* result)
{
    /* Calculate excution time and print to out file in tab separated
       format.  */
    unsigned long long int nano_sec = stop_time - start_time;
    check_fun_id (fun_id);
    check_array_index (array_index);
    check_code_ver(code_ver);

    result[fun_id].execution_time[array_index][code_ver] = nano_sec;
}

#if GET_TIME_OF_DAY
#include <sys/time.h>

unsigned long long int
get_time(void)
{
    struct timeval tv;
    unsigned long long ns;
    /* Unfortunately, the precision is actually usec.  We will calculate ns
       to be consistent with the value from the chrono method.  */
    gettimeofday (&tv, NULL);
    ns = tv.tv_sec * 1000000000;
    ns += tv.tv_usec * 1000;

    return ns;
}
#else
#include <chrono>

unsigned long long int
get_time(void)
{
    using namespace std::chrono;

    std::chrono::nanoseconds ns =
        std::chrono::high_resolution_clock::now().time_since_epoch();

    return ns.count();
}
#endif

void record_float_result (unsigned int fun_id, unsigned int array_index,
                          unsigned int code_ver, float result_f,
                          struct results_data_t*results)
{
    check_fun_id (fun_id);
    check_array_index (array_index);
    check_code_ver(code_ver);

    results[fun_id].result_type = RESULT_FLOAT;
    results[fun_id].result_f[array_index][code_ver] = result_f;
}

void record_int_result (unsigned int fun_id, unsigned int array_index,
                        unsigned int code_ver,
                        size_t result_i,
                        struct results_data_t*results)
{
    check_fun_id (fun_id);
    check_array_index (array_index);
    check_code_ver(code_ver);

    results[fun_id].result_type = RESULT_INT;
    results[fun_id].result_i[array_index][code_ver] = result_i;
}

/**********  Eulcidian tests *************/

int
test_fvec_L2sqr_ref (struct results_data_t* distance_results,
                     unsigned int fun_id, unsigned int array_index,
                     unsigned int num_runs,
                     bool run_code_version[NUM_CODE_VERSIONS], const float* x,
                     const float* y, size_t array_size) {

    unsigned long long int  t0;
    unsigned long long int  t1;
    float result;
    int i;

    check_fun_id (fun_id);

    /* Test the original code */
    t0 = get_time();
    result = 0;

    for (i = 0; i < num_runs; i++) {
        result += base::fvec_L2sqr_ref(x, y, (size_t)array_size);
    }
    t1 = get_time();;

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);
    record_float_result (fun_id, array_index, CODE_VER_ORIG, result,
                         distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        t0 = get_time();
        result = 0;

        for (i = 0; i < num_runs; i++)
            result += powerpc::fvec_L2sqr_ref_ppc (x, y, (size_t)array_size);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);
        record_float_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                             distance_results);
    }

    /* Test the intrinsic ppc version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        t0 = get_time();
        result = 0;

        for (i = 0; i < num_runs; i++)
            result += powerpc::fvec_L2sqr_ref_ippc (x, y, (size_t)array_size);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);
        record_float_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                             distance_results);
    }

    return 0;
}

int
test_fvec_norm_L2sqr_ref (struct results_data_t* distance_results,
                          unsigned int fun_id, unsigned int array_index,
                          unsigned int num_runs,
                          bool run_code_version[NUM_CODE_VERSIONS],
                          const float* x, size_t array_size) {

    unsigned long long int  t0;
    unsigned long long int  t1;
    float result;
    unsigned int i;

    check_fun_id (fun_id);

    /* Test the original code */
    t0 = get_time();
    result = 0;

    for (i = 0; i < num_runs; i++)
        result += base::fvec_norm_L2sqr_ref (x, (size_t)array_size);

    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);
    record_float_result (fun_id, array_index, CODE_VER_ORIG, result,
                         distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        t0 = get_time();
        result = 0;

        for (i = 0; i < num_runs; i++)
            result += powerpc::fvec_norm_L2sqr_ref_ppc (x, (size_t)array_size);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);
        record_float_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                             distance_results);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        t0 = get_time();
        result = 0;

        for (i = 0; i < num_runs; i++)
            result += powerpc::fvec_norm_L2sqr_ref_ippc (x,
                                                         (size_t)array_size);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);
        record_float_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                             distance_results);
    }
    return 0;
}

int
test_fvec_L2sqr_ny_transposed_ref (
    struct results_data_t* distance_results,
    unsigned int fun_id, unsigned int array_index, unsigned int num_runs,
    bool run_code_version[NUM_CODE_VERSIONS], float * dis, const float* x,
    const float* y0, const float* y1, size_t d, size_t d_offset, size_t ny) {

    unsigned long long int  t0;
    unsigned long long int  t1;
    float result;
    int i;

    check_fun_id (fun_id);

    /* Initialize the result, size of result is ny not d.  */
    for (i = 0; i < ny; i++)
        dis[i] = 0.0;

    /* Test the original code */
    t0 = get_time();

    for (i = 0; i < num_runs; i++)
        base::fvec_L2sqr_ny_transposed_ref (dis, x, y0, y1, d, d_offset, ny);

    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);

    /* Calcuate a single result for comparison purposes.  */
    result = 0.0;
    for (i = 0; i < ny; i++)
        result += dis[i];

    record_float_result (fun_id, array_index, CODE_VER_ORIG, result,
                         distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        /* Initialize the result, size of result is ny not d.  */
        for (i = 0; i < ny; i++)
            dis[i] = 0.0;

        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            powerpc::fvec_L2sqr_ny_transposed_ref_ppc (dis, x, y0, y1, d,
                                                       d_offset, ny);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);

        /* Calcuate a single result for comparison purposes.  */
        result = 0.0;
        for (i = 0; i < ny; i++)
            result += dis[i];

        record_float_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                             distance_results);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        /* Initialize the result, size of result is ny not d.  */
        for (i = 0; i < ny; i++)
            dis[i] = 0.0;

        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            powerpc::fvec_L2sqr_ny_transposed_ref_ippc (dis, x, y0, y1, d,
                                                        d_offset, ny);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);

        /* Calcuate a single result for comparison purposes.  */
        result = 0.0;
        for (i = 0; i < ny; i++)
            result += dis[i];

        record_float_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                             distance_results);
    }

    return 0;
}

int
test_fvec_L2sqr_batch_4_ref (struct results_data_t* distance_results,
                             unsigned int fun_id, unsigned int array_index,
                             unsigned int num_runs,
                             bool run_code_version[NUM_CODE_VERSIONS],
                             const float* x, const float* y0, const float* y1,
                             const float* y2, const float* y3, const size_t d,
                             float& dp0, float& dp1, float& dp2,
                             float& dp3)
{

    unsigned long long int  t0;
    unsigned long long int  t1;
    float result;
    int i;

    check_fun_id (fun_id);

    /* Test the original code */

    dp0 = 0;
    dp1 = 0;
    dp2 = 0;
    dp3 = 0;

    t0 = get_time();
    result = 0;

    for (i = 0; i < num_runs; i++)
    {
        base::fvec_L2sqr_batch_4_ref (x, y0, y1, y2, y3, d, dp0, dp1, dp2,
                                      dp3);
	    result += dp0 + dp1 + dp2 + dp3;
	}
    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);

    record_float_result (fun_id, array_index, CODE_VER_ORIG, result,
                         distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
        {
            powerpc::fvec_L2sqr_batch_4_ref_ppc (x, y0, y1, y2, y3, d,
                                                 dp0, dp1, dp2, dp3);
            result += dp0 + dp1 + dp2 + dp3;
        }

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);

        record_float_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                             distance_results);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
        {
            powerpc::fvec_L2sqr_batch_4_ref_ippc (x, y0, y1, y2, y3, d,
                                                  dp0, dp1, dp2, dp3);
            result += dp0 + dp1 + dp2 + dp3;
        }

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);

        record_float_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                             distance_results);
    }

    return 0;
}

int
test_ivec_L2sqr_ref (struct results_data_t* distance_results,
                     unsigned int fun_id, unsigned int array_index,
                     unsigned int num_runs,
                     bool run_code_version[NUM_CODE_VERSIONS], const int8_t* x,
                     const int8_t* y, size_t d)
{

    unsigned long long int  t0;
    unsigned long long int  t1;
    int32_t result;
    int i;

    check_fun_id (fun_id);

    /* Test the original code */
    t0 = get_time();

    for (i = 0; i < num_runs; i++)
        result = base::ivec_L2sqr_ref (x, y, d);

    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);

    record_int_result (fun_id, array_index, CODE_VER_ORIG, result,
                       distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            result = powerpc::ivec_L2sqr_ref_ppc (x, y, d);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);

        record_int_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                           distance_results);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            result = powerpc::ivec_L2sqr_ref_ippc (x, y, d);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);

        record_int_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                           distance_results);
    }

    return 0;
}

/**********  Inner product tests *************/
int
test_fvec_inner_product_ref (struct results_data_t* inner_prod_result,
                             unsigned int fun_id, unsigned int array_index,
                             unsigned int num_runs,
                             bool run_code_version[NUM_CODE_VERSIONS],
                             const float* x, const float* y, size_t array_size)
{

    unsigned long long int  t0;
    unsigned long long int  t1;
    float result;
    int i;

    check_fun_id (fun_id);

    /* Test the original code */
    t0 = get_time();
    result = 0;

    for (i = 0; i < num_runs; i++)
        result += base::fvec_inner_product_ref(x, y, (size_t)array_size);

    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 inner_prod_result);
    record_float_result (fun_id, array_index, CODE_VER_ORIG, result,
                         inner_prod_result);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        t0 = get_time();
        result = 0;

        for (i = 0; i < num_runs; i++)
            result += powerpc::fvec_inner_product_ref_ppc (x, y,
                                                           (size_t)array_size);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     inner_prod_result);
        record_float_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                             inner_prod_result);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        t0 = get_time();
        result = 0;

        for (i = 0; i < num_runs; i++)
            result += powerpc::fvec_inner_product_ref_ippc (x, y,
                                                           (size_t)array_size);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     inner_prod_result);
        record_float_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                             inner_prod_result);
    }

    return 0;
}

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
                                     float& dp3)
{

    unsigned long long int  t0;
    unsigned long long int  t1;
    float result;
    int i;

    check_fun_id (fun_id);

    /* Test the original code */

    dp0 = 0;
    dp1 = 0;
    dp2 = 0;
    dp3 = 0;

    t0 = get_time();
    result = 0;

    for (i = 0; i < num_runs; i++)
    {
        base::fvec_inner_product_batch_4_ref (x, y0, y1, y2, y3, d, dp0, dp1,
                                              dp2, dp3);
	    result += dp0 + dp1 + dp2 + dp3;
	}
    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);

    record_float_result (fun_id, array_index, CODE_VER_ORIG, result,
                         distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
        {
            powerpc::fvec_inner_product_batch_4_ref_ppc (x, y0, y1, y2, y3, d,
                                                         dp0, dp1, dp2, dp3);
            result += dp0 + dp1 + dp2 + dp3;
        }

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);

        record_float_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                             distance_results);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
        {
            powerpc::fvec_inner_product_batch_4_ref_ippc (x, y0, y1, y2, y3, d,
                                                          dp0, dp1, dp2, dp3);
            result += dp0 + dp1 + dp2 + dp3;
        }

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);

        record_float_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                             distance_results);
    }

    return 0;
}

int
test_ivec_inner_product_ref (struct results_data_t* distance_results,
                             unsigned int fun_id, unsigned int array_index,
                             unsigned int num_runs,
                             bool run_code_version[NUM_CODE_VERSIONS],
                             const int8_t* x, const int8_t* y, size_t d)
{

    unsigned long long int  t0;
    unsigned long long int  t1;
    int32_t result;
    int i;

    check_fun_id (fun_id);

    /* Test the original code */
    t0 = get_time();

    for (i = 0; i < num_runs; i++)
        result = base::ivec_inner_product_ref (x, y, d);

    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);

    record_int_result (fun_id, array_index, CODE_VER_ORIG, result,
                       distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            result = powerpc::ivec_inner_product_ref_ppc (x, y, d);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);

        record_int_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                           distance_results);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            result = powerpc::ivec_inner_product_ref_ippc (x, y, d);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);

        record_int_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                           distance_results);
    }

    return 0;
}

/**********  Manhattan distance test *************/

int 
test_fvec_L1_ref (struct results_data_t* distance_results,
                  unsigned int fun_id, unsigned int array_index,
                  unsigned int num_runs,
                  bool run_code_version[NUM_CODE_VERSIONS],
                  const float* x, const float* y, size_t d)
{

    unsigned long long int  t0;
    unsigned long long int  t1;
    float result;
    int i;

    //std::cout<< num_runs << "\t"<< *x <<"\t" << *y << d << std::endl;
    check_fun_id (fun_id);

    /* Test the original code */
    t0 = get_time();

    for (i = 0; i < num_runs; i++)
        result = base::fvec_L1_ref (x, y, d);

    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);

    record_float_result (fun_id, array_index, CODE_VER_ORIG, result,
                       distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            result = powerpc::fvec_L1_ref_ppc (x, y, d);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);

        record_float_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                             distance_results);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            result = powerpc::fvec_L1_ref_ippc (x, y, d);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);

        record_float_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                             distance_results);
    }

    return 0;
}

/**********  Cosine distance test *************/

int 
test_cosine_distance_ref (struct results_data_t* distance_results,
                          unsigned int fun_id, unsigned int array_index,
                          unsigned int num_runs,
                          bool run_code_version[NUM_CODE_VERSIONS],
                          const float* x, const float* y, size_t d)
{

    unsigned long long int  t0;
    unsigned long long int  t1;
    float result;
    int i;

    //std::cout<< num_runs << "\t"<< *x <<"\t" << *y << d << std::endl;
    check_fun_id (fun_id);

    /* Test the original code */
    t0 = get_time();

    for (i = 0; i < num_runs; i++)
        result = base::cosine_distance_ref (x, y, d);

    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);

    record_float_result (fun_id, array_index, CODE_VER_ORIG, result,
                       distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            result = powerpc::cosine_distance_ref_ppc (x, y, d);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);

        record_float_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                             distance_results);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            result = powerpc::cosine_distance_ref_ippc (x, y, d);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);

        record_float_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                             distance_results);
    }

    return 0;
}

/**********  Hamming distance test *************/

int 
test_hamming_distance_ref (struct results_data_t* distance_results,
                           unsigned int fun_id, unsigned int array_index,
                           unsigned int num_runs,
                           bool run_code_version[NUM_CODE_VERSIONS],
                           const uint8_t* vec1, const uint8_t* vec2,
                           size_t size)
{

    unsigned long long int  t0;
    unsigned long long int  t1;
    size_t result;
    int i;

    //std::cout<< num_runs << "\t"<< *x <<"\t" << *y << d << std::endl;
    check_fun_id (fun_id);

    /* Test the original code */
    t0 = get_time();

    for (i = 0; i < num_runs; i++)
        result = base::hamming_distance_ref (vec1, vec2, size);

    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);

    record_int_result (fun_id, array_index, CODE_VER_ORIG, result,
                       distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
#if VEC_POPCNT_SUPPORTED
            result = powerpc::hamming_distance_ref_ppc (vec1, vec2, size);
#else
            result = base::hamming_distance_ref (vec1, vec2, size);
#endif
        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);

        record_int_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                           distance_results);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
#if VEC_POPCNT_SUPPORTED
            result = powerpc::hamming_distance_ref_ippc (vec1, vec2, size);
#else
            result = base::hamming_distance_ref (vec1, vec2, size);
#endif

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);

        record_int_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                           distance_results);
    }

    return 0;
}

/**********  Jaccard distance test *************/

int 
test_jaccard_distance_ref (struct results_data_t* distance_results,
                           unsigned int fun_id, unsigned int array_index,
                           unsigned int num_runs,
                           bool run_code_version[NUM_CODE_VERSIONS],
                           const float* x, const float* y, size_t d)
{

    unsigned long long int t0;
    unsigned long long int t1;
    float result;
    int i;

    //std::cout<< num_runs << "\t"<< *x <<"\t" << *y << d << std::endl;
    check_fun_id (fun_id);

    /* Test the original code */
    t0 = get_time();

    for (i = 0; i < num_runs; i++)
        result = base::jaccard_distance_ref (x, y, d);

    t1 = get_time();

    record_time (fun_id, array_index, CODE_VER_ORIG, t0, t1,
                 distance_results);

    record_float_result (fun_id, array_index, CODE_VER_ORIG, result,
                       distance_results);

    /* Test the ppc version of the code */
    if (run_code_version[RUN_OPTIMIZED_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            result = powerpc::jaccard_distance_ref_ppc (x, y, d);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_OPTIMIZED_PPC, t0, t1,
                     distance_results);

        record_float_result (fun_id, array_index, CODE_OPTIMIZED_PPC, result,
                             distance_results);
    }

    /* Test the ppc intrinsic version of the code */
    if (run_code_version[RUN_INTRINSIC_CODE])
    {
        result = 0;
        t0 = get_time();

        for (i = 0; i < num_runs; i++)
            result = powerpc::jaccard_distance_ippc (x, y, d);

        t1 = get_time();

        record_time (fun_id, array_index, CODE_INTRINSIC_PPC, t0, t1,
                     distance_results);

        record_float_result (fun_id, array_index, CODE_INTRINSIC_PPC, result,
                             distance_results);
    }

    return 0;
}
