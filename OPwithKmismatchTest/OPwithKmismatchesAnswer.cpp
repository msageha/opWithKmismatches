#include <algorithm>
#include <bitset>
#include <cassert>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define P 10
#define T 100000
#define K 1

using namespace std;

template <typename TKey>
struct slice {
private:
  std::vector<TKey>* _vector = nullptr;
  typename std::vector<TKey>::iterator _begin;
  typename std::vector<TKey>::iterator _end;

public:
  slice() {
  }
  slice(std::vector<TKey> &vector, const int startIndex, const int endIndex) {
    _vector = &vector;
    _begin = vector.begin() + startIndex;
    _end = vector.begin() + endIndex;
  }

  const TKey& at(const int index) const {
    assert((int)std::distance(_begin, _end) >= index);
    return *(_begin + index);
  }

  const TKey& operator [](const int index) const {
    assert((int)std::distance(_begin, _end) >= index);
    return *(_begin + index);
  }
};

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

bool isomorphicWithKmismatches(vector<int> &pattern1, slice<int> &pattern2) {
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
  vector <int> pattern(P);
  vector <int> text(T);
  vector <int> startIndex(T, 0);
  readfile("../PatternText/pattern.txt", pattern);
  readfile("../PatternText/text.txt", text);
  readfile("./hammingAnswerK=1.txt", startIndex);
  for(int i=0; i<T-P+1; i++) {
    auto Ti = slice<int>(text, startIndex[i], startIndex[i]+P-1);
    if(isomorphicWithKmismatches(pattern,Ti)) {
      cout << "i: " << startIndex[i] << endl;
    }
    if(startIndex[i]==0) break;
  }

  return 0;
}
