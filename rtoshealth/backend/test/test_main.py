import test.memory as memory
import test.scheduler as scheduler
import test.test_util as util
import util.os_health as os_health
import util.logging as logging

import json

from datetime import datetime 

memory_tests = [memory.test_memory001_alloc_benchmark, ]
scheduler_tests = [scheduler.test_scheduler001_sleep_benchmark, scheduler.test_scheduler002_contextswitch_benchmark]

def execute_test(testcollection, test_log):
    results = []
    for test in testcollection:
        logging.print_bold("Starting test " + test.__name__)
        util.prepare_device()
        result = test()
        results.append(result)
        test_log.write("Results of test " + test.__name__ + "\n")
        test_log.write(json.dumps(result) + "\n")
    return results

def run_all():
    test_log = open("test/log/test_" +  datetime.today().strftime('%Y-%m-%d_%H:%M:%S'), "a+")

    lifetime = os_health.get_lifetime()

    meta = "Test running on OS version " + str(lifetime["os_version"]) + " on commit " + str(lifetime["git_hash"]) + "\n"
    test_log.write(meta)

    results = []
    results.append(execute_test(memory_tests, test_log))
    results.append(execute_test(scheduler_tests, test_log))
    # for mem_test in memory_tests:
    #     logging.print_bold("Starting test " + mem_test.__name__)
    #     util.prepare_device()
    #     result = mem_test()
    #     results.append(result)
    #     test_log.write("Results of test " + mem_test.__name__ + "\n")
    #     test_log.write(json.dumps(result) + "\n")

    # for sched_test in scheduler_tests:
    #     logging.print_bold("Starting test " + sched_test.__name__)
    #     util.prepare_device()
    #     result = sched_test()
    #     results.append(result)
    #     test_log.write("Results of test " + sched_test.__name__ + "\n")
    #     test_log.write(json.dumps(result) + "\n")

    return results

