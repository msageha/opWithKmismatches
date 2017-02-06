#STchangeIndex.csvというST[i:i+m]からST[i+1:i+m+1]に変更されるときに，どのインデックスが変わったかを示しているcsvファイルを吐く
import numpy as np
SUPECIALCHAR = 10001
K = 1 #mistakesNum

with open('../PatternText/pattern.txt', 'r') as f:
  P = np.array(list(map(int, f.read().splitlines())))
PLength = len(P)

predP = np.zeros(PLength, dtype=int)
pred = SUPECIALCHAR
for i in P.argsort():
  predP[i] = pred
  pred = i

SP = np.zeros(PLength, dtype=int)
for i in range(PLength):
  if(predP[i]==SUPECIALCHAR):
    SP[i] = predP[i]
  else:
    SP[i] = predP[i] - i

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
    pred = i+j

ST = np.zeros((TLength-PLength+1, PLength), dtype=int)
for i in range(TLength-PLength+1):
  for j in range(PLength):
    if(predT[i][j]==SUPECIALCHAR):
      ST[i][j] = predT[i][j]
    else:
      ST[i][j] = predT[i][j] - (i+j)

STchangeIndex = np.zeros((TLength-PLength, PLength), dtype=int) #STがi→i+1になるときにどこのインデックスがチェンジしたかをiが管理している

for i in range(TLength-PLength):
    for j in range(PLength-1):
        if ST[i][j+1] != ST[i+1][j]:
            STchangeIndex[i][j] = 1

with open('./STchangeIndex.csv', 'w') as f:
    for i in range(TLength-PLength):
        for j in range(PLength):
            f.write(str(STchangeIndex[i][j])+'\t')
        f.write('\n')

