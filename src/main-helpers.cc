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

#include <iomanip>
#include <iostream>
#include "main-helpers.h"
#include <cstring>
#include <string>

#define MAX_DATE 25 // for test_results and test_time file names suffix

#define no_argument 0
#define required_argument 1
#define optional_argument 2

#define HELP_OPT                                            1000
#define VERSION_OPT                                         1001
#define SIZE_OPT                                            1002
#define FVEC_L2SQR_REF_OPT                                  1003
#define FVEC_NORM_L2SQR_REF_OPT                             1004
#define FVEC_L2SQR_NY_REF_OPT                               1005
#define FVEC_L2SQR_NY_TRANSPOSED_REF_OPT                    1006
#define FVEC_L2SQR_NY_NEAREST_REF_OPT                       1007
#define FVEC_L2SQR_NY_NEAREST_Y_TRANSPOSED_REF_OPT          1008
#define FVEC_L2SQR_BATCH_4_REF_OPT                          1009
#define IVEC_L2SQR_REF_OPT                                  1010
#define FVEC_INNER_PRODUCT_REF_OPT                          1011
#define FVEC_INNER_PRODUCT_NY_REF_OPT                       1012
#define FVEC_INNER_PRODUCT_BATCH_4_REF_OPT                  1013
#define IVEC_INNER_PRODUCT_REF_OPT                          1014
#define FVEC_L1_REF_OPT                                     1015
#define COSINE_DISTANCE_REF_OPT                             1016
#define HAMMING_DISTANCE_REF_OPT                            1017
#define JACCARD_DISTANCE_REF_OPT                            1018


// undocumented option for developers use
#define VERBOSE_OPT                                         999

/* Defines for the functions to say if it has been optimized or testing of the 
   function should be excluded for some reason.  */
#define NOT_OPTIMIZED  0
#define IS_OPTIMIZED   1
#define EXCLUDE_FLAG   1   /* Flag to filter out functions whose optimized 
                            version is not ready to be released. Command 
                            line option -X will print the results for 
                            functions marke with exclud. */
#define INCLUDE_FLAG   0

/* declared in main-tests.cc  */

void check_fun_id (unsigned int fun_id);

int long_opt = 0;

const struct option longopts[] =
  {
    {"size",                required_argument,  &long_opt, SIZE_OPT},
    {"version",             no_argument, &long_opt, VERSION_OPT},
    {"help",                no_argument, &long_opt, HELP_OPT},
    {"fvec_L2sqr_ref",      no_argument, &long_opt, FVEC_L2SQR_REF_OPT},
    {"fvec_norm_L2sqr_ref", no_argument, &long_opt, FVEC_NORM_L2SQR_REF_OPT},
    {"fvec_L2sqr_ny_transposed_ref", no_argument, &long_opt,
                                     FVEC_L2SQR_NY_TRANSPOSED_REF_OPT},
    {"fvec_L2sqr_batch_4_ref", no_argument, &long_opt,
                               FVEC_L2SQR_BATCH_4_REF_OPT},
    {"ivec_L2sqr_ref", no_argument, &long_opt, IVEC_L2SQR_REF_OPT},
    {"fvec_inner_product_ref", no_argument, &long_opt,
                               FVEC_INNER_PRODUCT_REF_OPT},
    {"fvec_inner_products_batch_4_ref", no_argument, &long_opt,
                                        FVEC_INNER_PRODUCT_BATCH_4_REF_OPT},
    {"ivec_inner_products_ref", no_argument, &long_opt,
                                IVEC_INNER_PRODUCT_REF_OPT},

    {"fvec_L1_ref", no_argument, &long_opt,
                               FVEC_L1_REF_OPT},
    
    {"cosine_distance_ref",no_argument, &long_opt, COSINE_DISTANCE_REF_OPT },
    {"hamming_distance_ref", no_argument, &long_opt, HAMMING_DISTANCE_REF_OPT},
    {"jaccard_distance_ref",no_argument, &long_opt, JACCARD_DISTANCE_REF_OPT},

    /* The code versions to run.  */
    {"run_optimized_code", no_argument, &long_opt,
                                RUN_OPTIMIZED_CODE},
    {"run_intrinsic_code", no_argument, &long_opt,
                                RUN_INTRINSIC_CODE},

    
    /* undocumented developers option */
    {"VERBOSE", no_argument, &long_opt, VERBOSE_OPT},
    {0,0,0,0},
  };

std::string getDateAsFileSuffix(void)
{
   time_t now;
   char the_date[MAX_DATE];

   the_date[0] = '\0';

   now = time(NULL);

   if (now != -1)
   {
      strftime(the_date, MAX_DATE, "%d_%b_%Y_%X", gmtime(&now));
   }

   return std::string(the_date);
}

