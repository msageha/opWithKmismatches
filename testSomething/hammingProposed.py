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

#前処理の終了
fragment = SPの部分文字列(SP[i..i+k])と開始位置 or $(ない場合) 
fragmentArray = [fragment, ..., fragment]#フラグメントの集合．
def whichFrag(ST`[i]): ST`[i]が所属しているフラグメントのインデックスを返す．
  return fragmentArrayのインデックス

def changeFrag(i, l): #フラグメントのインデックスと，そのインデックスのl番目が間違っているという情報があたえられたときに，そのフラグメントを変更する
  fragment = fragmentArray[i]
  if len(fragment) == 1:
    fragment = 変えるフラグメントを代入
  elif len(fragment) == 2:
    if l==0:
      fragment[l] = 変えるフラグメントを代入 #SPの部分文字列1文字?(SP[i..i+k]) or $(ない場合) 
      fragment[l+1]をfragment[l]の後ろに挿入
    elif l==1:
      fragment[l-1]をfragment[l]の前に挿入
      fragment[l] = 変えるフラグメントを代入 #SPの部分文字列1文字?(SP[i..i+k]) or $(ない場合) 
  elif len(fragment) >= 3:
    fragment[1,...,l-1]をfragment[l]の前に挿入
    fragment[l] = 変えるフラグメントを代入 # SPの部分文字列1文字?(SP[i..i+k]) or $(ない場合) 
    fragment[l,...,last]をfragment[l]の後ろに挿入
  fragmentArray[i] = fragment #変えたものをfragmentArrayに反映してあげる
  return l #変更したインデックス

def connectFrag(i, j): #jは，SP[j] != ST'[j] となった位置, iは，jの前でSP[i] != ST'[i]となった位置
  fragment_i = #ST'[i]が所属しているフラグメント
  fragment_j = #ST'[j]が所属しているフラグメント
  fragment_i_index = fragment_iがあるfragmentArrayのindex #つまり，fragment_i==fragmentArray[fragment_i_index]
  fragment_j_index = fragment_jがあるfragmentArrayのindex #つまり，fragment_j==fragmentArray[fragment_j_index]
  changeFrag(i)
  changeFrag(j)
  if (fragment_j_index - fragment_i_index) >= 3:
    fragment_new = fragment_(i+1) + ... + fragment_(j+1)
  fragmentArray[fragment_i_index + 1] = fragment_new
  fragmentArrayにおいて，fragment_i_index + 2 から，fragment_jまでを削除

before_mistake_index = 0
def fragment_check(fragment, i): #フラグメントがあたえられたときに，そのフラグメントとそれに対応するSP[i..]のハミング距離を返す
  if fragment == $:
    return 1
  j = fragmentと共通しているSPの部分文字列の開始位置
  if LCP[i][j] == len(fragment) #フラグメントの長さとLCPが一致している時(つまり，フラグメント中にSP[i] != ST'[i] となるような間違いがない場合)
    return 0 #len(fragment) - LCP[i][j]
  k = LCP[i][j]+1 #kには，フラグメントとそれに対応するSP[i..]について何番目が間違っているか入っている．つまり，SP[i+k] != ST'[i+k]
  l = changeFrag(fragmentのインデックス, k) #間違いがあった場所のフラグメントのインデックス
  if (l - before_mistake_index) >= 3: #間違いがあったインデックスとその前の間違いの長さが３つ以上あった場合，フラグメントをつなげたい
    connectFrag(before_mistake_index, l)
  before_mistake_index = l
  sum = 0
  fragment_succ = fragmentのサクセッサー 
  fragment = fragmentArray[whichFrag(ST`[l])] #間違いがあった場所のフラグメント
  fragment_pred = fragmentのプレデセッサー
  index = i+len(fragment_pred)
  for frag in range(fragment, fragment_succ):
    sum += fragment_check(frag, index)
    index += len(frag)
  return k+sum

index = 0 #どこまでのインデックスについてチェックが進んだかを示す変数
for i in range(fragmentArray):
  fragment_check(i, index)
  index += len(i) #チェックが終わったフラグメントの長さを入れてあげる

