#パターンとテキストの各部分文字列についてナイーブにハミング距離を計算して，K以下のインデックスを吐くスクリプト
import numpy as np
import time

SUPECIALCHAR = -100001
K =  #mistakesNum

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

with open('hammingAnswerK={}.txt'.format(K), 'r') as f:
  hammingAnswer = np.array(list(map(int, f.read().splitlines())))

answer = []
for candidate in hammingAnswer:
  dTmp = []
  elementTmp = [] #Tsubの要素のtemp
  I = [] #weightの配列
  for i in range(PLength):
    if SP[i] != ST[candidate][i]:
      dTmp.append(i)
      elementTmp.append(Tsub[candidate][i])
    elif SP[i] == SUPECIALCHAR and ST[candidate][i] == SUPECIALCHAR:
      dTmp.append(i)
      elementTmp.append(Tsub[candidate][i])
  dTmp = np.array(dTmp)
  elementTmp = np.array(elementTmp)
  d = dTmp[elementTmp.argsort()]
  a_dash = P[d]
  j = 0 #dの順番を管理している変数
  weight = 0
  beforeA = -1 #前の数字を保存しておく
  beforeB = -1
  for i in Tsub[candidate].argsort():
    if SP[i] == ST[candidate][i]:
      if ((beforeA < P[i] and beforeB == Tsub[candidate][i])
        or (beforeA == P[i] and beforeB < Tsub[candidate][i])):
        pass
      else:
        weight += 1
        beforeA = P[i]
        beforeB = Tsub[candidate][i]
    else: #異なっていた時
      if i != Tsub[candidate].argsort()[0]: #最初ではない時
        I.append(weight)
      weight = 1
      beforeA = -1 #前の数字の初期化
      beforeB = -1
  I.append(weight) #1番最後を加える
  I = np.array(I)
  with open('weightedLCS(K={}).txt'.format(K), 'a') as f:
    f.write('index:{}\n'.format(candidate))
    f.write('a:')
    for i in a_dash:
      f.write(str(i)+'\t')
    f.write('\nI:')
    for i in I:
      f.write(str(i)+'\t')
    f.write('\n')

  #重み付き最長共通部分列
  v = np.zeros(PLength, dtype=int)
  P_th = np.zeros(PLength, dtype=int) #ベクトルを値の最大値分，確保したくないため．何番目に大きいか
  NumTmp = -1
  IndexTmp = P.argsort()[0]
  for i in range(len(P)):
    if NumTmp != P[P.argsort()[i]]:
      NumTmp = P[P.argsort()[i]]
      IndexTmp = i
    P_th[P.argsort()[i]] = IndexTmp
  a_dash_th = P_th[d]
  for i in range(len(a_dash_th)):
    if a_dash_th[i]==0:
      v[a_dash_th[i]] = max(v[0], I[i])
    else:
      v[a_dash_th[i]] = max(v[a_dash_th[i]],max(v[0:a_dash_th[i]])+I[i])
  if max(v) >= PLength-K:
    answer.append(candidate)

for i in range(len(answer)):
  print(str(answer[i])+'\t')
