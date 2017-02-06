#任意の２つのインデックスからの部分文字列について，最長共通部分文字列の長さを返す
import numpy as np

with open('../PatternText/pattern.txt', 'r') as f:
  P = np.array(list(map(int, f.read().splitlines())))
predV = np.zeros(len(P), dtype=int)
pred = len(P)+1
for i in P.argsort():
  predV[i] = pred
  pred = i

S = np.zeros(len(P), dtype=int)
for i in range(len(P)):
  if(predV[i]==len(P)+1):
    S[i] = predV[i]
  else:
    S[i] = predV[i] - i

LCS = np.array([np.zeros(len(P))]*len(P), dtype=int)

for i in range(len(P)):
  for j in range(0,len(P)-i):
    count = 0
    for k in range(i, len(P)-j):
      if(S[k]==S[j+k]):
        count += 1
      else:
        break
    LCS[i][j+i] = count
    LCS[j+i][i] = count

with open('LCS.csv', 'w') as f:
  for i in range(len(P)):
    for j in range(len(P)):
      f.write(str(LCS[i][j]))
      f.write('\t')
    f.write('\n')