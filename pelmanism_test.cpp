#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wiringPi.h>
#include <dirent.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <map>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define GPIO19 19
#define GPIO20 20
#define GPIO21 21
#define GPIO22 22
#define GPIO23 23
#define GPIO24 24
#define GPIO25 25
#define GPIO26 26
#define BUTTON_NUM 4

using namespace std;

void setPair(int* pair, int fileNum);
int buttonSensing();
void playReactSound(int choosing, int* keys, vector<string> &wavfileList);
void playComingOut(int index);
void resetResource();
int translateTextToWav(vector<string> &wavfileList);
int judgeGameEnd(int* blockGotten);
void onLoadText();
void onGameStart(int* blockGotten, int* keys, vector<string> &wavfileList);
void onFirstStep(int* blockGotten, int* keys, vector<string> &wavfileList);
void onSecondStep(int* blockGotten, int* keys, vector<string> &wavfileList);
void onGameEnd();

enum state {
  loadText,
  gameStart,
  waitFirstStep,
  waitSecondStep,
  gameEnd,
};

enum state gameState = loadText;

/* 点滅プロセス管理用 */
pid_t PID;
int status, segid;
int *segaddr;

void setPair(int* pair, int fileNum){
  int place;
  srand((unsigned)time(NULL));
  int pairs = (fileNum < BUTTON_NUM/2) ? BUTTON_NUM / 2 - 1 : BUTTON_NUM/2; 
  for(int i = 0; i < pairs ; i++){
    for(int j = 0 ; j < 2 ; j++){
      place = rand() % BUTTON_NUM;
      while(true){
      	if(pair[place] == -1) {
	  pair[place] = i;
	  break;
	}
	place = (place + 1) % BUTTON_NUM;
      }
    }
  }
  return;
}

int main(void){
  int status;
  int blockGotten[BUTTON_NUM] = {0,0,0,0};
  int keys[BUTTON_NUM] = {-1,-1,-1,-1};
  vector<string> wavfileList;
  
  if(wiringPiSetupGpio() == -1) return 1;

  pinMode(GPIO19,OUTPUT);
  pinMode(GPIO20,OUTPUT);
  pinMode(GPIO21,OUTPUT);
  pinMode(GPIO22,OUTPUT);
  
  pinMode(GPIO23,INPUT);
  pinMode(GPIO24,INPUT);
  pinMode(GPIO25,INPUT);
  pinMode(GPIO26,INPUT);

  while(true) {
    switch(gameState) {
      // ゲームに必要なテキストファイルの読み込み
      case loadText:
        // 各自の端末からのtextファイルを受け取る
        onLoadText();
        // 上記関数は内部がまだ未定義. 仕様固まり次第って感じで
        
      // ゲーム開始
      case gameStart:
        onGameStart(blockGotten, keys, wavfileList);
        break;

      // 各ターンの1手目
    case waitFirstStep:
      onFirstStep(blockGotten, keys, wavfileList);
      break;

      // 各ターンの2手目
    case waitSecondStep:
      onSecondStep(blockGotten, keys, wavfileList);
      break;

      // ゲーム終了
    case gameEnd:
      onGameEnd();
      break;

      // デフォルト
    default:
      break;
    }
  }

  return 0;
}

int buttonSensing(){
  // pin番号など確定次第、改造必須
  int status[BUTTON_NUM];
  while(true){
    status[0] = digitalRead(GPIO23);
    status[1] = digitalRead(GPIO24);
    status[2] = digitalRead(GPIO25);
    status[3] = digitalRead(GPIO26);
    if(status[0]) return 0;
    if(status[1]) return 1;
    if(status[2]) return 2;
    if(status[3]) return 3;
  }
}

void playReactSound(int choosing, int* keys, vector<string> &wavfileList){
  // 実際にはwavfileListを参照して再生するwavファイルを選択すること
  int index = keys[choosing] * 2;
  string reactFile = wavfileList[index];
  string command = "sudo aplay " + reactFile;
  system(command.c_str());
  return;
}

void playComingOut(int index, vector<string> &wavfileList){
  string command = "sudo aplay " + wavfileList[index];
  system(command.c_str());
  return;
}

