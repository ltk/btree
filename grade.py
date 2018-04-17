#!/usr/bin/python
#
# The authoritative place for this script is in the inginious-grader
# repository.

# To call this from the command line: python grade.py some_test_binary

from subprocess import call
from subprocess import STDOUT
from collections import OrderedDict
from getopt import getopt
import os
import sys
import json
import random

def usage():
    print("grade.py [-n] some_test_binary")
    print("")
    print("    -n : non-interactive. meant for autograders.")
    print("")
    print("Examples:")
    print(" $ python grade.py hello_test")
    print(" $ python grade.py -n linked_list_test")

# Be sure the user gave us a binary to test.
if len(sys.argv) < 2:
    usage()
    sys.exit(-1)

# Get the command line arguments and process them.
argv = sys.argv[1:]
try:
    opts, args = getopt(argv, "n")
except(e):
    print(e)
    usage()
    sys.exit(2)

noninteractive = False # default to interactive mode
for opt, arg in opts:
    if opt == '-n':
        noninteractive = True
binary = args[0]

# Load test tags and point values from points.json    
tests = OrderedDict()
try:
    with open('points.json') as point_file:
        top = json.load(point_file)
        points = top['points']
        for entry in points:
            for k in entry.keys():
                tests[k] = entry[k]        
except:
    print ('Couldn\'t open or parse points.json')
    sys.exit(-1)

# Invoke each test tag and record the exit status code    
results = OrderedDict()
failedTests = []
DN = open(os.devnull, 'w')
total_points = 0
try:
    for key, val in tests.items():
        status = call(["./" + binary, "[" + key + "]"], stdout=DN, stderr=STDOUT)
        if status != 0:
            failedTests.append("./" + binary + " \"[" + key + "]\"")
        results[key] = status
        total_points += val
    DN.close()
except:
    print ("Couldn't invoke the unit tests. Did it compile? (hint: type 'make' in your terminal)")
    sys.exit(-1)


# Tally points earned and print stuff out    
earned_points = 0
bads = [ ] # list of failed tests (keys)
for key in results:
    this_points = 0
    if results[key] == 0:
        this_points = tests[key]
    earned_points += this_points
    line = "{:<20} {:2} / {:2} ".format(key, str(this_points), str(tests[key])) # + chk
    if noninteractive and this_points != tests[key]:
        bads.append(key)
    elif not noninteractive:
        print(line)
if not noninteractive:
    print ("===============================")

full = False
happygram = ''
if earned_points == total_points:
    full = True
line = '{:<20} {:2} / {:2}'.format('TOTAL', str(earned_points), str(total_points))
if not noninteractive:
    print(line)

# Show a parting message to either submit or how to troubleshoot.
if not noninteractive:
    if full:
        print ("You should be good to submit your assignment now!")
    else:
        print ("Command line(s) to invoke specific failed unit tests follow this message. They")
        print ("will give you much more detailed information about what's wrong with your program.")
        print ("")

    for f in failedTests:
        print (f)

# Add final output for the grade. This is for the inginious callback. 
epf = float(earned_points)
tpf = float(total_points)
grade = int((epf / tpf) * 100)

# The last line MUST be the grade.
if noninteractive:
    if len(bads) == 0:
        print("All tests passed! Huzzah!")
    else:
        print("{} failed tests: {}".format(len(bads), ", ".join(bads)))
print (str(grade))