void
print_help (void)
{
    using namespace std;
    
    cout << " -v                      Print version and exit.\n";
    cout << " --version               Print version and exit.\n";
    cout << " -h                      Print help and exit.\n";
    cout << " --help                  Print help and exit.\n";
    cout << " -R <num>                Set the number of times to run each\n";
    cout << "                         function test.\n";
    cout << "                         Default = " << NUM_RUNS << endl;
    cout << " -s <num>, --size <num>  Array size to test.  Use multiple";
    cout << " times\n";
    cout << "                         to test multiple array sizes.\n";
    cout << "\n";
    cout << " -E                      Test all euclidean distance functions.";
    cout << "\n";
    cout << " Select specific euclidean tests.\n";
    cout << " --fvec_L2sqr_ref\n";
    cout << " --fvec_norm_L2sqr_ref\n";
    cout << " --fvec_L2sqr_ny_transposed_ref\n";
    cout << " --fvec_L2sqr_batch_4_ref\n";
    cout << " --ivec_L2sqr_ref\n";
    cout << "\n";
    cout << " -I                      Test all inner product distance functions.";
    cout << "\n";
    cout << " Select specific inner product tests.\n";
    cout << " --fvec_inner_product_ref\n";
    cout << " --fvec_inner_products_batch_4_ref\n";
    cout << " --ivec_inner_products_ref\n";
    cout << "\n";
    cout << " -C                       Test  Cosine distance function\n";
    cout << "\n";
    cout << " -H                       Test  Hamming distance function\n";
    cout << "\n";
    cout << " -J                       Test  Jaccard distance function\n";
    cout << "\n";
    cout << " -M                       Test  Manhattan distance function\n";
    cout << "\n";
    cout << " --run_optimized_code      Run the optimized C code versions\n";
    cout << " --run_intrinsic_code      Run the optimized intrinsic code versions\n";
    cout << " By default, the base and the optimized code versions are run.\n";
    cout << "\n";
    cout << "\n";
    cout << " By default, all tests are run for array an size of 16.\n";
    cout << "\n";
    cout << " To run just the Euclidean subset of tests on array sizes of";
    cout << " 32 use the command:";
    cout << "   bin/test -E -s 32\n\n";

}

void
print_cmd_opts (struct flags_t cmd_flags,
                struct results_data_t *result,
                char group_id_name[][GROUP_ID_NAME_MAX])
{
    unsigned int i;
    using namespace std;

    cout << "The value of the cmd_flags structure:\n";
    cout << "num_array_sizes = " << cmd_flags.num_array_sizes << endl;
    cout << endl;

    cout << "array_sizes = ";
    for (i = 0; i < cmd_flags.num_array_sizes; i++)
    {
        if (i == cmd_flags.num_array_sizes - 1)
            cout << cmd_flags.array_sizes[i] << endl;
        else
            cout << cmd_flags.array_sizes[i] << ", ";
    }
    cout << endl;

    cout << left << setw(40) << "Function name\t";
    cout << "\trun" << "\t   group\n";
    for (i = 0; i < FUNC_ID_MAX; i++)
        {
            cout << left << setw(40) << result[i].function_name << "  \t";
            cout << cmd_flags.run_func_flag[i] << "  \t";
            cout << group_id_name[result[i].test_group] << "\n";
        }

    cout << endl;
    cout << "Run optimized functions: " <<
        cmd_flags.run_code_version[CODE_OPTIMIZED_PPC] << endl;
    cout << "Run intrinsic functions: " <<
        cmd_flags.run_code_version[CODE_INTRINSIC_PPC] << endl;
    cout << endl;
}

void
check_short_opt_no_arg (int optind, char **argv)
{
    using namespace std;
    char* first_char;

    /* Verify the short option with no argument is just a single character.
       The argv[optind-1] is the option string, for example -h.  */
    first_char = strchr (argv[optind-1], '-');

    if (!((argv[optind-1] == first_char) && (strlen(argv[optind-1]) == 2)))
    {
        cout << "Error, unrecognized option " << argv[optind] << endl;
        print_help();
        exit(-1);
    }
}

void
check_short_opt_arg (int optind, char **argv)
{
    using namespace std;
    char* first_char;

    /* Verify the short option with argument is just a single character.
       If the short option was recognized then argv[optind-2] will be the
       option string, for example -s.  If it wasn't recognized, then
       argv[optind-1] will have the option string.  Check if argv[optind-1]
       starts with a '-'.   */
    first_char = strchr (argv[optind-1], '-');
    
    if ((argv[optind-1] == first_char) && (strlen(argv[optind-1]) != 2))
    {
        /* First char is not '-' followed by a single character.  */
        cout << "Error, unrecognized option " << argv[optind-1] << endl;
        print_help();
        exit(-1);
    }
}

void
get_size_arg (char *optarg, struct flags_t *cmd_flags)
{
    using namespace std;
    int val;

    if (cmd_flags->num_array_sizes == MAX_ARRAY_SIZES)
    {
        cout << "ERROR: exceeded max number of array sizes, " <<
            MAX_ARRAY_SIZES << endl;
        exit(-1);
    }
    val = atoi(optarg);

    cmd_flags->array_sizes[cmd_flags->num_array_sizes] = val;
    cmd_flags->num_array_sizes = cmd_flags->num_array_sizes + 1;
}

/*
 * The following function getopt_long was taken from:
 *   https://ftp.software.ibm.com/aix/freeSoftware/aixtoolbox/PATCHES/libzip-1.8.0-getopt.patch
 * and adapted to work with C++.
 *
 * getopt_long
 *      Parse argc/argv argument vector, with long options.
 *
 * This implementation does not use optreset.  Instead, we guarantee that
 * it can be restarted on a new argv array after a previous call returned -1,
 * if the caller resets optind to 1 before the first call of the new series.
 * (Internally, this means we must be sure to reset "place" to EMSG before
 * returning -1.)
 */
