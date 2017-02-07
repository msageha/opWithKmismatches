#include <iostream>
#include <utility>
#include <string>
#include <set>
#include <vector>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <sstream>
#include <time.h>

#define P 16
#define T 10000
#define K 4
#define $ T+1

using namespace std;
//pred pred(i)と，i のペア
//presentIndexは，現在のiのこと

inline int changeTableS(int predNum, int index) { //returnは，tableS[index]に入れる値
  if (predNum == $) return $;
  return (predNum-index);
}

int check(vector <int> &pred, vector <int> &tableS, int index) {
  for (int i = index; i < index+P; i++) {
    if (tableS[i%P] == $ && pred[i%P] == $) {

    }else if (tableS[i%P] != pred[i%P] - i) return -1;
  }
  return 0;
}

int changeS(set<pair<int, int> > &predSet, vector <int> &pred, set<pair<int, int> > &tableASet, vector <int> &tableA,vector <int> &tableS , int presentIndex, int insertNum) {
  //操作１
  auto pos = predSet.begin();
  while ((*pos).first == presentIndex) {
    auto pos_temp = predSet.lower_bound(make_pair(pred[presentIndex%P], 0));
    auto pos_temp2 = next(pos_temp, 1);
    if (pos_temp2 != predSet.end() && (*pos_temp).first == (*pos_temp2).first && tableA[(*pos_temp).second%P] == tableA[(*pos_temp2).second%P]) { //現在指しているインデックスをpredが持っているとき,pred(i)と同じ値をもつpredが存在しているときに，そっちを指すように変更するため．
      pred[(*pos).second%P] = (*pos_temp2).second;
      tableS[(*pos).second%P] = changeTableS(pred[(*pos).second%P], (*pos).second);
      predSet.insert(make_pair((*pos_temp2).second, (*pos).second));
    }
    else {
      pred[(*pos).second%P] = pred[presentIndex%P];
      tableS[(*pos).second%P] = changeTableS(pred[(*pos).second%P], (*pos).second);
      predSet.insert(make_pair(pred[presentIndex%P], (*pos).second));
    }
    predSet.erase(pos);
    pos = predSet.begin();
  }
  //cout << "操作1終わり" << endl;

  //操作2
  auto insertIt = tableASet.lower_bound(make_pair(tableA[presentIndex%P], 0));
  tableASet.erase(insertIt);
  // int deleteNumFromPredSet = tableA[presentIndex%P];
  tableA[presentIndex%P] = insertNum; //tableA vectorの修正終了
  insertIt = tableASet.lower_bound(make_pair(insertNum, 0)); //insertNum以上の中で最小の値を持つペアをさしている

  pos = predSet.lower_bound(make_pair(pred[presentIndex%P], 0));
  predSet.erase(pos);
  int k = 0;
  if (insertIt == tableASet.end()) { //insertされる数字が最大となったときの処理
    insertIt--;
    auto insertItTemp = tableASet.lower_bound(make_pair((*insertIt).first, 0));
    predSet.insert(make_pair((*insertItTemp).second, presentIndex + P));
    pred[presentIndex%P] = (*insertItTemp).second;
    tableS[presentIndex%P] = changeTableS(pred[presentIndex%P], presentIndex);
    tableASet.insert(make_pair(insertNum, presentIndex + P)); //tableASetの構築終わり
    //cout << "操作2終わり" << endl;
    return 0;
  }
  else {
    k = (*insertIt).second; //8 firstは2
    tableASet.insert(make_pair(insertNum, presentIndex + P)); //tableASetの構築終わり
  }


  pred[presentIndex%P] = pred[k%P]; //$
  tableS[presentIndex%P] = changeTableS(pred[presentIndex%P], presentIndex+P);
  int predK = pred[k%P]; //$
  predSet.insert(make_pair(pred[presentIndex%P], presentIndex + P));
  if (tableA[k%P] == tableA[presentIndex%P]) { //挿入した数字と同じ値をもつものがあった場合

  }
  else {
    pred[k%P] = presentIndex + P; //6
    tableS[k%P] = changeTableS(pred[k%P], k);
    pos = predSet.lower_bound(make_pair(predK, 0));
    while ((*pos).first == predK && (*pos).second != presentIndex + P) {
      pred[(*pos).second%P] = presentIndex + P;
      tableS[(*pos).second%P] = changeTableS(pred[(*pos).second%P], (*pos).second);
      predSet.insert(make_pair(presentIndex + P, (*pos).second));
      predSet.erase(pos);
      pos = predSet.lower_bound(make_pair(predK, 0));
    }
  }
  //cout << "操作2終わり" << endl;

  return 0;
}

int makePred(vector<int> &pattern, vector<int> &pred) {
  int min = 0;
  for(int i=0; i<P; i++) {
    min = -1;
    for(int j=0; j<P; j++) {
      if(i==j) continue;
      if(pattern[j] < pattern[i] && min < pattern[j]) {
        min = pattern[j];
        pred[i] = j;
      }
    }
    if(min==-1) pred[i] = $;
  }
  return 0;
}

