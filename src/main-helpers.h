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

#include <cstring>
#include <string>
#include <ctime>
#include <ios>
#include <iostream>
#include <fstream>
#include "main-tests.h"
#include <unistd.h>

#define VERSION    "0.6"
#define VER_DATE   "9/23/2024"

#define PPC_BASE_SUFFIX ""
#define PPC_OPT_SUFFIX "_ppc"
#define PPC_INTRINSIC_SUFFIX "_ippc"
#define MAX_SUFFIX 6

/* The scalar instruction used in the base versus the VSX instructions ued
   in the intrinsic an optimized versions have slightly different rounding
   modes which leads to small differences in the resutls.  The result check
   is primarily intended to verify the optimized code versions are correct.
   Ignore the round off errors.  */
#define ERR_THRESHOLD 0.00005

#include <string.h>
struct option
{
        const char *name;
        int has_arg;
        int *flag;
        int val;
};

/*
 * The following definitions are used by function getopt_long.  The function
 * was taken from:
 *   https://ftp.software.ibm.com/aix/freeSoftware/aixtoolbox/PATCHES/libzip-1.8.0-getopt.patch

 * and adapted to work with * C++.
*/
#define no_argument 0
#define required_argument 1
#define optional_argument 2

#define BADCH   '?'
#define BADARG  ':'
#define EMSG    ""

std::string getDateAsFileSuffix(void);
int read_cmd_opts (int argc, char ** argv, struct flags_t *cmd_flags);

int load_data_float (size_t d, float **x, float **y0, float **y1, float **y2,
                     float **y3);
void release_data_float (float **x, float **y0, float **y1, float **y2,
                         float **y3, float *dis);
void load_data_int8 (size_t d, int8_t **x, int8_t **y);
void release_data_int8 (int8_t **x, int8_t **y);
void load_data_char (size_t d, uint8_t **c1, uint8_t **c2);
void release_data_char (uint8_t **c1, uint8_t **c2);

/* Call each function NUM_RUNS to get a reasonably large execution time for
   the function.  Goal is to have the number of runs large enough relative
   to the various system activity to get a reasonably consistent execution
   time.   */
#define NUM_RUNS 10000000     /* Default.  */

struct flags_t {
    int array_sizes[MAX_ARRAY_SIZES];
    int num_runs = NUM_RUNS;
    int num_array_sizes = 0;
    bool run_func_flag[FUNC_ID_MAX];
    bool run_un_optimized = false;
    bool run_excluded = false;
    bool verbose_output = false;
    bool run_subset = false;
    bool run_code_version[NUM_CODE_VERSIONS];
};

/* The indexes to access the group names in group_id_name */
#define GROUP_ID_NAME_MAX  20

enum test_group_id {
    EUCLIDEAN = 0,
    INNER_PRODUCT,
    MANHATTAN,
    COSINE,
    HAMMING,
    JACCARD,
    GROUP_ID_MAX,
};

void print_cmd_opts (struct flags_t cmd_flags,
                    struct results_data_t *result,
                   char group_id_name[][GROUP_ID_NAME_MAX]);

void initialize_group_func_names (char group_id_name[][GROUP_ID_NAME_MAX],
                                  struct results_data_t *result);
void print_time (std::ofstream &out_file, int fun_index, int array_index,
                 struct results_data_t* result,
                 struct flags_t cmd_flags,
                 char group_id_name[][GROUP_ID_NAME_MAX]);
void print_result (std::ofstream &out_file, int fun_index, int array_index,
                   struct results_data_t* result, struct flags_t cmd_flags,
                   char group_id_name[][GROUP_ID_NAME_MAX]);
void disable_excluded_un_optimized_tests (struct flags_t *cmd_flags,
                                          struct results_data_t *result);
void setup_function_info(struct results_data_t *result, int fun_id,
                         int test_group,
                         const char* name, bool optimized,
                         bool output_excluded);
