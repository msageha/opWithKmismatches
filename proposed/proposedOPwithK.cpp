#include <algorithm>
#include <bitset>
#include <cassert>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#define DEBUG 

using namespace std;

int m, n, K, SPECIALCHAR; //使用グローバル変数
//プロトタイプ宣言
void changePredS(vector<int> &predTi, vector<int> &STi, int insertIndex, int insertNum, map<int, int> &fragMap, map<int, int> SPValueIndex);
void removePrefix(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, int index, int removeNum, map<int, int> &fragMap, map<int, int> SPValueIndex);
void insertSuffix(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, int index, int insertNum, map<int, int> &fragMap, map<int, int> SPValueIndex);
void slideWindow(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, vector<int> &Text, vector<int>::iterator TextIt, map<int, int> &fragMap, map<int, int> &SPValueIndex);
int FragHDistance(int index, int fragIndex, int fragLen, vector<vector<int>> &LCP);
int FragLength(map<int, int> &mp, map<int, int>::iterator it);
bool HammingUnder3K(map<int, int> &mp, vector<int> &SP, vector<vector<int>> &LCP);
int insertValueSTi(map<int, int> &SPValueIndex, int value);
void ChangeFrag(map<int, int> &mp, int index, int insertNum);
void slideFrag(map<int, int> &mp, int index, int insertNum);
void makeWeightedLCS(vector<int> &a, vector<int> &relative_a, set<pair<int, int>> &setb, vector<int> &Sa, vector<int> &Sb, int index, vector<int> &a_dash, vector<int> &I);
int calculateWeightedLCS(vector<int> &a_dash, vector<int> &I);
bool isomorephicKmismatches(vector<int> &pattern, vector<int> &relativeP, vector<int> &SP, vector<vector<int>> &LCP, vector<int> &STi, set<pair<int, int>> &TiSet, map<int, int> &fragMap, int index);

//ファイルを読み込んでくる関数
int readfile(string fileName, vector<int> &input) { //ファイルの読み込み関数
  //cout << "read " << fileName << " file" << endl;
  ifstream file;
  string tmp;
  file.open(fileName);
  if (file.is_open()) {
    while(getline(file, tmp)) {
      try { input.push_back(stoi(tmp)); }
      catch (invalid_argument e) { return -1; }
    }
  }
  else { return -1; }
  file.close();
  return 0;
}
int readCSV(string fileName, vector<vector<int> > &input) { //STchangeIndex,LCSを読み込む関数
  //cout << "read" << fileName << " file" << endl;
  ifstream file;
  stringstream ss;
  string tmp;
  file.open(fileName);
  if (file.is_open()) {
    for (int i = 0; i<input.size(); i++) {
      getline(file, tmp);
      ss << tmp;
      for (int j = 0; j<input[0].size(); j++) ss >> input[i][j];
    }
  }
  else { return 1; }
  file.close();
  return 0;
}

//デバッグ用プリント関数
void printMatrix(vector<vector<int> > &input) {
  for (int i = 0; i<input.size(); i++) {
    for (int j = 0; j<input[0].size(); j++) cout << input[i][j] << "\t";
    cout << endl;
  }
}
void printVector(vector<int> &input, int startIndex = 0) {
  for (int i = startIndex; i<input.size() + startIndex; i++) cout << input[i%m] << "\t";
  cout << endl;
}
void printMap(map<int, int> &mp) {
  for (auto p : mp) cout << "first=" << p.first << "\tsecond=" << p.second << endl;
}
void printSetPair(set<pair<int, int>> &setPair) {
  for (auto p : setPair) cout << "first=" << p.first << "\tsecond=" << p.second << endl;
}

