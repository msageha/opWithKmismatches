import numpy as np
import itertools
import time

mismatches = 0

with open('../PatternText/pattern.txt', 'r') as f:
  P = np.array(list(map(int, f.read().splitlines())))

with open('../PatternText/text.txt', 'r') as f:
  T = np.array(list(map(int, f.read().splitlines())))

ans = np.zeros(len(T), dtype=list) #謎いことしてる
PLength = len(P)
TLength = len(T)
start = time.time()
for i in range(TLength-PLength):
  print("start::{}".format(i))
  for K in itertools.combinations(range(PLength), PLength-mismatches):
    check = True
    for x in K:
      for y in K:
        if((P[x] <= P[y] and T[i+x] > T[i+y])
          or (T[i+x] <= T[i+y] and P[x] > P[y])):
          check = False
          break
    if(check):
      ans[i] = K
      break
finish = time.time()
with open('../answerPy/{}.txt'.format(mismatches), 'w') as f:
  f.write('P: {0}\nT: {1}\nmismatches: {2}\n'.format(PLength, TLength, mismatches))
  for i in range(len(T)):
    if(ans[i]):
      f.write('i: {0}\tK: {1}\n'.format(i, ans[i]))
  f.write('time: {}sec'.format(finish-start))