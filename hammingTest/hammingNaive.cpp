#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <ctime>
#include <vector>

#define P 10
#define T 100000
#define K 2
#define $ 100001

using namespace std;

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

bool HammingUnder3K(vector<int> &SP, vector<int> &STi){
  int hamming = 0;
  for(int i=0; i<SP.size(); i++) {
    if(SP[i]!=STi[i]) hamming++;
    if(hamming>3*K) return false;
  }
  return true;
}

int main() {
  vector<int> SP(P);
  vector<vector<int> > ST(T-P+1, vector<int> (P));
  clock_t start = clock();
  if(readSignature("./Signature.csv", SP, ST) == -1) { return 1; }
  for(int i=0; i<ST.size(); i++) {
    if(HammingUnder3K(SP, ST[i])) cout << i << endl;
  }
  clock_t finish = clock();
  cout << "time:::" << (double)(finish-start)/CLOCKS_PER_SEC << "sec" << endl;
  return 0;
}