int
getopt_long(int argc, char *const argv[],
            const char *optstring,
            const struct option *longopts, int *longindex)
{
    const char *place = EMSG;       /* option letter processing */
    char *oli;                      /* option letter list index */

    if (!*place)
    {                                      /* update scannng pointer */
        if (optind >= argc)
        {
            place = EMSG;
            return -1;
        }

        place = argv[optind];

        if (place[0] != '-')
        {
            place = EMSG;
            return -1;
        }

        place++;

        if (place[0] && place[0] == '-' && place[1] == '\0')
        {                                               /* found "--" */
            ++optind;
            place = EMSG;
            return -1;
        }

        if (place[0] && place[0] == '-' && place[1])
        {
            /* long option */
            size_t namelen;
            int    i;

            place++;

            namelen = strcspn(place, "=");
            for (i = 0; longopts[i].name != NULL; i++)
            {
                if (strlen(longopts[i].name) == namelen
                    && strncmp(place, longopts[i].name, namelen) == 0)
                {
                    int has_arg = longopts[i].has_arg;

                    if (has_arg != no_argument)
                    {
                        if (place[namelen] == '=')
                            optarg = (char *) (place + namelen + 1);
                        else if (optind < argc - 1 &&
                                 has_arg == required_argument)
                        {
                            optind++;
                            optarg = argv[optind];
                        }
                        else
                        {
                            if (optstring[0] == ':')
                                return BADARG;

                            if ((opterr && has_arg) == required_argument)
                                fprintf(stderr,
                                        "%s: option requires an argument -- %s\n",
                                        argv[0], place);

                            place = EMSG;
                            optind++;

                            if (has_arg == required_argument)
                                return BADCH;
                            optarg = NULL;
                        }
                    }
                    else
                    {
                        optarg = NULL;
                        if (place[namelen] != 0)
                        {
                            /* XXX error? */
                        }
                    }

                    optind++;

                    if (longindex)
                        *longindex = i;

                    place = EMSG;

                    if (longopts[i].flag == NULL)
                        return longopts[i].val;
                    else
                    {
                        *longopts[i].flag = longopts[i].val;
                        return 0;
                    }
                }
            }

            if (opterr && optstring[0] != ':')
                fprintf(stderr,
                        "%s: illegal option -- %s\n", argv[0], place);
            place = EMSG;
            optind++;
            return BADCH;
        }
    }

    /* short option */
    optopt = (int) *place++;

    oli = (char *) strchr(optstring, optopt);
    if (!oli)
    {
        if (!*place)
            ++optind;
        if (opterr && *optstring != ':')
            fprintf(stderr,
                    "%s: illegal option -- %c\n", argv[0], optopt);
        return BADCH;
    }

    if (oli[1] != ':')
    {                                            /* don't need argument */
        optarg = NULL;
        if (!*place)
            ++optind;
    }
    else
    {                                            /* need an argument */
        if (*place)                             /* no white space */
            optarg = (char *)place;
        else if (argc <= ++optind)
        {                                               /* no arg */
            place = EMSG;
            if (*optstring == ':')
                return BADARG;
            if (opterr)
                fprintf(stderr,
                        "%s: option requires an argument -- %c\n",
                        argv[0], optopt);
            return BADCH;
        }
        else
            /* white space */
            optarg = argv[optind];
        place = EMSG;
        ++optind;
    }
    return optopt;
}

