# To call this from the command line:
# python grade.py

from subprocess import call
from subprocess import STDOUT
from collections import OrderedDict
import os
import sys
import json

# Be sure the user gave us a binary to test.
if len(sys.argv) != 2:
    print 'Syntax:  python grade.py some_test'
    print 'Example: python grade.py linked_list_test'
    sys.exit(-1)
binary = sys.argv[1]

# Set up some colorization definitions and color printer.
class termcolors:
    CYAN = '\033[36m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

def color_print(color, text):
    print color + text + termcolors.ENDC

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
    print 'Couldn\'t open or parse points.json'
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
    print ""
except:
    print "Couldn't invoke the unit tests. Did it compile? (hint: type 'make' in your terminal)"
    sys.exit(-1)

# In the event the student has full points, show a happy emoji at the end
def choose_happygram():
    # choose among the following at random, once i have network and can find out how random works in python...
    # u'\U0001F389' # party popper
    # u'\U0001f604' # grin
    # u'\U0001F308' # rainbow
    # u'\U0001F60E' # sunglasses
    return u'\U0001F60E'

# Tally points earned and print stuff out

# Special condition for b-tree homework, we have 40 points possible out of 20 points for 'full credit'
actual_total_points = total_points
# total_points = 20
earned_points = 0
for key in results:
    this_points = 0
    if results[key] == 0:
        this_points = tests[key]
    earned_points += this_points
    chk = ''
    col = termcolors.WARNING
    if this_points > 0:
        col = termcolors.CYAN
        chk = u'\u2713'
    line = "{:<20} {:2} / {:2}  ".format(key, str(this_points), str(tests[key])) + chk
    color_print(col, line)
print "==============================="
col = termcolors.FAIL
full = False
happygram = ''
if earned_points > 0:
    col = termcolors.WARNING
if earned_points >= total_points:
    col = termcolors.OKGREEN
    full = True
if earned_points == actual_total_points:
    happygram = choose_happygram()
line = '{:<20} {:2} / {:2}'.format('TOTAL', str(earned_points), str(total_points)) + ' ' + happygram
color_print(col, line)

# Show a parting message to either submit or how to troubleshoot.
print ""
if full:
    print "You should be good to submit your assignment now!"
else:
    print "Command line(s) to invoke specific failed unit tests follow this message. They"
    print "will give you much more detailed information about what's wrong with your program."
    print ""
    
for f in failedTests:
    print f

