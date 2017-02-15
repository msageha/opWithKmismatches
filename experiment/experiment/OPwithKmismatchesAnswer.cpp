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

int m, n, K, SPECIALCHAR; //�g�p�O���[�o���ϐ�
//�v���g�^�C�v�錾
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

//�t�@�C����ǂݍ���ł���֐�
int readfile(string fileName, vector<int> &input) { //�t�@�C���̓ǂݍ��݊֐�
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
int readCSV(string fileName, vector<vector<int> > &input) { //STchangeIndex,LCS��ǂݍ��ފ֐�
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

//�f�o�b�O�p�v�����g�֐�
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

//�E�B���h�E�����炵�Ă������߂̊֐�
void changePredS(vector<int> &predTi, vector<int> &STi, int insertIndex, int insertNum, map<int, int> &fragMap, map<int, int> SPValueIndex) { //pred��S��ύX����Ƃ��ɌĂяo���֐��Dset�ɂ����č폜����C�e���[�^�Cpred�ɑ}������pair�������DinsertIndex��P��mod������Ă��Ȃ��l�����҂��Ă���
  predTi[insertIndex%m] = insertNum;
  if (insertNum != SPECIALCHAR) insertNum = insertNum - insertIndex; //pred��SpecialCharacter($)�̂Ƃ��͂��̂܂�
  STi[insertIndex%m] = insertNum;
  int insert = insertValueSTi(SPValueIndex, STi[insertIndex%m]); //frag���ύX����K�v�����邽��
  ChangeFrag(fragMap, insertIndex, insert);
}
void removePrefix(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, int index, int removeNum, map<int, int> &fragMap, map<int, int> SPValueIndex) { //T[index:index+P]�܂ł̕���������ɂ����āCT[index]���폜����
  auto it = TiMap.lower_bound(make_pair(removeNum, index));
  assert(it != TiMap.end() && it->first == removeNum && it->second == index);
  TiMap.erase(it++);
  if (it != TiMap.end()) { //�擪�̗v�f���w��pred������Ƃ��Ƃ������ƁDit�́CTi[index]�����P�傫���v�f���w���C�e���[�^
    changePredS(predTi, STi, (*it).second, predTi[index%m], fragMap, SPValueIndex);
  }
}
void insertSuffix(set<pair<int, int>> &TiSet, vector<int> &predTi, vector<int> &STi, int index, int insertNum, map<int, int> &fragMap, map<int, int> SPValueIndex) { //T[index:index+P-2]�܂ł̕���������ɂ����āCT[index:index+P-1]�ɂ���
  auto tempPair = TiSet.insert(make_pair(insertNum, index + m - 1));
  auto prevIt = TiSet.end();
  if (tempPair.first != TiSet.begin()) prevIt = prev(tempPair.first);
  auto nextIt = next(tempPair.first);
  if (prevIt != TiSet.end()) { //T[index+P-1]����1�������v�f������ꍇ
    changePredS(predTi, STi, index + m - 1, (*prevIt).second, fragMap, SPValueIndex);
  }
  else { //T[index+P-1]����1�������v�f���Ȃ������ꍇ
    changePredS(predTi, STi, index + m - 1, SPECIALCHAR, fragMap, SPValueIndex);
  }
  if (nextIt != TiSet.end()) { //T[index+P-1]����1�傫���v�f������ꍇ
    changePredS(predTi, STi, (*nextIt).second, index + m - 1, fragMap, SPValueIndex);
  }
}
void slideWindow(set<pair<int, int>> &TiMap, vector<int> &predTi, vector<int> &STi, vector<int> &Text, vector<int>::iterator TextIt, map<int, int> &fragMap, map<int, int> &SPValueIndex) { //T[index:index+P-1]��T[index+1:index+P]�ɂ���
  int index = (int)distance(Text.begin(), TextIt); //���ݎQ�Ƃ��Ă���index��int�^�Ŏ擾 size_t����int�ɃL���X�g���Ă��邩�炠�܂�ǂ��Ȃ�.
  removePrefix(TiMap, predTi, STi, index, Text[index], fragMap, SPValueIndex);

  insertSuffix(TiMap, predTi, STi, index + 1, Text[index + m], fragMap, SPValueIndex);
  int insertNum = insertValueSTi(SPValueIndex, STi[(index + m) % m]);
  slideFrag(fragMap, index, insertNum);
}

//SP��STi�̃n�~���O�������t���O�����g���g���ċ��߂�֐�
int FragHDistance(int index, int fragIndex, int fragLen, vector<vector<int>> &LCP) { //�t���O�����g(SP[fragIndex:])�ƒ����ƁC���݂݂̂Ă���C���f�b�N�XSP[index:]�����͂��ꂽ�Ƃ��ɁCSP[j:]��SP[index:]��H������Ԃ��֐�
  if (fragIndex == SPECIALCHAR && fragLen>1) return 1 + FragHDistance(index + 1, fragIndex + 1, fragLen - 1, LCP);
  if (fragIndex == SPECIALCHAR && fragLen == 1) return 1;
  if (LCP[index][fragIndex] >= fragLen) return 0;
  if (fragLen == 1) return 1;
  int Hamming = 1;
  int mismatchIndex = LCP[index][fragIndex]; //�ԈႦ���������C���f�b�N�X
  return Hamming + FragHDistance(index + mismatchIndex + 1, fragIndex + mismatchIndex + 1, fragLen - mismatchIndex - 1, LCP);
}
int FragLength(map<int, int> &mp, map<int, int>::iterator it) {//�}�b�v�ƃC�e���[�^����������ꂽ�Ƃ��Ƀt���O�����g�̒�����Ԃ��֐�
                                                               //�Œ�ł�1���Ԃ����
  auto itSuc = next(it);
  if (itSuc == mp.end()) return (*(mp.begin())).first + m - (*it).first;
  else return (*itSuc).first - (*it).first;
}
bool HammingUnder3K(map<int, int> &mp, vector<int> &SP, vector<vector<int>> &LCP) { //�t���O�����g���^����ꂽ���n�~���O������Ԃ��֐�
  int beforeMismatchIndex = 0;
  int fragCountBetweetnMismatches = 0; //�~�X�}�b�`�Ԃ̃t���O�����g�̐��̊Ǘ�
  int index = 0; //SP[index:]��LCP���ׂ邽�߂ɂǂ������ׂ邩���Ǘ�����ϐ�
  int hamming = 0;
  for (auto it = mp.begin(); it != mp.end(); it++) {
    int fragLen = FragLength(mp, it);
    int FragHamming = FragHDistance(index, (*it).second, fragLen, LCP);
    //�f�o�b�O�p�̏���
    // if((*(mp.begin())).first==311) {
    //   cout << "it.first=" << it->first << "\tit.second" << it->second << "\tFragHamming=" << FragHamming << "\t" << endl;
    // }
    //�����܂�
    if (FragHamming == 0) {
      (*it).second = index;
      fragCountBetweetnMismatches++;
    }
    else { //�s��v���������Ƃ��̏���
      if (fragCountBetweetnMismatches >= 3) { //�~�X�}�b�`�̊Ԃ̃t���O�����g�̌��̔���D�_����fig2���Q��
                                              //�t���O�����g��concatenate����
        auto itTmp = prev(it);
        for (int i = 1; i<fragCountBetweetnMismatches; i++) mp.erase(itTmp--);
      }
      fragCountBetweetnMismatches = 0; //�~�X�}�b�`�Ԃ̃t���O�����g�̐���������
    }
    index += fragLen; //�C���f�b�N�X���t���O�����g�̒����������߂�
    hamming += FragHamming;
    //�f�o�b�O�p�̏���
    //if((*(mp.begin())).first==311) cout << "hamming=" << hamming << endl;
    //�����܂�
    if (hamming > 3 * K) return false;
  }
  return true;
}
int insertValueSTi(map<int, int> &SPValueIndex, int value) {
  auto SPit = SPValueIndex.find(value);
  if (SPit == SPValueIndex.end()) return SPECIALCHAR;
  else return (*SPit).second;
}
void ChangeFrag(map<int, int> &mp, int index, int insertNum) { //STi[index]���ω��������ɁC����ɑΉ�����t���O�����g��ς���֐� index��mod�����Ȃ��l�����҂��Ă���
  if ((*mp.begin()).first + m - 1 < index) index -= m; //�����o�O�ɂȂ肻���B���ł���index�̒l�ɒ���
  auto it = mp.lower_bound(index);
  if ((*it).first != index) { //�L�[������������
    it--;
    int FragLen = FragLength(mp, it);
    int l = index - (*it).first; //STi[index]�́C�������Ă���t���O�����g����l�ԖڂɂȂ�
    mp[index] = insertNum;
    if (l + 1 != FragLen) { //w[l+index+1:]�������K�v������
      mp[index + 1] = (*it).second + l + 1;
    }
  }
  else {
    int FragLen = FragLength(mp, it);
    if (FragLen != 1) { //|w[0]|=1�ƁCw[1:]�ɕ�����K�v�����鎞(w[0]�́Cmp[index]�ɓ�����)
      mp[index + 1] = (*it).second + 1;
    }
    (*it).second = insertNum; //mp[index] = insertNum;�Ɠ��������D�������̂ق�����������
  }
}
void slideFrag(map<int, int> &mp, int index, int insertNum) { //�E�B���h�E�����炷��(i���C���N�������g���鎞)�ɁC�Ή�����悤�ɁCmap������������
  auto it = mp.begin();
  if (FragLength(mp, it) != 1) mp[index + 1] = (*it).second + 1;
  mp.erase(it);
  mp[index + m] = insertNum; //mp��STi[index+P]�ɂ�����t���O�����g��}��
}

//�d�ݕt��LCS�ɂ�OPwithKmismatch���ǂ������`�F�b�N����֐�
void makeWeightedLCS(vector<int> &a, vector<int> &relative_a, set<pair<int, int>> &setb, vector<int> &Sa, vector<int> &Sb, int index, vector<int> &a_dash, vector<int> &I) { //I��weight��vector
  vector<int> d;
  int weight = 0;
  int predNum_a = -1;
  int predNum_b = -1;
  for (auto x : setb) {
    if (Sa[x.second-index] == Sb[x.second%m]) { //Sb[((x.second-index)+index)%P]��\���Ă���Bindex���Y���Ă���x.second���Y�����C�����A�����index���z��̃X�^�[�g�ʒu���Y���Ă��邽��
      if (x == (*setb.begin())) { //1�ԍŏ��̗v�f�������������ꍇ(d0�ɓ�����)
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
      if (x != (*setb.begin())) I.push_back(weight); //1�ԍŏ��̗v�f���قȂ��Ă���킯�ł͂Ȃ���
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

//�e�ϐ�������������O����
void initializeSTi(set<pair<int, int>> &TiSet, vector<int> &predTi, vector<int> &STi, vector<int> &Text, vector<int>::iterator it) { //STi�̏�����
  int index = (int)distance(Text.begin(), it); //���ݎQ�Ƃ��Ă���index��int�^�Ŏ擾 size_t����int�ɃL���X�g���Ă��邩�炠�܂�ǂ��Ȃ�
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
    predP[x.second] = pred; //x.second�͕K��P�ȉ��ł���K�v������D
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
  copy(pattern.begin(), pattern.end(), sortedP.begin()); //sortedP�̃T�C�Y���w�肵�Ă��Ȃ��Ɣ͈͊O�A�N�Z�X����D
  sort(sortedP.begin(), sortedP.end());
  for (int i = 0; i<pattern.size(); i++) {
    for (int j = 0; j<pattern.size(); j++) {
      if (sortedP[i] == pattern[j]) relativeP[j] = i;
    }
  }
}
void initializeFrag(map<int, int> &mp, vector<int> &SP, vector<int> &STi) { //�t���O�����g�̏������֐�
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
void initializeSPValueIndexMap(map<int, int> &mp, vector<int> &SP) { //SP�ɑΉ�����Map���쐬���邽�߂̊֐�
  for (int i = 0; i<SP.size(); i++) {
    auto it = mp.find(SP[i]);
    if (it == mp.end()) {
      mp[SP[i]] = i;
    }
  }
}

void OPwithKmismatches() {
  //�t�@�C���̓ǂݍ��݁A�y�сA���������
  vector<int> pattern;
  vector<int> text;
  readfile("../../PatternText/pattern.txt", pattern);
  readfile("../../PatternText/text.txt", text);
  m = pattern.size();
  n = text.size();
  K = 0;
  SPECIALCHAR = -n - 1;
  vector<int> SP(m);
  initializeSP(SP, pattern);
  map<int, int> SPValueIndex;
  initializeSPValueIndexMap(SPValueIndex, SP);
  vector<vector<int>> LCP(m, vector<int>(m));
  readCSV("../../PatternText/LCP.csv", LCP);
  vector<int> relativeP(m);
  makeRelativeP(pattern, relativeP);
  //�������I��
  for (K = 0; K<m/3; K++) {
    //�V�����n�߂邽�тɏ���������K�v���������
    set<pair<int, int>> TiSet;
    vector<int> predTi(m);
    vector<int> STi(m);
    auto it = text.begin();
    initializeSTi(TiSet, predTi, STi, text, it); //TiMap, predTi, STi�̏�����
    map<int, int> fragMap;
    initializeFrag(fragMap, SP, STi);
    //�v���X�^�[�g
    clock_t start = clock();
    int index = 0;
    if (isomorephicKmismatches(pattern, relativeP, SP, LCP, STi, TiSet, fragMap, index)) DEBUG cout << index << endl;
    for (index = 1; index <= n - m; index++) {
      slideWindow(TiSet, predTi, STi, text, it, fragMap, SPValueIndex);
      if (isomorephicKmismatches(pattern, relativeP, SP, LCP, STi, TiSet, fragMap, index)) DEBUG cout << index << endl;
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
    if (calculateWeightedLCS(a_dash, I) >= m - K) return true; //�����̏o��
  }
  return false;
}


int main() {
  OPwithKmismatches();
  return 0;
}