int
read_cmd_opts (int argc, char **argv, struct flags_t *cmd_flags)
{
    using namespace std;
    unsigned int i;
    int index;
    int iarg=0;
    int val;
    /* Intially set the enable_all_X_tests to false.  The command line
       arguments will set to true as needed.  */
    bool run_subset_of_tests = false;
    bool enable_all_eculidean_tests = false;
    bool enable_all_inner_product_tests = false;
    bool enable_all_manhattan_tests = false;
    bool enable_all_cosine_tests = false;
    bool enable_all_hamming_tests = false;
    bool enable_all_jaccard_tests = false;

    bool run_subset_of_code = false;
    bool run_optimized_code = false;
    bool run_intrinsiec_code = false;

    /* Initialize the function run flags to false.  */
    for (i = 0; i < FUNC_ID_MAX; i++)
    {
        cmd_flags->run_func_flag[i] = false;
    }

    /* Initialize the code version run flags to false.  */
    cmd_flags->run_code_version[CODE_VER_ORIG] = true;  /* always run base */
    for (i = 1; i < NUM_CODE_VERSIONS; i++)
    {
        cmd_flags->run_code_version[i] = false;
    }

    while(iarg != -1)
    {
        iarg = getopt_long(argc, argv, "s:R:EIHCMJvh", longopts, &index);

        if (iarg == -1)
            /* At end of arguments exit loop.  */
            break;

        switch (iarg)
        {
        case 0:
            switch (long_opt)
            {
            case HELP_OPT:
                print_help();
                exit(0);
                break;

            case VERSION_OPT:
                std::cout << "Version "<< VERSION << "    Date: "
                          << VER_DATE << std::endl;
                exit(0);
                break;

            case SIZE_OPT:
                get_size_arg (optarg, cmd_flags);
                break;

                /* Select a subset of events to run, otherwise run all tests.
                 */
            case FVEC_L2SQR_REF_OPT:
                run_subset_of_tests = true;
                cmd_flags->run_func_flag[FVEC_L2SQR_REF] = true;
                break;

            case FVEC_NORM_L2SQR_REF_OPT:
                run_subset_of_tests = true;
                cmd_flags->run_func_flag[FVEC_NORM_L2SQR_REF] = true;
                break;

            case FVEC_L2SQR_NY_TRANSPOSED_REF_OPT:
                run_subset_of_tests = true;
                cmd_flags->run_func_flag[FVEC_L2SQR_NY_TRANSPOSED_REF] = true;
                break;

            case FVEC_L2SQR_BATCH_4_REF_OPT:
                run_subset_of_tests = true;
                cmd_flags->run_func_flag[FVEC_L2SQR_BATCH_4_REF] = true;
                break;

            case IVEC_L2SQR_REF_OPT:
                run_subset_of_tests = true;
                cmd_flags->run_func_flag[IVEC_L2SQR_REF] = true;
                break;

            case FVEC_INNER_PRODUCT_REF_OPT:
                run_subset_of_tests = true;
                cmd_flags->run_func_flag[FVEC_INNER_PRODUCT_REF] = true;
                break;

            case FVEC_INNER_PRODUCT_BATCH_4_REF_OPT:
                run_subset_of_tests = true;
                cmd_flags->run_func_flag[FVEC_INNER_PRODUCT_BATCH_4_REF]
                    = true;
                break;

            case IVEC_INNER_PRODUCT_REF_OPT:
                run_subset_of_tests = true;
                cmd_flags->run_func_flag[IVEC_INNER_PRODUCT_REF]
                    = true;
                break;

            case RUN_OPTIMIZED_CODE:
                run_subset_of_code = true;
                cmd_flags->run_code_version[CODE_OPTIMIZED_PPC] = true;
                break;

            case RUN_INTRINSIC_CODE:
                run_subset_of_code = true;
                cmd_flags->run_code_version[CODE_INTRINSIC_PPC] = true;
                break;

            case VERBOSE_OPT:
                cmd_flags->verbose_output = true;
                break;

            default:
                cout << "ERROR, unknown long_opt value " << long_opt << endl;
                exit(-1);
            }
            break;

        case 'h':
            check_short_opt_no_arg(optind, argv);
            print_help();
            exit(0);
            break;

        case 'v':
            check_short_opt_no_arg(optind, argv);
            std::cout << "Version "<< VERSION << "    Date: "
                      << VER_DATE << std::endl;
            exit(0);
            break;
    
        case 's':     /* Sort set size option.  */
            check_short_opt_arg(optind, argv);
            get_size_arg (optarg, cmd_flags);
            break;

        case 'R':     /* Set number of runs. */
            check_short_opt_arg(optind, argv);
            cmd_flags->num_runs = atoi(optarg);
            break;

        case 'E':     /* Run all Euclidean tests. */
            check_short_opt_no_arg(optind, argv);
            run_subset_of_tests = true;
            enable_all_eculidean_tests = true;
            break;

        case 'I':     /* Run all inner product tests. */
            check_short_opt_no_arg(optind, argv);
            run_subset_of_tests = true;
            enable_all_inner_product_tests = true;
            break;
    
        case 'M':     /* Run all Manhattan distance tests.  */
            check_short_opt_no_arg(optind, argv);
            run_subset_of_tests = true;
            enable_all_manhattan_tests = true;
            cmd_flags->run_func_flag[FVEC_L1_REF] = true;
            break;
    
        case 'C':     /* Run all COSINE distance tests.  */
            check_short_opt_no_arg(optind, argv);
            run_subset_of_tests = true;
            enable_all_cosine_tests = true;
            cmd_flags->run_func_flag[COSINE_DISTANCE_REF] = true;
            break;

        case 'H':     /* Run all Hamming distance tests.  */
            check_short_opt_no_arg(optind, argv);
            run_subset_of_tests = true;
            enable_all_hamming_tests = true;
            cmd_flags->run_func_flag[HAMMING_DISTANCE_REF] = true;
            break;
    
        case 'J':     /* Run all Jaccard distance tests.  */
            check_short_opt_no_arg(optind, argv);
            run_subset_of_tests = true;
            enable_all_jaccard_tests = true;
            cmd_flags->run_func_flag[JACCARD_DISTANCE_REF] = true;
            break;
        default:
            std::cout << endl;
            print_help();
            exit(-1);
        }
    }

    /* Set defaults.  */
    if (cmd_flags->num_array_sizes == 0)
    {
        /* Minimum run is for vector length of 16.  */
        cmd_flags->num_array_sizes = 1;
        cmd_flags->array_sizes[0] = 16;
    }

    /* By default enable all tests in the group unless the user specifically
       specified a subset of events to run.  */
    if ((run_subset_of_tests && enable_all_eculidean_tests )
        || !run_subset_of_tests)
    {
        cmd_flags->run_func_flag[FVEC_L2SQR_REF] = true;
        cmd_flags->run_func_flag[FVEC_NORM_L2SQR_REF] = true;
        cmd_flags->run_func_flag[FVEC_L2SQR_NY_TRANSPOSED_REF] = true;
        cmd_flags->run_func_flag[FVEC_L2SQR_BATCH_4_REF] = true;
        cmd_flags->run_func_flag[IVEC_L2SQR_REF] = true;
    }

    if ((run_subset_of_tests && enable_all_inner_product_tests )
        || !run_subset_of_tests)
    {
        cmd_flags->run_func_flag[FVEC_INNER_PRODUCT_REF] = true;
        cmd_flags->run_func_flag[FVEC_INNER_PRODUCT_BATCH_4_REF] = true;
        cmd_flags->run_func_flag[IVEC_INNER_PRODUCT_REF] = true;
    }


    if ((run_subset_of_tests && enable_all_manhattan_tests)
        || !run_subset_of_tests)
    {
        cmd_flags->run_func_flag[FVEC_L1_REF] = true;
    }

    if ((run_subset_of_tests && enable_all_cosine_tests)
         || !run_subset_of_tests)
    {
        cmd_flags->run_func_flag[COSINE_DISTANCE_REF] = true;
    }

    if ((run_subset_of_tests && enable_all_hamming_tests)
         || !run_subset_of_tests)
    {
        cmd_flags->run_func_flag[HAMMING_DISTANCE_REF] = true;
    }

    if ((run_subset_of_tests && enable_all_jaccard_tests)
         || !run_subset_of_tests)
    {
        cmd_flags->run_func_flag[JACCARD_DISTANCE_REF] = true;
    }

    /* Set which code bases to run.  If run_subset_of code has not been set,
       then just run the optimized code base by default.  Otherwise, run the
       specified code bases.  */

    if (run_subset_of_code == false)
        cmd_flags->run_code_version[CODE_OPTIMIZED_PPC] = true;
    
   /* Still may need to disable tests marked as excluded or not optimized.
      However, we can't do that until the results structure with the
      function names and their corresponding flags have been initialized.
      Will disable the tests as needed after the call to
      initialize_group_func_names() is done.  */

    return 0;
}

