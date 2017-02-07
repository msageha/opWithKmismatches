#OPとなるような二つのパターンを生成するスクリプト
import random
import numpy as np
P = 16
Parray1 = np.random.randint(1, 10000, P)
o = Parray1.argsort()

file = open('pattern1.txt', 'w')
for i in range(0, P):
  file.write(str(Parray1[i]) + '\n')
file.close()

Parray2 = np.zeros(P, dtype=int)
Parray2[o] = np.random.randint(1, 10000, P).cumsum()
file = open('pattern2.txt', 'w')
for i in range(0, P):
  file.write(str(Parray2[i]) + '\n')
file.close()
