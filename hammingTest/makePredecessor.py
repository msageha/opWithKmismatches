#パターンとテキストのpredecessorを作り，csvを吐くスクリプト
import numpy as np

SUPECIALCHAR = 100001

with open('../PatternText/pattern.txt', 'r') as f:
  P = np.array(list(map(int, f.read().splitlines())))
PLength = len(P)

predP = np.zeros(PLength, dtype=int)
pred = SUPECIALCHAR
for i in P.argsort():
  predP[i] = pred
  pred = i

with open('../PatternText/text.txt', 'r') as f:
  T = np.array(list(map(int, f.read().splitlines())))
TLength = len(T)

Tsub = np.zeros((TLength-PLength+1, PLength), dtype=int)
for i in range(TLength-PLength+1):
  for j in range(PLength):
    Tsub[i][j] = T[i+j]

predT = np.zeros((TLength-PLength+1, PLength), dtype=int)

for i in range(TLength-PLength+1):
  pred = SUPECIALCHAR
  for j in Tsub[i].argsort():
    predT[i][j] = pred
    pred = j + i

with open('./predecessor.csv', 'w') as f:
  for i in range(PLength):
    f.write(str(predP[i])+'\t')
  f.write('\n\n')
  for i in range(TLength-PLength+1):
    for j in range(PLength):
      f.write(str(predT[i][j])+'\t')
    f.write('\n')