void
setup_function_info(struct results_data_t *result, int fun_id,
                    int test_group, const char* name)
  
{
    check_fun_id (fun_id);
    if (strlen(name) >= NAME_LEN)
        {
            std::cout << "ERROR, function name length " << strlen(name);
            std::cout << " is too long.  Increase #define NAME_LEN.Exiting.";
            std::cout << std::endl;
            exit (-1);
        }
    strcpy (result[fun_id].function_name, name);
    result[fun_id].test_group = test_group;
}

void
set_group_name (int group_id, const char* name,
                char group_id_name[][GROUP_ID_NAME_MAX])
{
    using namespace std;

    if (group_id >= GROUP_ID_MAX)
    {
        cout << "ERROR: the group id exceeds the GROUP_ID_MAX. Exiting.\n";
        exit (-1);
    }

    if (strlen(name) >= GROUP_ID_NAME_MAX)
    {
        cout << "ERROR: the group name length (" << strlen(name) << "exceeds the GROUP_ID_NAME_MAX value " << GROUP_ID_NAME_MAX << endl;
        cout << "Exiting.\n";
        exit (-1);
    }
    strcpy(group_id_name[group_id], name);
}

void
initialize_group_func_names (char group_id_name[][GROUP_ID_NAME_MAX],
                             struct results_data_t *result)
{
    int fun_id;

    /* Initialize the group names.  Add new group name ids to
       enum test_group_id in main-helpers.h  */
    set_group_name (EUCLIDEAN, "Euclidean", group_id_name);
    set_group_name (INNER_PRODUCT, "Inner product", group_id_name);
    set_group_name (MANHATTAN, "Manhattan", group_id_name);
    set_group_name (COSINE, "Cosine", group_id_name);
    set_group_name (HAMMING, "Hamming", group_id_name);
    set_group_name (JACCARD, "Jaccard", group_id_name);
    
    /* The IS_OPTIMIZED is used if the PowerPC function has been optimized,
       use NOT_OPTIMIZED otherwise.

       If for some reason, we want to generally exclude a function from the
       testing (for example the optimized code is worse than the original code)
       use EXCLUDE_FLAG, otherwise use INCLUDE_FLAG.

       Generally, if the function is not optimized, I have also set
       EXCLUDE_FLAG but that is not required.
    */

    /*  Euclidean functions.  */
    fun_id = FVEC_L2SQR_REF;
    setup_function_info (result, fun_id, EUCLIDEAN, "fvec_L2sqr_ref");

    fun_id = FVEC_NORM_L2SQR_REF;
    setup_function_info (result, fun_id, EUCLIDEAN, "fvec_norm_L2sqr_ref");

    fun_id = FVEC_L2SQR_NY_TRANSPOSED_REF;
    /* Current attempts to optimize did not improve performance. Using
       optimized function is identical to base function.  */
    setup_function_info (result, fun_id, EUCLIDEAN,
                         "fvec_L2sqr_ny_transposed_ref");

    fun_id = FVEC_L2SQR_BATCH_4_REF;
    setup_function_info (result, fun_id, EUCLIDEAN,
                         "fvec_L2sqr_batch_4_ref");