//ウィンドウをずらしていくための関数
void changePredS(vector<int> &predTi, vector<int> &STi, int insertIndex, int insertNum, map<int, int> &fragMap, map<int, int> SPValueIndex) { //predとSを変更するときに呼び出す関数．setにおいて削除するイテレータ，predに挿入するpairも入れる．insertIndexはPのmodを取っていない値を期待している
  predTi[insertIndex%m] = insertNum;
  if (insertNum != SPECIALCHAR) insertNum = insertNum - insertIndex; //predがSpecialCharacter($)のときはそのまま
  STi[insertIndex%m] = insertNum;
  int insert = insertValueSTi(SPValueIndex, STi[insertIndex%m]); //fragも変更する必要があるため
  ChangeFrag(fragMap, insertIndex, insert);
}
void removePrefix(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, int index, int removeNum, map<int, int> &fragMap, map<int, int> SPValueIndex) { //T[index:index+P]までの部分文字列において，T[index]を削除する
  auto it = TiMap.lower_bound(make_pair(removeNum, index));
  assert(it != TiMap.end() && it->first == removeNum && it->second == index);
  TiMap.erase(it++);
  if (it != TiMap.end()) { //先頭の要素を指すpredがあるときということ．itは，Ti[index]よりも１つ大きい要素を指すイテレータ
    changePredS(predTi, STi, (*it).second, predTi[index%m], fragMap, SPValueIndex);
  }
}
void insertSuffix(set<pair<int, int>> &TiSet, vector<int> &predTi, vector<int> &STi, int index, int insertNum, map<int, int> &fragMap, map<int, int> SPValueIndex) { //T[index:index+P-2]までの部分文字列において，T[index:index+P-1]にする
  auto tempPair = TiSet.insert(make_pair(insertNum, index + m - 1));
  auto prevIt = TiSet.end();
  if (tempPair.first != TiSet.begin()) prevIt = prev(tempPair.first);
  auto nextIt = next(tempPair.first);
  if (prevIt != TiSet.end()) { //T[index+P-1]よりも1つ小さい要素がある場合
    changePredS(predTi, STi, index + m - 1, (*prevIt).second, fragMap, SPValueIndex);
  }
  else { //T[index+P-1]よりも1つ小さい要素がなかった場合
    changePredS(predTi, STi, index + m - 1, SPECIALCHAR, fragMap, SPValueIndex);
  }
  if (nextIt != TiSet.end()) { //T[index+P-1]よりも1つ大きい要素がある場合
    changePredS(predTi, STi, (*nextIt).second, index + m - 1, fragMap, SPValueIndex);
  }
}
void slideWindow(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, vector<int> &Text, vector<int>::iterator TextIt, map<int, int> &fragMap, map<int, int> &SPValueIndex) { //T[index:index+P-1]をT[index+1:index+P]にする
  int index = (int)distance(Text.begin(), TextIt); //現在参照しているindexをint型で取得 size_tからintにキャストしているからあまり良くない.
  removePrefix(TiMap, predTi, STi, index, Text[index], fragMap, SPValueIndex);

  insertSuffix(TiMap, predTi, STi, index + 1, Text[index + m], fragMap, SPValueIndex);
  int insertNum = insertValueSTi(SPValueIndex, STi[(index + m) % m]);
  slideFrag(fragMap, index, insertNum);
}

//SPとSTiのハミング距離をフラグメントを使って求める関数
int FragHDistance(int index, int fragIndex, int fragLen, vector<vector<int>> &LCP) { //フラグメント(SP[fragIndex:])と長さと，現在のみているインデックスSP[index:]が入力されたときに，SP[j:]とSP[index:]のH距離を返す関数
  if (fragIndex == SPECIALCHAR && fragLen>1) return 1 + FragHDistance(index + 1, fragIndex + 1, fragLen - 1, LCP);
  if (fragIndex == SPECIALCHAR && fragLen == 1) return 1;
  if (LCP[index][fragIndex] >= fragLen) return 0;
  if (fragLen == 1) return 1;
  int Hamming = 1;
  int mismatchIndex = LCP[index][fragIndex]; //間違えがあったインデックス
  return Hamming + FragHDistance(index + mismatchIndex + 1, fragIndex + mismatchIndex + 1, fragLen - mismatchIndex - 1, LCP);
}
int FragLength(map<int, int> &mp, map<int, int>::iterator it) {//マップとイテレータがあたえられたときにフラグメントの長さを返す関数
                                                               //最低でも1が返される
  auto itSuc = next(it);
  if (itSuc == mp.end()) return (*(mp.begin())).first + m - (*it).first;
  else return (*itSuc).first - (*it).first;
}
bool HammingUnder3K(map<int, int> &mp, vector<int> &SP, vector<vector<int>> &LCP) { //フラグメントが与えられた時ハミング距離を返す関数
  int beforeMismatchIndex = 0;
  int fragCountBetweetnMismatches = 0; //ミスマッチ間のフラグメントの数の管理
  int index = 0; //SP[index:]とLCPを比べるためにどこから比べるかを管理する変数
  int hamming = 0;
  for (auto it = mp.begin(); it != mp.end(); it++) {
    int fragLen = FragLength(mp, it);
    int FragHamming = FragHDistance(index, (*it).second, fragLen, LCP);
    //デバッグ用の処理
    // if((*(mp.begin())).first==311) {
    //   cout << "it.first=" << it->first << "\tit.second" << it->second << "\tFragHamming=" << FragHamming << "\t" << endl;
    // }
    //ここまで
    if (FragHamming == 0) {
      (*it).second = index;
      fragCountBetweetnMismatches++;
    }
    else { //不一致があったときの処理
      if (fragCountBetweetnMismatches >= 3) { //ミスマッチの間のフラグメントの個数の判定．論文のfig2を参照
                                              //フラグメントをconcatenateする
        auto itTmp = prev(it);
        for (int i = 1; i<fragCountBetweetnMismatches; i++) mp.erase(itTmp--);
      }
      fragCountBetweetnMismatches = 0; //ミスマッチ間のフラグメントの数を初期化
    }
    index += fragLen; //インデックスをフラグメントの長さ分すすめる
    hamming += FragHamming;
    //デバッグ用の処理
    //if((*(mp.begin())).first==311) cout << "hamming=" << hamming << endl;
    //ここまで
    if (hamming > 3 * K) return false;
  }
  return true;
}
int insertValueSTi(map<int, int> &SPValueIndex, int value) {
  auto SPit = SPValueIndex.find(value);
  if (SPit == SPValueIndex.end()) return SPECIALCHAR;
  else return (*SPit).second;
}
void ChangeFrag(map<int, int> &mp, int index, int insertNum) { //STi[index]が変化した時に，それに対応するフラグメントを変える関数 indexはmodを取らない値を期待している
  if ((*mp.begin()).first + m - 1 < index) index -= m; //ここバグになりそう。飛んでくるindexの値に注意
  auto it = mp.lower_bound(index);
  if ((*it).first != index) { //キーが無かった時
    it--;
    int FragLen = FragLength(mp, it);
    int l = index - (*it).first; //STi[index]は，所属しているフラグメント内のl番目になる
    mp[index] = insertNum;
    if (l + 1 != FragLen) { //w[l+index+1:]も入れる必要がある
      mp[index + 1] = (*it).second + l + 1;
    }
  }
  else {
    int FragLen = FragLength(mp, it);
    if (FragLen != 1) { //|w[0]|=1と，w[1:]に分ける必要がある時(w[0]は，mp[index]に当たる)
      mp[index + 1] = (*it).second + 1;
    }
    (*it).second = insertNum; //mp[index] = insertNum;と同じ処理．こっちのほうが早いため
  }
}
void slideFrag(map<int, int> &mp, int index, int insertNum) { //ウィンドウをずらす時(iをインクリメントする時)に，対応するように，mapを書き換える
  auto it = mp.begin();
  if (FragLength(mp, it) != 1) mp[index + 1] = (*it).second + 1;
  mp.erase(it);
  mp[index + m] = insertNum; //mpにSTi[index+P]にあたるフラグメントを挿入
}

