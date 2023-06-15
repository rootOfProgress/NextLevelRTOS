import test.memory as memory
import test.scheduler as scheduler
import test.general as general
import test.test_util as util
import util.os_health as os_health
import util.logging as logging
from enum import Enum

import json

from datetime import datetime 

class TestType(Enum):
    BENCHMARK = 1,
    LOGIC = 2

failed_tests = []
num_of_passed = 0
num_of_failed = 0
general_tests = [general.test_general001_lifetime, ]
memory_benchmarks = [memory.test_memory001_alloc_benchmark, ]
scheduler_benchmarks = [scheduler.test_scheduler001_sleep_benchmark, scheduler.test_scheduler002_sleep_benchmark_load, scheduler.test_scheduler003_contextswitch_benchmark]
scheduler_tests = [scheduler.test_scheduler004_finish_task, scheduler.test_scheduler005_join_task]
memory_tests = [memory.test_memory002_various]

def execute_test(testcollection, test_log, test_type, clean_environment = True):
    global failed_tests
    global num_of_failed
    global num_of_passed

    for test in testcollection:
        results = []
        logging.print_bold("Starting test " + test.__name__)
        if (clean_environment):
            util.prepare_device()
        result = test()

        if (test_type == TestType.BENCHMARK):
            results.append(result)
        elif (test_type == TestType.LOGIC):
            for r in result:
                if (r[1]):
                    logging.print_success(r[0] + " PASSED")
                    results.append({r[0]: "PASSED" })
                    num_of_passed += 1
                else:
                    logging.print_fail(r[0] + " FAILED")
                    results.append({r[0]: "FAILED" })
                    failed_tests.append({ r[0] : "FAILED" })  
                    num_of_failed += 1                  

            # if (result):
            #     logging.print_success(test.__name__ + " PASSED")
            #     results.append({ test.__name__ : "PASSED" })
            # else:
            #     logging.print_fail(test.__name__ + " FAILED")
            #     results.append({ test.__name__ : "FAILED" })
            #     failed_tests.append({ test.__name__ : "FAILED" })
            
        test_log.write("\nResults of test " + test.__name__ + "\n")
        test_log.write(json.dumps(results, indent=1) + "\n\n")
    return results

def run_all():
    global num_of_failed
    global num_of_passed
    global failed_tests

    num_of_passed = 0
    num_of_failed = 0
    failed_tests = []
    test_log = open("test/log/test_" +  datetime.today().strftime('%Y-%m-%d_%H:%M:%S'), "a+")

    lifetime = os_health.get_lifetime()

    meta = "Test running on OS version " + str(lifetime["os_version"]) + " on commit " + str(lifetime["git_hash"]) + "\n"
    meta = "--Config--\n " + "Debug Mode: " + str(lifetime["debug_mode"]) + "\n" + " Systick Enabled: " + str(lifetime["systick_enabled"]) + "\n"
    test_log.write(meta)

    results = []
    results.append(execute_test(general_tests, test_log, TestType.LOGIC, False))
    results.append(execute_test(memory_tests, test_log, TestType.LOGIC, True))
    results.append(execute_test(scheduler_tests, test_log, TestType.LOGIC, True))
    results.append(execute_test(scheduler_benchmarks, test_log, TestType.BENCHMARK))
    results.append(execute_test(memory_benchmarks, test_log, TestType.BENCHMARK))

    if (failed_tests):
        logging.print_bold("\n Failed tests:")
        for failed in failed_tests:
            logging.print_fail(json.dumps(failed))
    # else:
        # logging.print_success(str(len(general_tests) + len(scheduler_tests)) + " TESTS PASSED")
        # logging.print_fail(str(len(failed_tests)) + " TESTS FAILED")

    logging.print_success(str(num_of_passed) + "/" + str(num_of_passed + num_of_failed) + " tests passed")

    return results