    fun_id = IVEC_L2SQR_REF;
    /* Currently, no changes to optimize the function.  */
    setup_function_info (result, fun_id, EUCLIDEAN, "ivec_L2sqr_ref");

    /*  Inner product functions.  */
    fun_id = FVEC_INNER_PRODUCT_REF;
    setup_function_info (result, fun_id, INNER_PRODUCT,
                         "fvec_inner_product_ref");

    fun_id = FVEC_INNER_PRODUCT_BATCH_4_REF;
    setup_function_info (result, fun_id, INNER_PRODUCT,
                         "fvec_inner_products_batch_4_ref");

    fun_id = IVEC_INNER_PRODUCT_REF;
    /* Attempts to optimize have not improved this function.  */
    setup_function_info (result, fun_id, INNER_PRODUCT,
                         "ivec_inner_products_ref");

    /* Manhattan tests */

    fun_id = FVEC_L1_REF;
    setup_function_info (result, fun_id, MANHATTAN,
                         "fvec_L1_ref");

    fun_id = COSINE_DISTANCE_REF;
    setup_function_info (result, fun_id, COSINE,
                         "cosine_distance_ref");

    fun_id = HAMMING_DISTANCE_REF;
    setup_function_info (result, fun_id, HAMMING,
                         "hamming_distance_ref");

    fun_id = JACCARD_DISTANCE_REF;
    setup_function_info (result, fun_id, JACCARD,
                         "jaccard_distance_ref");
}

void
print_group_name (std::ofstream &out_file, int *group_id, int index,
                  struct results_data_t* result,
                  char group_id_name[][GROUP_ID_NAME_MAX])
{
    /* Check to see if we are switching to a new group of
       functions.  */
    if (*group_id != result[index].test_group)
    {
        if (*group_id != -1)
            out_file << std::endl;

        /* Switching to a new group.  Print the group name.  */
        out_file << group_id_name[result[index].test_group];
        out_file << ":" << std::endl;
        *group_id = result[index].test_group;
    }
}

void
print_time_code_ver (std::ofstream &out_file, int fun_index_max,
                     int array_index_max,
                     struct results_data_t* result, struct flags_t cmd_flags,
                     char group_id_name[][GROUP_ID_NAME_MAX],
                     int code_ver, char *suffix)
{
    unsigned int i, j;
    int group_id = -1;     /* Initialize id to print group names  */

    for (i = 0; i< fun_index_max; i++)
    {
        if (cmd_flags.run_func_flag[i])
        {
            print_group_name (out_file, &group_id, i, result, group_id_name);

            out_file << "  " << result[i].function_name << suffix << "\t";

            for (j = 0; j< array_index_max; j++)
                out_file << result[i].execution_time[j][code_ver]
                         << "\t";

            out_file << "\n";
        }
    }
    out_file << "\n";
}

void
print_percentage_code_ver (std::ofstream &out_file, int fun_index_max,
                           int array_index_max,
                           struct results_data_t* result,
                           struct flags_t cmd_flags,
                           char group_id_name[][GROUP_ID_NAME_MAX],
                           int code_ver, char *suffix)
{
    /* Print the percentage improvement of the version of the code relative
       to the base version. */
    unsigned int i, j;
    int group_id = -1;          /* Initialize id to print group names  */

    for (j = 0; j< array_index_max; j++)
        out_file << cmd_flags.array_sizes[j] << "\t\t";

    out_file << "\n";

    group_id = -1;            /* Initialize id to print group names  */
    for (i = 0; i< fun_index_max; i++)
    {
        float percent_time, orig_f, ppc_f;

        if (cmd_flags.run_func_flag[i])
        {
            print_group_name (out_file, &group_id, i, result, group_id_name);

            out_file << "  " << result[i].function_name << suffix <<"\t";
            for (j = 0; j< array_index_max; j++)
            {
                orig_f = (float) result[i].execution_time[j][CODE_VER_ORIG];
                ppc_f = (float) result[i].execution_time[j][code_ver];

                percent_time = (ppc_f / orig_f) * 100.0;
                
                out_file << std::fixed << std::setprecision (2)
                         << percent_time << "%\t"<<"(" <<orig_f/ppc_f <<" times)\t";
		
            }
            out_file << "\n";
        }
    }
}

