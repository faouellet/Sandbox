import os
import re
from subprocess import Popen, PIPE
from matplotlib.ticker import FuncFormatter
import matplotlib.pyplot as plt

result_re = re.compile("Mean elapsed time: (?P<time>[0-9]+)")

cpp_files = [f for f in os.listdir(os.getcwd()) if f.endswith(".cpp")]

for f in cpp_files:
    compile_proc = Popen(["g++", "-O2", "-o", os.path.splitext(f)[0], f])
    assert(compile_proc.wait() == 0)

#programs = ["is", "ds", "loop", "it", "dt"]
#results = { "is":10, "loop":9, "ds":6, "it":2, "dt":0.5 }

#for p in programs:
#    process = Popen([p], stdout=PIPE)
#    (output, err) = process.communicate()
#    exit_code = process.wait()
#
#    match = result_re.match(output)
#    results[p] = match["time"]

#x = np.arange(len(results))
#
#fig, ax = plt.subplots()
#plt.bar(x, sorted(results.values(), reverse=True), align='center')
#plt.xticks(x, sorted(results, reverse=True, key=results.get))
#plt.show()