void resetResource(){
  // ゲーム切り替わり時に、テキストデータ・音声データを全て消す
  
  // txt_data下には、送信されてきたネタテキスト及び名前とネタをスプリットしたテキストを保存する
  system("sudo rm /home/xiao/pelmani/txt_data/*.txt");
  
  // wav_data下には、名前、ネタをwav化したファイルを保存する
  system("sudo rm /home/xiao/pelmani/wav_data/*.wav");
}


int translateTextToWav(vector<string> &wavfileList){
  DIR *dir;
  struct dirent *dp;
  string txtpath = "/home/xiao/pelmani/txt_data/";
  string wavpath = "/home/xiao/pelmani/wav_data/";
  multimap<string, int> nameList; //名前衝突判定
  int tmp;
  int fileNum = 0;
  
  dir = opendir(txtpath.c_str());
  // for(全てのネタtxtファイルを走査しきるまで)
  for(dp = readdir(dir) ; dp != NULL ; dp = readdir(dir)){
    if(strstr(dp->d_name,".txt") != NULL){ 
      fileNum++;
      string filename = string(dp->d_name);
      string complete_path = txtpath + filename;
      ifstream fin(complete_path.c_str());

      if (!fin) return -1;

      stringstream stream;
      stream << fin.rdbuf();
      fin.close();

      string rawdata(stream.str());
      // 1, rawdata内の文字列を 「:」を境に分ける("名前: ネタ" の形式をとるはずなので)
      stringstream ss{rawdata};
      string buf, name, neta;
      if(getline(ss, buf, ':')){name = buf;}
      if(getline(ss, buf, ':')){neta = buf;}
  
      // 2, 分けた文字列を加工して、それぞれ別のファイルとしてpelmani/txt_data下に保存
      name = name + "さん";
      neta = "の カミングアウト    " + neta;
      
      nameList.emplace(name,fileNum);
      
      // 同じ人のネタが出たら序数を振る
      tmp = nameList.count(name);
      if(tmp > 1) name = name + " " +to_string(tmp);
      
      int extent = filename.find_last_of(".");
      string original = filename.substr(0, extent);
      string name_txtfile = txtpath + original + "_name.txt"; //名前を格納するtxtファイル名
      string neta_txtfile = txtpath + original + "_neta.txt"; //ネタを格納するtxtファイル名
  
      ofstream nm(name_txtfile.c_str());
      nm << name << endl;
      nm.close();
      ofstream nt(neta_txtfile.c_str());
      nt << neta << endl;
      nt.close();
      
      // 3, 保存した時のファイル名を元にファイルパスを指定し、名前の部分とネタの部分をwav化し、pelmani/wav_data下に保存
      string name_wavfile = wavpath + original + "_name.wav"; //名前を格納するwavファイル名
      string neta_wavfile = wavpath + original + "_neta.wav"; //ネタを格納するwavファイル名
      
      string jtalk_command = "open_jtalk -m /usr/share/hts-voice/nitech-jp-atr503-m001/mei/mei_normal.htsvoice -x /var/lib/mecab/dic/open-jtalk/naist-jdic -ow ";
      
      string name_command = jtalk_command + name_wavfile + " " + name_txtfile;
      string neta_command = jtalk_command + neta_wavfile + " " + neta_txtfile;
      
      system(name_command.c_str());
      system(neta_command.c_str());
      
      // 4, wavfileListに保存したwavのファイルパスを追加する  
      wavfileList.push_back(name_wavfile);
      wavfileList.push_back(neta_wavfile);
    }
    // ここまでで一つのネタ分
  }
  // end for
  return fileNum;
}


int judgeGameEnd(int *blockGotten){
  for(int i = 0; i < BUTTON_NUM; i++){
    if(blockGotten[i]!=1) return 0;
  }
  return 1;
}

// ネタデータ受付時の処理
void onLoadText(){
  resetResource();
  // 音声「スマートフォンから投稿してください」
  system("sudo aplay /home/xiao/pelmani/play_asset/mei_asset2.wav");
  while(true){
    // ここでネタデータを受け付ける
    
    // ボタン数の半分ネタデータを受け付けるか、スタートボタンが押されたらbreak
    break;
  }
  
  //テスト用プリセット
  string txtpath = "/home/xiao/pelmani/txt_data/";
  
  string first = txtpath + "first.txt";
  
  ofstream nm(first.c_str());
  nm << "おさか:デバッグ中" << endl;
  nm.close();
  
  string second = txtpath + "second.txt";
  ofstream nt(second.c_str());
  nt << "おさか:眠い" << endl;
  nt.close();
  
  //プリセットここまで
  
  gameState = gameStart;
}

