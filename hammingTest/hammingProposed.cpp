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

int readCSV(string fileName, vector<vector<int> > &input) { //STchangeIndex,LCSを読み込む関数
  cout << "read" << fileName << " file" << endl;
  ifstream file;
  stringstream ss;
  string tmp;
  file.open(fileName);
  if(file.is_open()) {
    for(int i=0; i<input.size(); i++) {
      getline(file, tmp);
      ss << tmp;
      for(int j=0; j<input[0].size(); j++) ss >> input[i][j];
    }
  }else{ return 1; }
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

void printMap(map<int, int> &mp) {
  for (auto p:mp) cout << "first=" << p.first << "\tsecond=" << p.second << endl;
}

void initializeFrag(map<int, int> &mp, vector<int> &SP, vector<int> &STi) { //フラグメントの初期化関数
  int j = 0;
  for (int i=0; i<SP.size(); i++) {
    for(j=0; j<SP.size(); j++) {
      if (STi[i] == SP[j]) {
        mp[i] = j;
        break;
      }
    }
    if (j==SP.size()) mp[i] = $;
  }
}

int FragHDistance(int index, int fragIndex, int fragLen, vector<vector<int>> &LCP) { //フラグメント(SP[fragIndex:])と長さと，現在のみているインデックスSP[index:]が入力されたときに，SP[j:]とSP[index:]のH距離を返す関数
  if(fragIndex==$ && fragLen>1) return 1 + FragHDistance(index+1, fragIndex+1, fragLen-1, LCP);
  if(LCP[index][fragIndex] >= fragLen) return 0;
  if(fragLen==1) return 1;
  int Hamming = 1;
  int mismatchIndex = LCP[index][fragIndex]; //間違えがあったインデックス
  return Hamming + FragHDistance(index+mismatchIndex+1, fragIndex+mismatchIndex+1, fragLen-mismatchIndex-1, LCP);
}

int FragLength(map<int, int> &mp, map<int, int>::iterator it) {//マップとイテレータがあたえられたときにフラグメントの長さを返す関数
  //最低でも1が返される
  auto itSuc = next(it);
  if(itSuc == mp.end()) return (*(mp.begin())).first + P - (*it).first;
  else return (*itSuc).first - (*it).first;
}

bool HammingUnder3K(map<int, int> &mp, vector<int> &SP, vector<vector<int>> &LCP) { //フラグメントが与えられた時ハミング距離を返す関数
  int beforeMismatchIndex = 0;
  int fragCountBetweetnMismatches = 0; //ミスマッチ間のフラグメントの数の管理
  int index = 0; //SP[index:]とLCPを比べるためにどこから比べるかを管理する変数
  int hamming = 0;
  for(auto it = mp.begin(); it != mp.end(); it++) {
    int fragLen = FragLength(mp, it);
    int FragHamming = FragHDistance(index, (*it).second, fragLen, LCP);
    //デバッグ用の処理
    // if((*(mp.begin())).first==311) {
    //   cout << "it.first=" << it->first << "\tit.second" << it->second << "\tFragHamming=" << FragHamming << "\t" << endl;
    // }
    //ここまで
    if(FragHamming == 0) {
      (*it).second = index;
      fragCountBetweetnMismatches++;
    }else{ //不一致があったときの処理
      if(fragCountBetweetnMismatches >=3) { //ミスマッチの間のフラグメントの個数の判定．論文のfig2を参照
        //フラグメントをconcatenateする
        auto itTmp = prev(it);
        for(int i=1; i<fragCountBetweetnMismatches; i++) mp.erase(itTmp--);
      }
      fragCountBetweetnMismatches = 0; //ミスマッチ間のフラグメントの数を初期化
    }
    index += fragLen; //インデックスをフラグメントの長さ分すすめる
    hamming += FragHamming;
    //デバッグ用の処理
    //if((*(mp.begin())).first==311) cout << "hamming=" << hamming << endl;
    //ここまで
    if(hamming > 3*K) return false;
  }
  return true;
}

void MakeSPValueIndexMap(map<int, int> &mp, vector<int> &SP) { //SPの要素があるかどうか，また要素があった場合はそのインデックスを知るための関数
  for (int i=0; i<SP.size(); i++) {
    auto it = mp.find(SP[i]);
    if(it == mp.end()) {
      mp[SP[i]] = i;
    }
  }
}

int insertValueSTi(map<int, int> &SPValueIndex, int value) {
  auto SPit = SPValueIndex.find(value);
  if (SPit == SPValueIndex.end()) return $;
  else return (*SPit).second;
}

void ChangeFrag(map<int, int> &mp, int index, int insertNum) { //STi[index]が変化した時に，それに対応するフラグメントを変える関数
  auto it = mp.lower_bound(index);
  if((*it).first != index) { //キーが無かった時
    it--;
    int FragLen = FragLength(mp, it);
    int l = index - (*it).first; //STi[index]は，所属しているフラグメント内のl番目になる
    mp[index] = insertNum;
    if(l+1!=FragLen) { //w[l+index+1:]も入れる必要がある
      mp[index+1] = (*it).second+l+1;
    }
  }else{
    int FragLen = FragLength(mp, it);
    if(FragLen != 1) { //|w[0]|=1と，w[1:]に分ける必要がある時(w[0]は，mp[index]に当たる)
      mp[index+1] = (*it).second +1;
    }
    (*it).second = insertNum; //mp[index] = insertNum;と同じ処理．こっちのほうが早いため
  }
}

void slideWindow(map<int, int> &mp, int index, int insertNum) { //ウィンドウをずらす時(iをインクリメントする時)に，対応するように，mapを書き換える
  auto it = mp.begin();
  if(FragLength(mp, it)!=1) mp[index+1] = (*it).second+1;
  mp.erase(it);
  mp[index+P] = insertNum; //mpにSTi[index+P]にあたるフラグメントを挿入
}

int main() {
  vector<int> SP(P);
  vector<vector<int> > ST(T-P+1, vector<int> (P));
  if(readSignature("./Signature.csv", SP, ST) == -1) { return 1; }
  // printVector(SP);
  //printMatrix(ST);
  vector<vector<int>> STchangeIndex(T-P, vector<int> (P));
  if(readCSV("./STchangeIndex.csv", STchangeIndex) == -1) { return 1; }
  //printMatrix(STchangeIndex);
  vector<vector<int>> LCP(P, vector<int> (P));
  if(readCSV("./LCS.csv", LCP) == -1) { return 1; }
  // printMatrix(LCP);

  map<int, int> mp; //フラグメント
  initializeFrag(mp, SP, ST[0]);
  // printMap(mp);
  map<int, int> SPValueIndex;
  MakeSPValueIndexMap(SPValueIndex, SP);
  // printMap(SPValueIndex);

  clock_t start = clock();
  if(HammingUnder3K(mp, SP, LCP)) {
    cout << 0 << endl;
  }
  for(int i=0; i<STchangeIndex.size(); i++) {
    for(int j=0; j<STchangeIndex[0].size(); j++) {
      if(STchangeIndex[i][j]==1) { //STが変化したときの処理
        int insert = insertValueSTi(SPValueIndex, ST[i+1][j]);
        ChangeFrag(mp, j+i+1, insert);
      }
    }
    int insertNum = insertValueSTi(SPValueIndex, ST[i+1][P-1]);
    slideWindow(mp, i, insertNum);
    if(HammingUnder3K(mp, SP, LCP)) cout << i+1 << endl;
  }
  clock_t finish = clock();
  cout << "time:::" << (double)(finish-start)/CLOCKS_PER_SEC << "sec" << endl;
  return 0;
}
