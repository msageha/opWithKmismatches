#パターンとテキストのpredとSを作り，csvを吐くスクリプト
import numpy as np
import csv

SUPECIALCHAR = 10001

with open('./PatternText/pattern.txt', 'r') as f:
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

with open('./PatternText/text.txt', 'r') as f:
  T = np.array(list(map(int, f.read().splitlines())))
TLength = len(T)

Tsub = np.zeros((TLength-PLength, PLength), dtype=int)
for i in range(TLength-PLength):
  for j in range(PLength):
    Tsub[i][j] = T[i+j]

predT = np.zeros((TLength-PLength, PLength), dtype=int)

for i in range(TLength-PLength):
  pred = SUPECIALCHAR
  for j in Tsub[i].argsort():
    predT[i][j] = pred
    pred = i+j

ST = np.zeros((TLength-PLength, PLength), dtype=int)
for i in range(TLength-PLength):
  for j in range(PLength):
    if(predT[i][j]==SUPECIALCHAR):
      ST[i][j] = predT[i][j]
    else:
      ST[i][j] = predT[i][j] - (i+j)

with open('./PatternText/predS.csv', 'w') as f:
  writer = csv.writer(f, lineterminator='')
  f.write('P,')
  writer.writerow(P)
  f.write('\npredP,')
  writer.writerow(predP)
  f.write('\nS(P),')
  writer.writerow(SP)
  f.write('\n')
  for i in range(TLength-PLength):
    f.write('\nT[{}]-T[{}],'.format(i, i+PLength-1))
    writer.writerow(Tsub[i])
    f.write('\npred(T[{}]-T[{}]),'.format(i, i+PLength-1))
    writer.writerow(predT[i])
    f.write('\nS(T[{}]-T[{}]),'.format(i, i+PLength-1))
    writer.writerow(ST[i])
  f.write('\n')
