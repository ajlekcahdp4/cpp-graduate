#!/usr/bin/python

import random, sys, os, getopt

usage_string = "gentest.py -n <num> -o output"

MAX_REQUEST_VALUE = 200
MAX_CACHE_SIZE = 100
MAX_REQUESTS_NUMBER = 10000

def GenerateRandomTest(test_number):
    cache_size = random.randint(1, MAX_CACHE_SIZE)
    requests_number = random.randint(1, MAX_REQUESTS_NUMBER)
    test_str = GenerateTest(cache_size, requests_number)
    return test_str
    
def GenerateTest(cache_size, requests_number):
    test_str = []
    test_str.append(str(cache_size))
    test_str.append(str(requests_number))

    for request in range(requests_number):
        test_str.append(str(random.randint(0, MAX_REQUEST_VALUE)))
    return test_str

def Generate_N_Random_Tests (cmd):
    for test_number in range(cmd.number_of_tests):
        test_str = GenerateRandomTest(test_number)
        with open (os.path.join(cmd.output_path, "test{}.dat".format(test_number + 1)), 'w+') as test_file:
            test_file.write("{}".format(" ".join(test_str)))
            test_file.write("\n")
        



class CmdArgs:
    number_of_tests = 0
    output_path = "./resources"

    def __init__(self):
        pass

def main (argv):
    cmd = CmdArgs()
    try:
        opts, args = getopt.getopt(argv, "hn:o:")
    except getopt.GetoptError:
        print(usage_string)
        sys.exit()
    for opt, arg in opts:
        if opt == "-h":
            print(usage_string)
            sys.exit()
        if opt in ("-o", "--output"):
            cmd.output_path = str(arg)
        if opt in ("-n", "--number"):
            cmd.number_of_tests = int(arg)
    
    Generate_N_Random_Tests (cmd)



main(sys.argv[1:])