int makeTableS(vector<int> &pattern, vector<int> &tableS) {
  vector<int> pred(P);
  int min = 0;
  for(int i=0; i<P; i++) {
    min = -1;
    for(int j=0; j<P; j++) {
      if(i==j) continue;
      if(pattern[j] < pattern[i] && min < pattern[j]) {
        min = pattern[j];
        pred[i] = j;
      }
    }
    if(min==-1) pred[i] = $;
  }
  for(int i=0; i<P; i++) {
    tableS[i] = changeTableS(pred[i], i);
  }
  return 0;
}

int nextIgnore(bitset <P> &ignoreIndexBit, int mistakesNum) {
  if(mistakesNum==0) return 0; //mistakesNum==0のときだから
  do {
    ignoreIndexBit = bitset<P>(ignoreIndexBit.to_ulong() + 1ULL);
    if(ignoreIndexBit.count()==P) return 0;
  } while(ignoreIndexBit.count()!=mistakesNum);
  return 1;
}

int isomorphicWithKmistakes(vector <int> &pattern1, vector <int> &pattern2, int mistakesNum) {
  bitset <P> ignoreIndexBit;
  bool isomorphic = true; //順序同型かチェックする
  for(int k=0; k<mistakesNum; k++) ignoreIndexBit.set(k); //ignoreIndexBitの初期化
  //int presentIgnoreIndex = mistakesNum-1; //ignoreIndexを次に進めるためのIndexを保持している
  do {
    for(int i=0; i<P; i++) {
      if(ignoreIndexBit[i]==1) continue;
      for(int j=i; j<P; j++) { //j=0にする必要がある？
        if(ignoreIndexBit[j]==1) continue;
        if(pattern1[i] <= pattern1[j] && pattern2[i] > pattern2[j]) {
          isomorphic = false;
        } else if(pattern2[i] <= pattern2[j] && pattern1[i] > pattern1[j]){
          isomorphic = false;
        }
      }
    }
    if(isomorphic) return 1;
    isomorphic = true;
  }while(nextIgnore(ignoreIndexBit, mistakesNum));
  return 0;
}

int haming(vector<int> tableS1, vector<int> tableS2) {
  int count = 0;
  for(int i=0; i<P; i++) {
    if(tableS1[i] != tableS2[i]) count++;
  }
  return count;
}

int main(int argc, char** argv) {
  vector <int> pattern1(P);
  vector <int> pattern2(P);
  vector <int> text(T);
  vector <int> candidateIndex(T, -1);
  string tmp;
  ifstream file;
  file.open("pattern.txt");
  if (file.is_open()) {
    for(int i = 0; i<P; i++) {
      getline(file, tmp);
      try { pattern1[i] = stoi(tmp); }
      catch(invalid_argument e) { return -1; }
    }
  }
  file.close();
  file.open("text.txt");
  if (file.is_open()) {
    for(int i = 0; i<T; i++) {
      getline(file, tmp);
      try{ text[i] = stoi(tmp); }
      catch(invalid_argument e) { return -1; }
    }
  }
  file.close();

  // 読み込み終了

  vector<int> patternTableS(P);
  makeTableS(pattern1, patternTableS);

  set<pair<int, int>> textPredSet;
  set<pair<int, int>> textTableASet;
  vector<int> textPred(P);
  vector<int> textTableS(P);
  for(int i=0; i<P; i++) {
    pattern2[i] = text[i];
  }
  makePred(pattern2, textPred);
  for(int i=0; i<P; i++) {
    textTableS[i] = changeTableS(textPred[i], i);
    textPredSet.insert(make_pair(textPred[i], i));
    textTableASet.insert(make_pair(pattern2[i], i));
  }

  clock_t start = clock();

  int candidateCount = 0;
  int i=0;
  for(i=0; i<T-P+1; i++) {
    if(haming(patternTableS ,textTableS)<K) {
      candidateIndex[candidateCount] = i;
      candidateCount++;
    }
    changeS(textPredSet, textPred, textTableASet, pattern2, textTableS, i, text[i+P]);
  }
  if(haming(patternTableS ,textTableS)<K) {
    candidateIndex[candidateCount] = i;
    candidateCount++;
  }
  //あとは，candidateIndexに入っている解の候補となるインデックスを検証するだけ．
  int count = 0;
  for(i=0; i<candidateCount; i++) {
    for(int j=0; j<P; j++) {
      pattern2[j] = text[candidateIndex[i]+j];
    }
    if(isomorphicWithKmistakes(pattern1, pattern2, K)==0) {
    }else {
      cout << "CorrectIndex:::" << i << endl;
      count++;
    }
  }
  clock_t finish = clock();
  cout << "countNum:::" << count << endl;
  cout << "time:::" << (double)(finish-start)/CLOCKS_PER_SEC << "sec" << endl;
  for(int i=0; i<P; i++) {
    cout << "textTableS[" << i << "]=" << textTableS[i] << endl;
  }

  return 0;
}