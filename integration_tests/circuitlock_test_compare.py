import sys
import subprocess
import os

buildloc = sys.argv[1]
cmakepath = sys.argv[2]


goldenprefix = cmakepath + "/integration_tests/outputs/"
testoutprefix = cmakepath + "/integration_tests/temp_data/"


def compare_outputs(exe_string, stdoutfile, file_comps=None):
    exe_string = exe_string.replace("${BUILDLOC}", buildloc)
    exe_string = exe_string.replace("${CMAKE_SOURCE_DIR}", cmakepath)

    if not os.path.exists(testoutprefix):
        os.makedirs(testoutprefix)

    golden_stdout_name = goldenprefix + stdoutfile
    testout_stdout_name = testoutprefix + stdoutfile

    p = subprocess.Popen(exe_string.split(), stdout=subprocess.PIPE)
 
    testout_stdout, err = p.communicate()

    # error returned from call
    if err is not None:
        exit(1)

    testout_stdout_arr = testout_stdout.split('\n')
    golden_stdout = open(golden_stdout_name).read()

    testout_stdout = ""
    for iter1 in range(0, len(testout_stdout_arr)):
        line = testout_stdout_arr[iter1]
        if "Time " in line:
            continue
        testout_stdout += line
        if iter1 != (len(testout_stdout_arr)-1):
            testout_stdout += "\n"

    fout = open(testout_stdout_name, 'w')
    fout.write(testout_stdout)  
    fout.close()

    if testout_stdout != golden_stdout:
        exit(1)

    if file_comps is not None:
        for file_comp in file_comps:
            fing = open(goldenprefix + file_comp)
            fint = open(testoutprefix + file_comp)

            g_out = fing.read()
            t_out = fint.read()

            if g_out != t_out:
                exit(1)

            fing.close()
            fint.close()

    print "SUCCESS"



