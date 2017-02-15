#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>

#define DEBUG 
#define P 8
using namespace std;

int m, n, K;

int nextIgnore(bitset <P> &ignoreIndexBit, int mistakesNum) { //無視するところは1が立つ
  if(mistakesNum==0) return 0; //mistakesNum==0のときだから
  do {
    ignoreIndexBit = bitset<P>(ignoreIndexBit.to_ulong() + 1ULL);
    if(ignoreIndexBit.count()==m) return 0;
  } while(ignoreIndexBit.count()!=mistakesNum);
  return 1;
}

void makeNextIgnore(vector<bitset<P>> &ignoreIndexVectorBit){
  bitset <P> ignoreIndexBit;
  while(nextIgnore(ignoreIndexBit, K)){
    ignoreIndexVectorBit.push_back(ignoreIndexBit);
  }
  if(K==0) ignoreIndexVectorBit.push_back(ignoreIndexBit);
}

int readfile(string fileName, vector<int> &input) { //ファイルの読み込み関数
  cout << "read " << fileName << " file" << endl;
  ifstream file;
  string tmp;
  file.open(fileName);
  if(file.is_open()) {
    while(getline(file, tmp)) {
      try {input.push_back(stoi(tmp));}
      catch(invalid_argument e) {return -1;}
    }
  }else{return -1;}
  file.close();
  return 0;
}

bool isomorphicWithKmismatches(vector <int> &pattern1, vector <int> &pattern2, vector<bitset<P>> &ignoreIndexVectorBit) {
  bitset <P> ignoreIndexBit;
  bool check = true;
  for(auto ignoreIndexBit : ignoreIndexVectorBit){
    check = true;
    for(int i=0; i<m; i++) {
      if(ignoreIndexBit[i]==1) continue;
      for(int j=0; j<P; j++) {
        if(ignoreIndexBit[j]==1) continue;
        if((pattern1[i] <= pattern1[j] && pattern2[i] > pattern2[j]) or (pattern2[i] <= pattern2[j] && pattern1[i] > pattern1[j])) {
          check = false;
          break;
        }
      }
    }
    if(check) break;
  }
  return check;
}

int main(int argc, char** argv) {
  //ファイルの読み込み処理
  vector <int> pattern;
  vector <int> text;
  readfile("pattern.txt", pattern);
  readfile("text.txt", text);
  m = pattern.size();
  n = text.size();
  vector <int> textSubstring(m);
  ofstream file;
  for(K=0; K<=m/4; K++) {
    vector<bitset<P>> ignoreIndexVectorBit;
    makeNextIgnore(ignoreIndexVectorBit);
    file.open(("answer"+to_string(K)+".txt"), ios::trunc);
    file << "m: " << m << "\nT: " << n << "\nmismatches: " << K << endl;
    clock_t start = clock();
    for(int i=0; i<n-m+1; i++) {
      //DEBUG cout << i << endl;
      for(int j=0; j<m; j++) {
        textSubstring[j] = text[i+j];
      }
      if(isomorphicWithKmismatches(pattern,textSubstring, ignoreIndexVectorBit)) {
        DEBUG file << i << endl;
      }
    }
    clock_t finish = clock();
    file << "time: " << (double)(finish-start)/CLOCKS_PER_SEC << "sec" << endl;
    file.close();
  }
  return 0;
}