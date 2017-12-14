#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wiringPi.h>

#define GPIO23 23
#define GPIO24 24
#define GPIO25 25
#define GPIO26 26

void setPair(int* pair, int arrayNum);
int buttonSensing();
int judgeGameEnd(int* blockGotten);
void onGameStart(int* blockGotten, int* keys);
void onFirstStep(int* blockGotten, int* keys);
void onSecondStep(int* blockGotten, int* keys);
void onGameEnd();

enum state {
  gameStart,
  waitFirstStep,
  waitSecondStep,
  gameEnd,
};

enum state gameState = gameStart;

void setPair(int* pair, int arrayNum){
  int flag = 1;
  int place;
  srand((unsigned)time(NULL));
  for(int i = 0; i < arrayNum/2 ; i++){
    for(int j = 0 ; j < 2 ; j++){
      place = rand() % 4;
      while(true){
      	if(pair[place] == -1) {
	  pair[place] = i;
	  break;
	}
	place = (place + 1) % 4;
      }
    }
  }
  return;
}

int main(void){
  int status;
  int blockGotten[4] = {0,0,0,0};
  int keys[4] = {-1,-1,-1,-1};
  
  if(wiringPiSetupGpio() == -1) return 1;

  pinMode(GPIO23,INPUT);
  pinMode(GPIO24,INPUT);
  pinMode(GPIO25,INPUT);
  pinMode(GPIO26,INPUT);

  while(true) {
    switch(gameState) {
      // ゲーム開始
    case gameStart:
      onGameStart(blockGotten, keys);
      break;

      // 各ターンの1手目
    case waitFirstStep:
      onFirstStep(blockGotten, keys);
      break;

      // 各ターンの2手目
    case waitSecondStep:
      onSecondStep(blockGotten, keys);
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
  int status[4] = {0,0,0,0};
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

void playReactSound(int choosing, int* keys){
  switch(keys[choosing]){
  case 0:
    system("sudo aplay /home/xiao/wavmusic/choose0.wav");
    break;
  case 1:
    system("sudo aplay /home/xiao/wavmusic/choose1.wav");
    break;
  default:
    break;
  }
}

int judgeGameEnd(int *blockGotten){
  for(int i = 0; i < 4; i++){
    if(blockGotten[i]!=1) return 0;
  }
  return 1;
}

// ゲーム開始時の処理
void onGameStart(int* blockGotten, int* keys){
  for(int i = 0; i< 4; i++){
    blockGotten[i] = 0;
    keys[i] = -1;
  }
  setPair(keys, 4);
  
  gameState = waitFirstStep;
}

// 各ターンの1手目が打たれるのを待っているときの処理
void onFirstStep(int* blockGotten, int* keys) {
  int choosing = -1;
  while(true){
    choosing = buttonSensing();
    if(blockGotten[choosing]==0){
      blockGotten[choosing] = 2;
      playReactSound(choosing,keys);
      break;
    }
  }
  // 1手目が打たれたら2手目の待機に移動
  gameState = waitSecondStep;
}
 
// 各ターンの2手目が打たれるのを待っているときの処理
void onSecondStep(int* blockGotten, int* keys) {
  // まだゲームが終了しないなら1手目に戻る
  int choosing = -1;
  while(true){
    choosing = buttonSensing();
    if(blockGotten[choosing]==0){
      blockGotten[choosing] = 2;
      playReactSound(choosing,keys);
      break;
    }
  }
  for(int i = 0; i < 4; i++){
    if(i == choosing) continue;
    if(blockGotten[i] == 2){
      if(keys[choosing] == keys[i]){
	blockGotten[choosing] = 1;
	blockGotten[i] = 1;
	system("sudo aplay /home/xiao/wavmusic/rightAnswer.wav");
      } else {
	blockGotten[choosing] = 0;
	blockGotten[i] = 0;
	system("sudo aplay /home/xiao/wavmusic/badAnswer.wav");
      }
    }
  }

  if(judgeGameEnd(blockGotten)){    
    // ゲームが終了するならゲーム終了状態へ移行
    gameState = gameEnd;
  } else {
    // ゲームが続くなら1手目に戻る
    gameState = waitFirstStep;
  }
}

// ゲーム終了時の処理
void onGameEnd() {
  system("sudo aplay /home/xiao/wavmusic/gameEnd.wav");
  // ゲームをリスタートするならゲーム開始状態へ移行
  while(true){
    if(buttonSensing()) break;
  }
  gameState = gameStart;
}
