#パターンとテキストの各部分文字列についてナイーブにハミング距離を計算して，K以下のインデックスを吐くスクリプト
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

correctIndex = np.zeros((TLength-PLength+1), dtype=int) #ハミング距離が3K以下のインデックス
#前処理終わり
start = time.time()
for i in range(TLength-PLength+1):
  mistakesNum = 0
  for j in range(PLength):
    if(ST[i][j] != SP[j]):
      mistakesNum += 1
    if mistakesNum>3*K:
      break
  if mistakesNum <= 3*K:
    print ('correctIndex={}'.format(i))
    correctIndex[i] = 1
finish = time.time()
print('time: {}sec'.format(finish-start))
