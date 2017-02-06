#パターンとテキストの各部分文字列について提案手法にそってハミング距離を計算して，K以下のインデックスを吐くスクリプト
import numpy as np
import time

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
      ST[i][j] = predT[i][j] - (i+j)

#LCPの準備
LCP = np.array([np.zeros(len(P))]*len(P))
for i in range(len(P)):
  for j in range(0,len(P)-i):
    count = 0
    for k in range(i, len(P)-j):
      if(SP[k]==SP[j+k]):
        count += 1
      else:
        break
    LCP[i][j+i] = count
    LCP[j+i][i] = count

frag = np.zeros((TLength-PLength+1, PLength), dtype=int)
for i in range(TLength-PLength+1):
  for j in range(PLength):
    for k in range(PLength):
      if(ST[i][j]==SP[k]):
        frag[i][j] = k
        break
      else:
        frag[i][j] = SUPECIALCHAR

with open('./fragAnswer.csv', 'w') as f:
  for i in range(TLength-PLength+1):
    for j in range(PLength):
      f.write(str(frag[i][j]))
      f.write(',')
    f.write('\n')

