import os

for i in range(0, 6): 
    procs = pow(2, i)
    max = 16
    for j in range(23): 
        max = max * 2
        f = open('./Makefile', 'w')
        f.write('./bin/bitonic' + str(max) + '_' + str(procs) + '.o: ./src/bitonic.c\n')
        f.write('\tmpcc ./src/bitonic.c -I/usr/include/sys -L/usr/lpp/ppe.hpct/lib -lhpc -lpmapi -o ./bin/bitonic' + str(max) + '_' + str(procs) + '.o -lm -I/usr/lpp/ppe.hpct/include -L/usr/lpp/ppe.hpct/lib -lhpc -lpmapi\n')
        f.write('run: ./bitonic' + str(max) + '_' + str(procs) + '.o\n')
        f.write('\t./bitonic' + str(max) + '_' + str(procs) + '.o ../input/input' + str(max) + '.txt -procs ' + str(procs) + '\n')
        f.write('.PHONY: clean\n')
        f.write('clean:')
        f.write('rm -r ./bin/bitonic' + str(max) + '_' + str(procs) + '.o\n')
        f.close()
        os.system('make')

        f = open('./job/parallel' + str(max) + '_' + str(procs) + '.job', 'w')
        f.write('#@ initialdir = ~/bin\n')
        f.write('#@ input = /dev/null\n')
        f.write('#@ output = ../output/res' + str(max) + '_' + str(procs) + '.out\n')
        f.write('#@ error = ../output/res' + str(max) + '_' + str(procs) + '.err\n')
        f.write('#@ job_type = parallel\n')
        f.write('#@ class = medium\n')
        f.write('#@ total_tasks = ' + str(procs) + '\n')
        f.write('#@ queue\n')
        f.write('./bitonic' + str(max) + '_' + str(procs) + '.o ../input/input' + str(max) + '.txt\n')
        f.close()
        os.system('llsubmit ./job/parallel' + str(max) + '_' + str(procs) + '.job')