// ゲーム開始時の処理
void onGameStart(int* blockGotten, int* keys, vector<string> &wavfileList){
  int fileNum, rnd1, rnd2;
  string name_asset_path, neta_asset_path, rnd_name, rnd_neta;
  
  // マス取得状態とマスに割り当てられているkeyの初期化
  for(int i = 0; i < BUTTON_NUM; i++){
    blockGotten[i] = 0;
    keys[i] = -1;
  }
  wavfileList.clear();
  
  // wavfileListをすでに参照で受け取っているのでこの関数呼び出しは不正になるかも. コンパイルエラー時注意.
  fileNum = translateTextToWav(wavfileList);
  
  /* ファイル数に応じた処理 */
  
  // 0~1
    // 強制終了
  if (fileNum < 2){
    // ゲームできないお知らせ音声
    gameState = gameEnd;
    return;
  } else {
    srand((unsigned)time(NULL));
    // 2~3
      // wavfileListにただの音、パーティーゲームならではの命令を7組まで埋める
    // 4~7
      // wavfileListにただの音を7組分になるように入れる
    // 8~
      // 特に変更箇所はなし
    rnd1 = rand() % 10;
    rnd2 = rand() % 10;
    name_asset_path = "/home/xiao/pelmani/name_asset/asset";
    neta_asset_path = "/home/xiao/pelmani/neta_asset/asset";
    for(int i = 0; i < BUTTON_NUM/2 - 1 - fileNum; i++){
      rnd_name = name_asset_path + to_string(rnd1)+".wav";
      rnd_neta = name_asset_path + to_string(rnd2)+".wav";
      wavfileList.push_back(rnd_name);
      wavfileList.push_back(rnd_neta);
      rnd1 = (rnd1 + 3) % 10;
      rnd1 = (rnd2 + 7) % 10;
    }
  
    // setPairはドボンマスの考慮を入れる
    setPair(keys, fileNum);
    
    int timer = 5;
    while(timer){
      digitalWrite(GPIO19, rand()%2);
      digitalWrite(GPIO20, rand()%2);
      digitalWrite(GPIO21, rand()%2);
      digitalWrite(GPIO22, rand()%2);
      timer--;
      delay(300);
    }
    digitalWrite(GPIO19, 1);
    digitalWrite(GPIO20, 1);
    digitalWrite(GPIO21, 1);
    digitalWrite(GPIO22, 1);
    // 音声「ゲームを開始します」
    system("sudo aplay /home/xiao/pelmani/play_asset/mei_asset1.wav");
  
    gameState = waitFirstStep;
  }
}

// 各ターンの1手目が打たれるのを待っているときの処理
void onFirstStep(int* blockGotten, int* keys, vector<string> &wavfileList) {
  int choosing = -1;
  while(true){
    choosing = buttonSensing();
    //空きマス(0)を見つけたら選択中(2)に設定
    if(blockGotten[choosing] == 0){
      blockGotten[choosing] = 2;
      break;
    }
  }
  if(keys[choosing] == -1) {
    blockGotten[choosing] = 1;
    // ドボン！
    digitalWrite(choosing+19, 0);
    system("sudo aplay /home/xiao/wavmusic/badAnswer.wav");//仮の音声(ペア不一致とは分けたい)
    // 音声「次の人に交代してください」
    system("sudo aplay /home/xiao/pelmani/play_asset/mei_asset4.wav");
  } else {
    playReactSound(choosing,keys,wavfileList);
    // ボタンの点滅と次の手の待機は別プロセスで行う。
    // 共有メモリセグメント作成
    if ((segid = shmget(IPC_PRIVATE, 100, 0600)) == -1){
      perror( "shmget error." );
      exit( EXIT_FAILURE );
    }
    
    PID = fork();
    
    if(PID < 0){
      std::cout << "Fork Failed." << std::endl;
      exit(1);
    } else if (PID == 0){
      // ボタンの点滅(子プロセス)
      if ((segaddr = (int *)shmat(segid, NULL, 0)) == (void *)-1) {
	perror( "ChildProcess shmat error." );
	exit(EXIT_FAILURE);
      }
      *segaddr = 0;
      //共有メモリのdetach
      if (shmdt(segaddr) == -1) {
	perror( "ChildProcess shmdt error." );
	exit(EXIT_FAILURE);
      }
      while(true){
        digitalWrite(choosing+19, 0);
        delay(500);
        digitalWrite(choosing+19, 1);
        delay(500);
	if ((segaddr = (int *)shmat(segid, NULL, 0)) == (void *)-1) {
	  perror( "ChildProcess shmat error." );
	  exit(EXIT_FAILURE);
	}
	cout << *segaddr << endl;
	if(*segaddr == 1) break;
	//共有メモリのdetach
	if (shmdt(segaddr) == -1) {
	  perror( "ChildProcess shmdt error." );
	  exit(EXIT_FAILURE);	  
	}
      }
      exit(EXIT_SUCCESS);
      } else {
      // 1手目が打たれたら2手目の待機に移動(親プロセス)
      gameState = waitSecondStep;
    }
  }
  return;
}
 
