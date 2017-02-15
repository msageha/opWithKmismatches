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

int readFile(string fileName, vector<int> &input) {//�t�@�C���̓ǂݍ��݊֐�
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

void initializeSTi(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, vector<int> &Text, vector<int>::iterator it) { //STi�̏�����
  int index = (int)distance(Text.begin(), it); //���ݎQ�Ƃ��Ă���index��int�^�Ŏ擾 size_t����int�ɃL���X�g���Ă��邩�炠�܂�ǂ��Ȃ�
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

void changePredS(vector<int> &predTi, vector<int> &STi, int insertIndex, int insertNum) { //pred��S��ύX����Ƃ��ɌĂяo���֐��Dset�ɂ����č폜����C�e���[�^�Cpred�ɑ}������pair�������D
  predTi[insertIndex%P] = insertNum;
  if (insertNum != $) insertNum = insertNum - insertIndex; //pred��SpecialCharacter($)�̂Ƃ��͂��̂܂�
  STi[insertIndex%P] = insertNum;
}

void removePrefix(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, int index, int removeNum) { //T[index:index+P]�܂ł̕���������ɂ����āCT[index]���폜����
  auto it = TiMap.lower_bound(make_pair(removeNum, index));
  assert(it != TiMap.end() && it->first == removeNum && it->second == index);
  TiMap.erase(it++);
  if (it != TiMap.end()) { //�擪�̗v�f���w��pred������Ƃ��Ƃ������ƁDit�́CTi[index]�����P�傫���v�f���w���C�e���[�^
    changePredS(predTi, STi, (*it).second%P, predTi[index%P]);
  }
}

void insertSuffix(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, int index, int insertNum) { //T[index:index+P-2]�܂ł̕���������ɂ����āCT[index:index+P-1]�ɂ���
  auto tempPair = TiMap.insert(make_pair(insertNum, index + P - 1));
  auto prevIt = prev(tempPair.first);
  auto nextIt = next(tempPair.first);
  if (prevIt != TiMap.end()) { //T[index+P-1]����1�������v�f������ꍇ
    changePredS(predTi, STi, index + P - 1, (*prevIt).second);
  }
  else { //T[index+P-1]����1�������v�f���Ȃ������ꍇ
    changePredS(predTi, STi, index + P - 1, $);
  }
  if (nextIt != TiMap.end()) { //T[index+P-1]����1�傫���v�f������ꍇ
    changePredS(predTi, STi, (*nextIt).second, index + P - 1);
  }
}

void slideWindow(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, vector<int> &Text, vector<int>::iterator TextIt) { //T[index:index+P-1]��T[index+1:index+P]�ɂ���
  int index = (int)distance(Text.begin(), TextIt); //���ݎQ�Ƃ��Ă���index��int�^�Ŏ擾 size_t����int�ɃL���X�g���Ă��邩�炠�܂�ǂ��Ȃ�.
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
  initializeSTi(TiMap, predTi, STi, Text, it);//predTiMap, predTi, STi�̏�����
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