//重み付きLCSにてOPwithKmismatchかどうかをチェックする関数
void makeWeightedLCS(vector<int> &a, vector<int> &relative_a, set<pair<int, int>> &setb, vector<int> &Sa, vector<int> &Sb, int index, vector<int> &a_dash, vector<int> &I) { //Iはweightのvector
  vector<int> d;
  int weight = 0;
  int predNum_a = -1;
  int predNum_b = -1;
  for (auto x : setb) {
    if (Sa[x.second-index] == Sb[x.second%m]) { //Sb[((x.second-index)+index)%P]を表している。index分ズレているx.secondをズレを修正し、さらにindex分配列のスタート位置がズレているため
      if (x == (*setb.begin())) { //1番最小の要素が等しかった場合(d0に当たる)
        d.push_back(x.second-index);
        a_dash.push_back(relative_a[x.second-index]);
      }
      if ((predNum_a<a[x.second - index] && predNum_b == x.first)
        || (predNum_a == a[x.second - index] && predNum_b<x.first)) {
      }
      else {
        weight++;
        predNum_a = a[x.second - index];
        predNum_b = x.first;
      }
    }
    else {
      d.push_back(x.second - index);
      a_dash.push_back(relative_a[x.second - index]);
      if (x != (*setb.begin())) I.push_back(weight); //1番最初の要素が異なっているわけではない時
      weight = 1;
      predNum_a = -1;
      predNum_b = -1;
    }
  }
  I.push_back(weight);
}
int calculateWeightedLCS(vector<int> &a_dash, vector<int> &I) {
  vector<int> v(m);
  for (int i = 0; i<I.size(); i++) {
    int temp = 0;
    if (a_dash[i] != 0) temp = *max_element(v.begin(), v.begin() + a_dash[i]);
    v[a_dash[i]] = max(v[a_dash[i]], temp + I[i]);
  }
  return *max_element(v.begin(), v.end());
}

