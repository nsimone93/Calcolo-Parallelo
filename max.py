import os

all = []

max = 16
for j in range(23):
	max = max * 2
	row = []
	row.append(max)
	for i in range(0, 6):
		f = open('./output/res' + str(max) + '_' + str(pow(2, i)) + '.out', 'r')
		line = f.read().split('\n')
		for k in range(len(line)-1):
			times = line[k].split(';')
			if len(row) > i + 1:
				if row[i + 1] < float(times[-1]):
					row[i + 1] = float(times[-1])
			else:
				row.append(float(times[-1]))
		f.close()
	all.append(row)

f = open('./all.csv', 'w')
for i in range(len(all)):
	for j in range(len(all[0])):
		if j != len(all[0]) - 1:
			f.write(str(all[i][j]) + ';')
		else:
			f.write(str(all[i][j]) + '\n')
f.close()
