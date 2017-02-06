#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>

#define P 7
#define T 10000
#define K 2

using namespace std;

int readCSV(string fileName, vector<vector<int> > &input) {
  cout << "read" << fileName << " file" << endl;
  ifstream file;
  stringstream ss;
  string tmp;
  file.open(fileName);
  if(file.is_open()) {
    for(int i=0; i<input.size(); i++) {
      getline(file, tmp);
      ss << tmp;
      for(int j=0; j<input[0].size(); j++) {
        ss >> input[i][j];
      }
    }
  }else{ return -1; }
  file.close();
  return 0;
}

int main() {
  map<int, int> mp;
  vector<vector<int> > STchangeIndex(T-P-1, vector<int> (P));
  if(readCSV("./STchangeIndex.csv", STchangeIndex) == -1) { return -1; }
  for(int i=0; i<STchangeIndex.size(); i++) {
    for(int j=0; j<STchangeIndex[0].size(); j++) cout << STchangeIndex[i][j] << "\t";
    cout << endl;
  }
  return 0;
}