void
print_time (std::ofstream &out_file, int fun_index_max, int array_index_max,
            struct results_data_t* result, struct flags_t cmd_flags,
            char group_id_name[][GROUP_ID_NAME_MAX])
{
    unsigned int i, j;
    int group_id = -1;
    char suffix[MAX_SUFFIX];

    /* The function execution times are in result, the array containing the
       floags of which fucntions were run is in cmd_flags.  Only print the
       results for the functions that were run.  */

    using namespace std;

    /* Print the base execution times.  */
    out_file << "Original code execution time in ns.\n";
    out_file << "Function name \t array size\n\t";
    strcpy (suffix, PPC_BASE_SUFFIX);

    for (j = 0; j< array_index_max; j++)
        out_file << cmd_flags.array_sizes[j] << "\t";

    out_file << "\n";

    print_time_code_ver (out_file, fun_index_max, array_index_max, result,
                         cmd_flags, group_id_name, CODE_VER_ORIG, suffix);
    out_file << "\n";

    /* Print the optimized results.  */
    if (cmd_flags.run_code_version[CODE_OPTIMIZED_PPC])
    {
        out_file << "Optimized PowerPC code execution time in ns.\n";
        out_file << "Function name \t array size\n\t";
        strcpy (suffix, PPC_OPT_SUFFIX);

        print_time_code_ver (out_file, fun_index_max, array_index_max, result,
                             cmd_flags, group_id_name, CODE_OPTIMIZED_PPC, suffix);

        /*  Print the Percentage improvement for the optimized code version
            relative to the base version.  */
        out_file <<  "Percentage of optimized PPC execution time versus original time.\n";
        out_file << "Function name \t array size\n\t";
        print_percentage_code_ver (out_file, fun_index_max, array_index_max,
                                   result, cmd_flags, group_id_name,
                                   CODE_OPTIMIZED_PPC, suffix);
        out_file << "\n";
    }

    /* Print the intrinsic results.  */
    if (cmd_flags.run_code_version[CODE_INTRINSIC_PPC])
    {
        out_file << "Intrinsic PowerPC code execution time in ns.\n";
        out_file << "Function name \t array size\n\t";
        strcpy (suffix, PPC_INTRINSIC_SUFFIX);

        print_time_code_ver (out_file, fun_index_max, array_index_max, result,
                             cmd_flags, group_id_name, CODE_INTRINSIC_PPC,
                             suffix);

        /*  Print the Percentage improvement for the optimized code version
            relative to the base version.  */
        out_file <<  "Percentage of intrinsic PPC execution time versus original time.\n";
        out_file << "Function name \t array size\n\t";
        print_percentage_code_ver (out_file, fun_index_max, array_index_max,
                                   result, cmd_flags, group_id_name,
                                   CODE_INTRINSIC_PPC, suffix);
        out_file << "\n";

    }
}

int
print_results_check_int (int result_base, int result_run,
                         std::ofstream &out_file)
{
    float diff;
    int rtn = 0;

    if (result_base != 0)
         diff = abs (result_base - result_run) / result_base;
    else
        diff = result_run;

    if (diff < ERR_THRESHOLD)
        out_file << "True\t";
    else
    {
        out_file << "False\t";
        rtn = 1;
    }
    return rtn;
}

int
print_results_check_flt (float result_base, float result_run,
                         std::ofstream &out_file)
{
    float diff;
    int rtn = 0;

    if (result_base != 0.0)
         diff = abs (result_base - result_run) / result_base;
    else
        diff = result_run;

    if (diff < ERR_THRESHOLD)
        out_file << "True\t";
    else
    {
        out_file << "False\t";
        rtn = 1;
    }
    return rtn;
}

void
print_results_code_ver (std::ofstream &out_file, int fun_id_max,
                        int array_index_max,
                        struct results_data_t* result,
                        struct flags_t cmd_flags,
                        char group_id_name[][GROUP_ID_NAME_MAX],
                        int code_ver, char *suffix)
{
    int i, j;
    int group_id = -1;   /* Initialize id to print group names  */
    int exceeded_threshold = 0;

    for (j = 0; j< array_index_max; j++)
        if (code_ver != CODE_VER_ORIG)
            out_file << cmd_flags.array_sizes[j] << "\t\t";
        else
            out_file << cmd_flags.array_sizes[j] << "\t";

    out_file << "\n";

    for (i = 0; i< fun_id_max; i++)
    {
        if (cmd_flags.run_func_flag[i])
        {
            print_group_name (out_file, &group_id, i, result, group_id_name);

            out_file << "  " << result[i].function_name << suffix << "\t";

            if ( result[i].result_type == RESULT_FLOAT)
            {
                for (j = 0; j< array_index_max; j++)
                {
                    if (code_ver != CODE_VER_ORIG)
                    {
                        out_file << result[i].result_f[j][code_ver] << "\t";
                        exceeded_threshold += print_results_check_flt (
                            result[i].result_f[j][CODE_VER_ORIG],
                            result[i].result_f[j][code_ver],
                            out_file);
                    }
                    else
                        out_file << result[i].result_f[j][code_ver] << "\t";
                }
            }
            else
            {
                for (j = 0; j< array_index_max; j++)
                {
                    if (code_ver != CODE_VER_ORIG)
                    {
                        out_file << result[i].result_i[j][code_ver] << "\t";
                        exceeded_threshold += print_results_check_int (
                            result[i].result_i[j][CODE_VER_ORIG],
                            result[i].result_i[j][code_ver],
                            out_file);
                    }
                    else
                        out_file << result[i].result_i[j][code_ver] << "\t";
                }
            }
            out_file << "\n";
        }
    }
    if (exceeded_threshold > 0) {
        out_file << "\n";
        out_file << "\n";
        out_file << "There were " << exceeded_threshold << " which exceeded "
                 << "the result variability threshold.\n";
    }
    out_file << "\n";
}

