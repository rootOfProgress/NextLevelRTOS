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
general_tests = [general.test_general001_lifetime, ]
memory_benchmarks = [memory.test_memory001_alloc_benchmark, ]
scheduler_benchmarks = [scheduler.test_scheduler001_sleep_benchmark, scheduler.test_scheduler002_sleep_benchmark_load, scheduler.test_scheduler003_contextswitch_benchmark]
scheduler_tests = [scheduler.test_scheduler004_finish_task]

def execute_test(testcollection, test_log, test_type, clean_environment = True):
    global failed_tests
    results = []
    for test in testcollection:
        logging.print_bold("Starting test " + test.__name__)
        if (clean_environment):
            util.prepare_device()
        result = test()

        if (test_type == TestType.BENCHMARK):
            results.append(result)
        elif (test_type == TestType.LOGIC):
            if (result):
                logging.print_success(test.__name__ + " PASSED")
                results.append({ test.__name__ : "PASSED" })
            else:
                logging.print_fail(test.__name__ + " FAILED")
                results.append({ test.__name__ : "FAILED" })
                failed_tests.append({ test.__name__ : "FAILED" })
            
        test_log.write("Results of test " + test.__name__ + "\n")
        test_log.write(json.dumps(results) + "\n\n")
    return results

def run_all():
    global failed_tests
    failed_tests = []
    test_log = open("test/log/test_" +  datetime.today().strftime('%Y-%m-%d_%H:%M:%S'), "a+")

    lifetime = os_health.get_lifetime()

    meta = "Test running on OS version " + str(lifetime["os_version"]) + " on commit " + str(lifetime["git_hash"]) + "\n"
    test_log.write(meta)

    results = []
    results.append(execute_test(general_tests, test_log, TestType.LOGIC, False))
    results.append(execute_test(scheduler_tests, test_log, TestType.LOGIC, False))
    results.append(execute_test(memory_benchmarks, test_log, TestType.BENCHMARK))
    results.append(execute_test(scheduler_benchmarks, test_log, TestType.BENCHMARK))

    if (failed_tests):
        logging.print_bold("\n Failed tests:")
        for failed in failed_tests:
            logging.print_fail(json.dumps(failed))
    else:
        logging.print_success(str(len(general_tests) + len(scheduler_tests)) + " TESTS PASSED")
        logging.print_fail(str(len(failed_tests)) + " TESTS FAILED")
    return results