//各変数を初期化する前処理
void initializeSTi(set<pair<int, int>> &TiSet, vector<int> &predTi, vector<int> &STi, vector<int> &Text, vector<int>::iterator it) { //STiの初期化
  int index = (int)distance(Text.begin(), it); //現在参照しているindexをint型で取得 size_tからintにキャストしているからあまり良くない
  for (int i = index; i<index + m; i++) TiSet.insert(make_pair(Text[i], i));
  int pred = SPECIALCHAR;
  for (auto x : TiSet) {
    predTi[x.second%m] = pred;
    pred = x.second;
  }
  for (int i = 0; i<m; i++) {
    if (predTi[i] == SPECIALCHAR) STi[i] = SPECIALCHAR;
    else STi[i] = predTi[i] - i - index;
  }
}
void initializeSP(vector<int> &SP, vector<int> &pattern) {
  set<pair<int, int>> P_Set;
  vector<int> predP(m);
  for (int i = 0; i<m; i++) P_Set.insert(make_pair(pattern[i], i));
  int pred = SPECIALCHAR;
  for (auto x : P_Set) {
    assert(x.second<m);
    predP[x.second] = pred; //x.secondは必ずP以下である必要がある．
    pred = x.second;
  }
  for (int i = 0; i<m; i++) {
    if (predP[i] == SPECIALCHAR) SP[i] = SPECIALCHAR;
    else SP[i] = predP[i] - i;
  }
}
void makeRelativeP(vector<int> &pattern, vector<int> &relativeP) {
  vector<int> sortedP(pattern);
  assert(sortedP.size()>0);
  copy(pattern.begin(), pattern.end(), sortedP.begin()); //sortedPのサイズを指定していないと範囲外アクセスする．
  sort(sortedP.begin(), sortedP.end());
  for (int i = 0; i<pattern.size(); i++) {
    for (int j = 0; j<pattern.size(); j++) {
      if (sortedP[i] == pattern[j]) relativeP[j] = i;
    }
  }
}
void initializeFrag(map<int, int> &mp, vector<int> &SP, vector<int> &STi) { //フラグメントの初期化関数
  int j = 0;
  for (int i = 0; i<SP.size(); i++) {
    for (j = 0; j<SP.size(); j++) {
      if (STi[i] == SP[j]) {
        mp[i] = j;
        break;
      }
    }
    if (j == SP.size()) mp[i] = SPECIALCHAR;
  }
}
void initializeSPValueIndexMap(map<int, int> &mp, vector<int> &SP) { //SPに対応するMapを作成するための関数
  for (int i = 0; i<SP.size(); i++) {
    auto it = mp.find(SP[i]);
    if (it == mp.end()) {
      mp[SP[i]] = i;
    }
  }
}

void OPwithKmismatches() {
  //ファイルの読み込み、及び、初期化作業
  vector<int> pattern;
  vector<int> text;
  readfile("pattern.txt", pattern);
  readfile("text.txt", text);
  m = pattern.size();
  n = text.size();
  K = 0;
  SPECIALCHAR = -n - 1;
  vector<int> SP(m);
  initializeSP(SP, pattern);
  map<int, int> SPValueIndex;
  initializeSPValueIndexMap(SPValueIndex, SP);
  vector<vector<int>> LCP(m, vector<int>(m));
  readCSV("LCP.csv", LCP);
  vector<int> relativeP(m);
  makeRelativeP(pattern, relativeP);
  //初期化終了
  for (K = 0; K<m/3; K++) {
    //新しく始めるたびに初期化する必要があるもの
    set<pair<int, int>> TiSet;
    vector<int> predTi(m);
    vector<int> STi(m);
    auto it = text.begin();
    initializeSTi(TiSet, predTi, STi, text, it); //TiMap, predTi, STiの初期化
    map<int, int> fragMap;
    initializeFrag(fragMap, SP, STi);
    //計測スタート
    clock_t start = clock();
    int index = 0;
    if (isomorephicKmismatches(pattern, relativeP, SP, LCP, STi, TiSet, fragMap, index));// DEBUG cout << index << endl;
    for (index = 1; index <= n - m; index++) {
      slideWindow(TiSet, predTi, STi, text, it, fragMap, SPValueIndex);
      if (isomorephicKmismatches(pattern, relativeP, SP, LCP, STi, TiSet, fragMap, index));// DEBUG cout << index << endl;
      it++;
    }
    clock_t finish = clock();
    cout << "K=" << K << "\ttime: " << (double)(finish - start) / CLOCKS_PER_SEC << "sec" << endl;
  }
}
bool isomorephicKmismatches(vector<int> &pattern, vector<int> &relativeP, vector<int> &SP, vector<vector<int>> &LCP, vector<int> &STi, set<pair<int, int>> &TiSet, map<int, int> &fragMap, int index) {
  if (HammingUnder3K(fragMap, SP, LCP)) {
    vector<int> a_dash;
    vector<int> I;
    makeWeightedLCS(pattern, relativeP, TiSet, SP, STi, index, a_dash, I);
    if (calculateWeightedLCS(a_dash, I) >= m - K) return true; //答えの出力
  }
  return false;
}


int main() {
  OPwithKmismatches();
  return 0;
}