void
print_result (std::ofstream &out_file, int fun_id_max, int array_index_max,
              struct results_data_t* result,
              struct flags_t cmd_flags,
              char group_id_name[][GROUP_ID_NAME_MAX])
{
    char suffix[MAX_SUFFIX];

    /* Original execution results.  */
    out_file << "Original code execution results.\n";
    out_file << "Function name \t array size\n\t";
    strcpy (suffix, PPC_BASE_SUFFIX);

    print_results_code_ver (out_file, fun_id_max, array_index_max, result,
                            cmd_flags, group_id_name,
                            CODE_VER_ORIG, suffix);
    out_file << "\n";

    /* PowerPC optimized execution results */
    if (cmd_flags.run_code_version[CODE_OPTIMIZED_PPC])
    {
        out_file << "PowerPC optimized code execution results.\n";
        out_file << "Function name \t array size\n\t";
        strcpy (suffix, PPC_OPT_SUFFIX);

        print_results_code_ver (out_file, fun_id_max, array_index_max, result,
                                cmd_flags, group_id_name,
                                CODE_OPTIMIZED_PPC, suffix);
        out_file << "\n";
    }

    /* PowerPC intrinsic execution results */
    if (cmd_flags.run_code_version[CODE_INTRINSIC_PPC])
    {
        out_file << "PowerPC intrinsic code execution results.\n";
        out_file << "Function name \t array size\n\t";
        strcpy (suffix, PPC_INTRINSIC_SUFFIX);

        print_results_code_ver (out_file, fun_id_max, array_index_max, result,
                                cmd_flags, group_id_name,
                                CODE_INTRINSIC_PPC, suffix);
        out_file << "\n";
    }
}

void
load_data_char (size_t d, uint8_t **c1, uint8_t **c2)
{
    using namespace std;
    uint8_t *c1p;
    uint8_t *c2p;

    *c1 = (uint8_t *)malloc(d * sizeof(uint8_t));
    *c2 = (uint8_t *)malloc(d * sizeof(uint8_t));

    if (!(*c1) || !(*c2)) {
        cout << "ERROR, failed to allocat the unit8_t data arrays.\n";
        /* Failed to malloc all of the arrays, release memory, return error.
         */
        free (*c1);
        free (*c2);
        exit (-1);
    }
    c1p = *c1;
    c2p = *c2;

    for (size_t  i = 0; i < (int) d; i++) {
        c1p[i] = (uint8_t) i;
        c2p[i] = (uint8_t) (i * 5);
    }
    return;
}

void
release_data_char (uint8_t **c1, uint8_t **c2)
{
    free (*c1);
    free (*c2);
    return;
}

int
load_data_float (size_t d, float **x, float **y0, float **y1, float **y2,
                 float **y3)
{
    using namespace std;
    int i;
    float *xp;
    float *y0p;
    float *y1p;
    float *y2p;
    float *y3p;

    *x = (float *) malloc(d * sizeof(float));
    *y0 = (float *) malloc(d * sizeof(float));
    *y1 = (float *) malloc(d * sizeof(float));
    *y2 = (float *) malloc(d * sizeof(float));
    *y3 = (float *) malloc(d * sizeof(float));

    if (!(*x) || !(*y0) || !(*y1) || !(*y2) || !(*y3)) {
        cout << "ERROR, failed to allocat the euclidean_l2_distance float data arrays.\n";
        /* Failed to malloc all of the arrays, release memory, return error.
         */
        free (*x);
        free (*y0);
        free (*y1);
        free (*y2);
        free (*y3);
        exit (-1);
    }

    xp = *x;
    y0p = *y0;
    y1p = *y1;
    y2p = *y2;
    y3p = *y3;

    for ( i = 0; i < (int) d; i++) {
        xp[i] = (float) (i + 3);
        /* Cosine distance function needs x and y0 to be relatively far apart.
           The issue is the optimized code uses the vsx instructions which
           do slightly different rounding.  If the x and y0 data points are
           too close, then the results vary more than desired.  */
        y0p[i] = (float) ((d - i) + 75.12);
        y1p[i] = (float) (i + 23.11);
        //       y2p[i] = (float) (i + 49.14);
        y2p[d-i-1] = (float) y1p[i] * y1p[i];
        y3p[i] = (float) (i + 10.10);
    }
    /* Tweek a couple of array values so fvec_L2sqr_ny_nearest_ref and
       fvec_L2sqr_ny_nearest_y_transposed_ref will compute a non zero result.
    */
    xp[0] = 1.4;
    y1p[1] = (float) (1);

    return 0;
}

void
release_data_float (float **x, float **y0, float **y1,
                    float **y2, float **y3,  float *dis)
{
    free (*x);
    free (*y0);
    free (*y1);
    free (*y2);
    free (*y3);
    free (dis);
    return;
}

void
load_data_int8 (size_t d, int8_t **x, int8_t **y)
{
    using namespace std;
    int i;
    int8_t *xp;
    int8_t *yp;

    *x = (int8_t *) malloc(d * sizeof(int8_t));
    *y = (int8_t *) malloc(d * sizeof(int8_t));

    if (!*x || !*y) {
        cout << "ERROR, failed to allocat the euclidean_l2_distance integer  data arrays.\n";
        /* Failed to malloc all of the arrays, release memory, return error.
         */
        free (*x);
        free (*y);
        exit (-1);
    }

    xp = *x;
    yp = *y;

    for ( i = 0; i < (int) d; i++) {
        xp[i] = i;
        yp[i] = 2*i;
    }
}

void
release_data_int8 (int8_t **x, int8_t **y)
{
    free (*x);
    free (*y);
}
