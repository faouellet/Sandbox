import os
import re
from subprocess import Popen, PIPE
from matplotlib.ticker import FuncFormatter
import matplotlib.pyplot as plt

result_re = re.compile("Mean elapsed time: (?P<time>[0-9]+)")

cpp_files = [f for f in os.listdir(os.getcwd()) if f.endswith(".cpp")]
programs = [os.path.splitext(p)[0] for p in cpp_files]

for i in range(len(cpp_files)):
    print "Compiling {0}".format(cpp_files[i])
    compile_proc = Popen(["g++", "-O2", "-o", programs[i], cpp_files[i]])
    assert(compile_proc.wait() == 0)

results = {}

for p in programs:
    print "Running {0}".format(p)
    process = Popen(["./{0}".format(p)], stdout=PIPE)
    (output, err) = process.communicate()
    assert(process.wait() == 0)

    match = result_re.match(output)
    results[p] = match.group("time")

print results

results_range = range(len(results))

fig, ax = plt.subplots()
plt.bar(results_range, sorted(results.values(), reverse=True), align='center')
plt.xticks(results_range, sorted(results, reverse=True, key=results.get))
plt.show()