// 各ターンの2手目が打たれるのを待っているときの処理
void onSecondStep(int* blockGotten, int* keys, vector<string> &wavfileList) {
  int choosing = -1;
  while(true){
    choosing = buttonSensing();
    if(blockGotten[choosing] == 0){
      blockGotten[choosing] = 2;
      if ((segaddr = (int *)shmat(segid, NULL, 0)) == (void *)-1) {
        perror( "ParentProcess shmat error." );
        exit(EXIT_FAILURE);
      }
      *segaddr = 1;
      //共有メモリのdetach
      if (shmdt(segaddr) == -1) {
        perror( "ParentProcess shmdt error." );
        exit(EXIT_FAILURE);
      }
      break;
    }
  }
  cout << "戻っておいで!" << endl;
  waitpid(PID,&status,0);
  if (WIFEXITED(status)){
    printf("exit, status=%d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)){
    printf("signal, sig=%d\n", WTERMSIG(status));
  } else {
    printf("abnormal exit\n");
  }
  for(int i = 0; i < 4; i++){
    //1手目の箇所を探査
    if(i == choosing) continue;
    if(blockGotten[i] == 2){
      if(keys[choosing] == -1){ // ドボンマスにヒット
        blockGotten[choosing] = 1;
	blockGotten[i] = 0;
        // ドボン！
        system("sudo aplay /home/xiao/wavmusic/badAnswer.wav");//仮の音声(ペア不一致とは分けたい)
        // 音声「次の人に交代してください」
        system("sudo aplay /home/xiao/pelmani/play_asset/mei_asset4.wav");
      } else if(keys[choosing] == keys[i]){ // ペアになってる！
	blockGotten[choosing] = 1;
	blockGotten[i] = 1;
        digitalWrite(i + 19, 0);
        digitalWrite(choosing + 19, 0);
        playReactSound(choosing,keys,wavfileList);
	system("sudo aplay /home/xiao/wavmusic/rightAnswer.wav");
        playReactSound(choosing,keys,wavfileList);
        playComingOut((keys[choosing] * 2 + 1), wavfileList);
      } else { //ペアになってない
	blockGotten[choosing] = 0;
	blockGotten[i] = 0;
        playReactSound(choosing,keys,wavfileList);
	system("sudo aplay /home/xiao/wavmusic/badAnswer.wav");
        // 音声「次の人に交代してください」
        system("sudo aplay /home/xiao/pelmani/play_asset/mei_asset4.wav");
      }
    }
  }

  if(judgeGameEnd(blockGotten)){    
    // ゲームが終了するならゲーム終了状態へ移行
    system("sudo aplay /home/xiao/wavmusic/gameEnd.wav");
    gameState = gameEnd;
  } else {
    // ゲームが続くなら1手目に戻る
    gameState = waitFirstStep;
  }
}

// ゲーム終了時の処理
void onGameEnd() {
  // ゲームをリスタートするならゲーム開始状態へ移行
  // 音声「ゲームを終了します」
  system("sudo aplay /home/xiao/pelmani/play_asset/mei_asset3.wav");
  while(true){
    // 実際はスライダーによるtxt受付へのモードチェンジを待つ
    if(buttonSensing()) break;
  }
  gameState = loadText;
}
