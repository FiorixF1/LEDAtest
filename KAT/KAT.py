import sys
from subprocess import Popen, PIPE

ARCH = sys.argv[1]
SL = sys.argv[2]
N0 = sys.argv[3]
assert ARCH in ["32", "64"], "ARCH must be 32 or 64"
assert SL in ["1", "2", "4"], "SL must be 1, 2 or 4"
assert N0 in ["2", "3", "4"], "N0 must be 2, 3 or 4"
key = (SL, N0)

files = {("1", "2"): "PQCkem-ephKAT_24_2.rsp",
         ("1", "3"): "PQCkem-ephKAT_24_3.rsp",
         ("1", "4"): "PQCkem-ephKAT_24_4.rsp",
         ("2", "2"): "PQCkem-ephKAT_32_2.rsp",
         ("2", "3"): "PQCkem-ephKAT_32_3.rsp",
         ("2", "4"): "PQCkem-ephKAT_32_4.rsp",
         ("4", "2"): "PQCkem-ephKAT_40_2.rsp",
         ("4", "3"): "PQCkem-ephKAT_40_3.rsp",
         ("4", "4"): "PQCkem-ephKAT_40_4.rsp"}

with open("./KAT/" + files[key], "r") as f:
    data = f.readlines()

output = "# LEDA\n"
count = 0

for row in data:
    words = row.split()
    if len(words) != 0 and words[0] == 'seed':
        process = Popen(["./{}_SL{}_N0{}".format(ARCH, SL, N0), words[-1]], stdout=PIPE)
        (response, err) = process.communicate()
        exit_code = process.wait()
        output += "\ncount = {}".format(count)
        output += response.decode("utf-8") + "\n"
        count += 1

with open("./KAT/KAT{}_SL{}_N0{}.txt".format(ARCH, SL, N0), "w") as f:
    f.write(output)

    
with open("./KAT/" + files[key], "r") as f:
    expected = f.readlines()
with open("./KAT/KAT{}_SL{}_N0{}.txt".format(ARCH, SL, N0), "r") as f:
    real = f.readlines()
    
for i in range(len(real)):
    if expected[i] != real[i]:
        print("Row {} should be:\n{}but it is actually\n{}".format(i, expected[i], real[i]))

