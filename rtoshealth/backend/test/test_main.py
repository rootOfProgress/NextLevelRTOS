import test.memory as memory
import test.scheduler as scheduler
import test.general as general
import test.test_util as util
import test.nrf2401 as nrf2401
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
scheduler_tests = [scheduler.test_scheduler004_finish_task, scheduler.test_scheduler005_join_task, scheduler.test_scheduler006_task_control]
memory_tests = [memory.test_memory002_various]

device_tests = [nrf2401.test_nrf_000_various]

def check_test_exceptions(testname, condition):
    if (testname == "test_scheduler006_task_control" and not condition):
        return False

    return True


def execute_test(testcollection, test_log, test_type, clean_environment = True, requires_logging = True):
    global failed_tests
    global num_of_failed
    global num_of_passed

    for test in testcollection:
        results = []
        lifetime_info = {}

        if (clean_environment):
            lifetime_info = util.prepare_device()

        if lifetime_info and not check_test_exceptions(test.__name__, lifetime_info["systick_enabled"] == '1'):
            logging.print_warning("Skipping test " + test.__name__)
            pass
        else:
            logging.print_bold("Starting test " + test.__name__)

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
            
            if requires_logging == True:
                test_log.write("\nResults of test " + test.__name__ + "\n")
                test_log.write(json.dumps(results, indent=1) + "\n\n")
    return results

def nrf2401():
    results = []
    test_log = []
    results.append(execute_test(device_tests, test_log, TestType.LOGIC, False, False))
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
    test_log.write(meta)

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

    logging.print_success(str(num_of_passed) + "/" + str(num_of_passed + num_of_failed) + " tests passed")

    return results

