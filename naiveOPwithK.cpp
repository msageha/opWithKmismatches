#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>

#define P 7
#define T 10000
#define K 2
using namespace std;

int nextIgnore(bitset <P> &ignoreIndexBit, int mistakesNum) { //無視するところは1が立つ
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

bool isomorphicWithKmismatches(vector <int> &pattern1, vector <int> &pattern2) {
  bitset <P> ignoreIndexBit;
  bool check = true;
  do{
    check = true;
    for(int i=0; i<P; i++) {
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
  }while(nextIgnore(ignoreIndexBit, K));
  return check;
}

int main(int argc, char** argv) {
  //ファイルの読み込み処理
  vector <int> pattern(P);
  vector <int> textSubstring(P);
  vector <int> text(T);
  readfile("./PatternText/pattern.txt", pattern);
  readfile("./PatternText/text.txt", text);

  ofstream file;
  file.open(("./answerC/"+to_string(K)+".txt"), ios::trunc);
  file << "P: " << P << "\nT: " << T << "\nmismatches: " << K << endl;
  clock_t start = clock();
  for(int i=0; i<T-P+1; i++) {
    for(int j=0; j<P; j++) {
      textSubstring[j] = text[i+j];
    }
    if(isomorphicWithKmismatches(pattern,textSubstring)) {
      file << "i: " << i << endl;
    }
  }
  clock_t finish = clock();
  file << "time: " << (double)(finish-start)/CLOCKS_PER_SEC << "sec" << endl;
  file.close();
  return 0;
}