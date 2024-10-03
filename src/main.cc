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
#include <chrono>
#include <ios>
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include "main-helpers.h"

#define NY_DISTANCE 8


int
main(int argc, char *argv[])
{
    int iresult, rtn;
    using namespace std;

    int *array_sizes;
    int size, num_array_sizes = 0;
    struct results_data_t *results;
    char group_id_name[GROUP_ID_MAX][GROUP_ID_NAME_MAX];

    long long int array_size, i, j, array_index;
    float **x_d = (float **)malloc(sizeof(float *));
    float **y0_d = (float **)malloc(sizeof(float *));
    float **y1_d = (float **)malloc(sizeof(float *));
    float **y2_d = (float **)malloc(sizeof(float *));
    float **y3_d = (float **)malloc(sizeof(float *));
    int8_t **xi_d = (int8_t **)malloc(sizeof(int8_t *));
    int8_t **yi_d = (int8_t **)malloc(sizeof(int8_t *));
    uint8_t **c1_d = (uint8_t **)malloc(sizeof(uint8_t *));
    uint8_t **c2_d = (uint8_t **)malloc(sizeof(uint8_t *));
    size_t d;

    float dp0 = 0;
    float dp1 = 0;
    float dp2 = 0;
    float dp3 = 0;
    float tmp_flt;
    float result;
    struct flags_t cmd_flags;


    rtn = read_cmd_opts (argc, argv, &cmd_flags);
    if (rtn)
        cout <<"ERROR reading command line args\n";


    // Create Result directory and output file for test results.
    std::filesystem::create_directories("./results");
    // suffix for test results and time file names
    string dateSuffix = "_" + getDateAsFileSuffix() + ".txt";

      // test_results ouptut text file with time stamp
    string RESULTS_OUTPUT = "results/test_results" + dateSuffix;
    // test_time output text file with time stamp
    string TIME_OUTPUT = "results/test_time" + dateSuffix;
    ofstream timefile (TIME_OUTPUT);
    ofstream resultfile (RESULTS_OUTPUT);
    
    if (!timefile) {
        cout << "Could not open output file " << TIME_OUTPUT << "exiting.\n";
        exit (-1);
    }

    if (!resultfile) {
        cout << "Could not open output file " << RESULTS_OUTPUT
             << "exiting.\n";
        exit (-1);
    }

    /* Set the array sizes to do the testing on.  */
    array_sizes = cmd_flags.array_sizes;
    num_array_sizes = cmd_flags.num_array_sizes;
    check_array_index (num_array_sizes);

    results = (struct results_data_t*)
        malloc (sizeof (results_data_t) * FUNC_ID_MAX);

    initialize_group_func_names(group_id_name, results);

    /* Print the command line arguments, the function names, run flag,
       associated group, optimized and excluded flags if the verbose command
       line argument is specified.  */
    if (cmd_flags.verbose_output)
        print_cmd_opts (cmd_flags, results, group_id_name);

    for (array_index = 0; array_index < num_array_sizes; array_index++) {
        /* Test the various distance functions in euclidean_l2_distance.cc  */
        /* Setup input arrays for the various euclidian distance tests */

        size = array_sizes[array_index];

        cout << "Running array size "<< size << endl;

        load_data_float (size, x_d, y0_d, y1_d, y2_d, y3_d);

        const float * x = *x_d;
        const float * y0 = *y0_d;
        const float * y1 = *y1_d;
        const float * y2 = *y2_d;
        const float * y3 = *y3_d;

        load_data_int8 (size, xi_d, yi_d);

        const int8_t * xi = *xi_d;
        const int8_t * yi = *yi_d;

        load_data_char (size, c1_d, c2_d);

        const uint8_t * c1 = *c1_d;
        const uint8_t * c2 = *c2_d;

        float *dis = (float *)malloc(sizeof(float *) * NY_DISTANCE);

        /**********  Eulcidian tests *************/

        /* Test fvec_L2sqr_ref  */
        if (cmd_flags.run_func_flag[FVEC_L2SQR_REF])
            test_fvec_L2sqr_ref (results, FVEC_L2SQR_REF, array_index,
                                 cmd_flags.num_runs,
                                 cmd_flags.run_code_version, x, y2, size);

        /* Test fvec_norm_L2sqr_ref  */
        if (cmd_flags.run_func_flag[FVEC_NORM_L2SQR_REF])
            test_fvec_norm_L2sqr_ref (results, FVEC_NORM_L2SQR_REF,
                                      array_index, cmd_flags.num_runs,
                                      cmd_flags.run_code_version, x,
                                      size);

        /* Test fvec_L2sqr_ny_transposed_ref  */
        if (cmd_flags.run_func_flag[FVEC_L2SQR_NY_TRANSPOSED_REF])
            test_fvec_L2sqr_ny_transposed_ref (results,
                                               FVEC_L2SQR_NY_TRANSPOSED_REF,
                                               array_index, cmd_flags.num_runs,
                                               cmd_flags.run_code_version,
                                               dis, x, y1, y2,
                                               (size_t)(size/4), 2,
                                               NY_DISTANCE);

        /* Test fvec_L2sqr_batch_4_ref   */
        if (cmd_flags.run_func_flag[FVEC_L2SQR_BATCH_4_REF])
            test_fvec_L2sqr_batch_4_ref (results, FVEC_L2SQR_BATCH_4_REF,
                                         array_index, cmd_flags.num_runs,
                                         cmd_flags.run_code_version, x, y0, y1,
                                         y2, y3, size, dp0, dp1, dp2, dp3);

        /* Test ivec_L2sqr_ref  */
        if (cmd_flags.run_func_flag[IVEC_L2SQR_REF])
            test_ivec_L2sqr_ref (results, IVEC_L2SQR_REF, array_index,
                                 cmd_flags.num_runs,
                                 cmd_flags.run_code_version, xi, yi, size);

        /**********  Inner product tests *************/
        /* Test inner_product_ref  */
        if (cmd_flags.run_func_flag[FVEC_INNER_PRODUCT_REF])
            test_fvec_inner_product_ref (results, FVEC_INNER_PRODUCT_REF,
                                         array_index, cmd_flags.num_runs,
                                         cmd_flags.run_code_version, x, y2,
                                         size);

        /* Test ivec_inner_product_batch_4_ref  */
        if (cmd_flags.run_func_flag[FVEC_INNER_PRODUCT_BATCH_4_REF])
            test_fvec_inner_product_batch_4_ref (results,
                                                 FVEC_INNER_PRODUCT_BATCH_4_REF,
                                                 array_index,
                                                 cmd_flags.num_runs,
                                                 cmd_flags.run_code_version,
                                                 x, y0, y1, y2, y3, size,
                                                 dp0, dp1, dp2, dp3);

        /* Test ivec_inner_product_ref  */
        if (cmd_flags.run_func_flag[IVEC_INNER_PRODUCT_REF])
            test_ivec_L2sqr_ref (results, IVEC_INNER_PRODUCT_REF, array_index,
                                 cmd_flags.num_runs,
                                 cmd_flags.run_code_version, xi, yi, size);



        /**********  Manhattan distance tests *************/
	
        if (cmd_flags.run_func_flag[FVEC_L1_REF])
          { 
	    test_fvec_L1_ref (results, FVEC_L1_REF, array_index,
                          cmd_flags.num_runs,
                          cmd_flags.run_code_version, x, y0, size);
	    }

        /**********  Cosine distance test *************/
	
        if (cmd_flags.run_func_flag[COSINE_DISTANCE_REF])
          { 
	    test_cosine_distance_ref (results, COSINE_DISTANCE_REF, array_index,
                                  cmd_flags.num_runs,
                                  cmd_flags.run_code_version, x, y0, size);
	    }

        /**********  Hamming distance test *************/

        if (cmd_flags.run_func_flag[HAMMING_DISTANCE_REF])
        {
            test_hamming_distance_ref (results, HAMMING_DISTANCE_REF,
                                       array_index, cmd_flags.num_runs,
                                       cmd_flags.run_code_version, c1, c2,
                                       size);
	    }

        /**********  Jaccard distance test *************/
	
        if (cmd_flags.run_func_flag[JACCARD_DISTANCE_REF])
        {
            test_jaccard_distance_ref (results, JACCARD_DISTANCE_REF,
                                       array_index,
                                       cmd_flags.num_runs,
                                       cmd_flags.run_code_version, x, y0,
                                       size);
	    }
       
        /* Release data arrays.  */
        release_data_float (x_d, y0_d, y1_d, y2_d, y3_d, dis);
        release_data_int8 (xi_d, yi_d);
        release_data_char (c1_d, c2_d);
    }

    /* Print results */
    print_time (timefile, FUNC_ID_MAX, array_index, results, cmd_flags,
                group_id_name);
    print_result (resultfile, FUNC_ID_MAX, array_index, results, cmd_flags,
                  group_id_name);

    /* Release results array.  */
    free (results);

    timefile.close();
    resultfile.close();

    return 0;
}

