#include <assert.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#define P 10
#define T 100000
#define $ T+1

using namespace std;

int readFile(string fileName, vector<int> &input) {//ファイルの読み込み関数
  cout << "read" << fileName << " file" << endl;
  ifstream file;
  file.open(fileName);
  if (file.is_open()) {
    for (int i = 0; i<input.size(); i++) file >> input[i];
  }
  else { return 1; }
  file.close();
  return 0;
}

void printVector(vector<int> &input, int startIndex = 0) {
  for (int i = startIndex; i<input.size() + startIndex; i++) cout << input[i%P] << "\t";
  cout << endl;
}

void printMap(map<int, int> &mp) {
  for (auto p : mp) cout << "first=" << p.first << "\tsecond=" << p.second << endl;
}

void printSetPair(set<pair<int, int>> &setPair) {
  for (auto p : setPair) cout << "p.first=" << p.first << "\tp.second=" << p.second << endl;
}

void initializeSTi(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, vector<int> &Text, vector<int>::iterator it) { //STiの初期化
  int index = (int)distance(Text.begin(), it); //現在参照しているindexをint型で取得 size_tからintにキャストしているからあまり良くない
  cout << "index=" << index << endl;
  for (int i = index; i<index + P; i++) TiMap.insert(make_pair(Text[i], i));
  int pred = $;
  for (auto x : TiMap) {
    predTi[x.second%P] = pred;
    pred = x.second;
  }
  for (int i = 0; i<P; i++) {
    if (predTi[i] == $) STi[i] = $;
    else STi[i] = predTi[i] - i - index;
  }
}

void changePredS(vector<int> &predTi, vector<int> &STi, int insertIndex, int insertNum) { //predとSを変更するときに呼び出す関数．setにおいて削除するイテレータ，predに挿入するpairも入れる．
  predTi[insertIndex%P] = insertNum;
  if (insertNum != $) insertNum = insertNum - insertIndex; //predがSpecialCharacter($)のときはそのまま
  STi[insertIndex%P] = insertNum;
}

void removePrefix(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, int index, int removeNum) { //T[index:index+P]までの部分文字列において，T[index]を削除する
  auto it = TiMap.lower_bound(make_pair(removeNum, index));
  assert(it != TiMap.end() && it->first == removeNum && it->second == index);
  TiMap.erase(it++);
  if (it != TiMap.end()) { //先頭の要素を指すpredがあるときということ．itは，Ti[index]よりも１つ大きい要素を指すイテレータ
    changePredS(predTi, STi, (*it).second%P, predTi[index%P]);
  }
}

void insertSuffix(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, int index, int insertNum) { //T[index:index+P-2]までの部分文字列において，T[index:index+P-1]にする
  auto tempPair = TiMap.insert(make_pair(insertNum, index + P - 1));
  auto prevIt = prev(tempPair.first);
  auto nextIt = next(tempPair.first);
  if (prevIt != TiMap.end()) { //T[index+P-1]よりも1つ小さい要素がある場合
    changePredS(predTi, STi, index + P - 1, (*prevIt).second);
  }
  else { //T[index+P-1]よりも1つ小さい要素がなかった場合
    changePredS(predTi, STi, index + P - 1, $);
  }
  if (nextIt != TiMap.end()) { //T[index+P-1]よりも1つ大きい要素がある場合
    changePredS(predTi, STi, (*nextIt).second, index + P - 1);
  }
}

void slideWindow(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, vector<int> &Text, vector<int>::iterator TextIt) { //T[index:index+P-1]をT[index+1:index+P]にする
  int index = (int)distance(Text.begin(), TextIt); //現在参照しているindexをint型で取得 size_tからintにキャストしているからあまり良くない.
  removePrefix(TiMap, predTi, STi, index, Text[index]);
  insertSuffix(TiMap, predTi, STi, index + 1, Text[index + P]);
}

int main() {
  // vector<int> Pattern(P);
  // if(readFile("./../PatternText/pattern.txt", P)==1) return 1;
  vector<int> Text(T);
  if (readFile("../../PatternText/text.txt", Text) == 1) return 1;
  set<pair<int, int>> TiMap;
  vector<int> predTi(P);
  vector<int> STi(P);
  auto it = Text.begin();
  initializeSTi(TiMap, predTi, STi, Text, it);//predTiMap, predTi, STiの初期化
  printVector(predTi);
  for (int i = 0; i<T - P; i++) {
    slideWindow(TiMap, predTi, STi, Text, it);
    printVector(predTi, i + 1);
    it++;
  }

  // auto it = Text.begin();
  // for(int i=0; i<T-P+1; i++) {
  //   initializeSTi(predTiMap, predTi, STi, Text, it);
  //   printVector(predTi);
  //   cout << "\n" << endl;
  //   predTiMap.clear();
  //   it++;
  // }


  return 0;
}