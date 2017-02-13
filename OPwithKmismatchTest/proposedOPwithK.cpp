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

#define P 10
#define T 100000
#define K 2
#define $ -T-1

using namespace std;

int nextIgnore(bitset <P> &ignoreIndexBit, int mistakesNum) { //無視するところは1が立つ //個々はもう少し高速化する必要があるかも…
  if(mistakesNum==0) return 0; //mistakesNum==0のときだから
  do {
    ignoreIndexBit = bitset<P>(ignoreIndexBit.to_ulong() + 1ULL);
    if(ignoreIndexBit.count()==P) return 0;
  } while(ignoreIndexBit.count()!=mistakesNum);
  return 1;
}

int readfile(string fileName, vector<int> &input) { //ファイルの読み込み関数
  cout << "read " << fileName << " file" << endl;
  ifstream file;
  string tmp;
  file.open(fileName);
  if(file.is_open()) {
    for(int i=0; i<input.size(); i++) {
      getline(file, tmp);
      try {input[i] = stoi(tmp);}
      catch(invalid_argument e) {return -1;}
    }
  }else{return -1;}
  file.close();
  return 0;
}

int readSignature(string fileName, vector<int> &SP, vector<vector<int> > &ST) { //Signatureを読み込む関数
  cout << "read" << fileName << " file" << endl;
  ifstream file;
  stringstream ss;
  string tmp;
  file.open(fileName);
  if(file.is_open()) {
    getline(file, tmp);
    ss << tmp;
    for(int i=0;  i<SP.size(); i++) ss >> SP[i];
    getline(file, tmp);
    for(int i=0; i<ST.size(); i++) {
      getline(file, tmp);
      ss << tmp;
      for(int j=0; j<ST[0].size(); j++) ss >> ST[i][j];
    }
  }else{ return 1; }
  file.close();
  return 0;
}

void printMatrix(vector<vector<int> > &input) {
  for(int i=0; i<input.size(); i++) {
    for (int j=0; j<input[0].size(); j++) cout << input[i][j] << "\t";
    cout << endl;
  }
}

void printVector(vector<int> &input) {
  for(int i=0; i<input.size(); i++) cout << input[i] << "\t";
  cout << endl;
}

void printSetPair(set<pair<int, int>> &setPair) {
  for (auto p:setPair) cout << "first=" << p.first << "\tsecond=" << p.second << endl;
}

void makeRelativeP(vector<int> &pattern, vector<int> &relativeP) {
  vector<int> sortedP(pattern);
  assert(sortedP.size()>0);
  copy(pattern.begin(), pattern.end(), sortedP.begin()); //sortedPのサイズを指定していないと範囲外アクセスする．
  sort(sortedP.begin(), sortedP.end());
  for(int i=0; i<pattern.size(); i++) {
    for(int j=0; j<pattern.size(); j++) {
      if(sortedP[i]==pattern[j]) relativeP[j] = i;
    }
  }
}

void makeWeightedLCS(vector<int> &a, vector<int> &relative_a, set<pair<int, int>> &setb, vector<int> &Sa, vector<int> &Sb, vector<int> &a_dash, vector<int> &I) { //Iはweightのvector
  vector<int> d;
  int weight = 0;
  int predNum_a = -1;
  int predNum_b = -1;
  for(auto x : setb) {
    if(Sa[x.second]==Sb[x.second]) {
      if(x == (*setb.begin())) { //1番最小の要素が等しかった場合(d0に当たる)
        d.push_back(x.second);
        a_dash.push_back(relative_a[x.second]);
      }
      if ((predNum_a<a[x.second] && predNum_b==x.first)
        || (predNum_a==a[x.second] && predNum_b<x.first)){
      }else{
        weight++;
        predNum_a = a[x.second];
        predNum_b = x.first;
      }
    }else{
      d.push_back(x.second);
      a_dash.push_back(relative_a[x.second]);
      if(x != (*setb.begin())) I.push_back(weight); //1番最初の要素が異なっているわけではない時
      weight = 1;
      predNum_a = -1;
      predNum_b = -1;
    }
  }
  I.push_back(weight);
}

int calculateWeightedLCS(vector<int> &a_dash, vector<int> &I) {
  vector<int> v(P);
  for(int i=0; i<I.size(); i++) {
    int temp=0;
    if (a_dash[i]!=0) temp = *max_element(v.begin(), v.begin()+a_dash[i]);
    v[a_dash[i]] = max(v[a_dash[i]], temp+I[i]);
  }
  return *max_element(v.begin(), v.end());
}

int main() {
  vector <int> pattern(P);
  vector <int> text(T);
  set<pair<int, int>> TiMap;
  vector <int> startIndex(1872, 0);
  vector <int> SP(P);
  vector <int> relativeP(P); //Pを相対的に小さくしたもの
  vector<vector<int>> ST(T-P+1, vector<int> (P));;
  readfile("../PatternText/pattern.txt", pattern);
  readfile("../PatternText/text.txt", text);
  readfile("./hammingAnswerK=2.txt", startIndex);
  readSignature("./Signature.csv", SP, ST);
  makeRelativeP(pattern, relativeP);
  for(int i=0; i<startIndex.size(); i++) {
    TiMap.clear();
    for(int j=0; j<P; j++) TiMap.insert(make_pair(text[startIndex[i]+j], j));
    vector<int> a_dash;
    vector<int> I;
    makeWeightedLCS(pattern, relativeP, TiMap, SP, ST[startIndex[i]], a_dash, I);
    if(calculateWeightedLCS(a_dash, I)>=P-K) {
      cout << startIndex[i] << endl;
    }
  }
  return 0;
}