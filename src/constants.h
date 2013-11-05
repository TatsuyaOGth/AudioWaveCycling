//
//  constants.h
//  AudioCycling
//
//  Created by Tatsuya Ogusu on 2013/11/02.
//
//

#ifndef AudioCycling_constants_h
#define AudioCycling_constants_h

#include "ofMain.h"


// OSC
static const int OSC_RECEIVE_PORT = 8010;
static const string OSC_SEND_HOST = "127.0.0.1";
static const int OSC_SEND_PORT = 8011;

// カメラの定位置
static const float CAM_POS_X = 0;
static const float CAM_POS_Y = 200;
static const float CAM_POS_Z = 0;
static const float CAM_TARGET_WIDTH = 4000; //ハンドルを傾けた時に視界を横にふる割合（適当）

// 道
static const float ROAD_WIDTH = 1000; //FBO描画時の道幅
static const float ROAD_LENGTH = 9000; //FBO描画時の道の長さ

// 背景のゆらぎ
static const int BACK_UPDATE_INTERVAL = 2;
static const int BACK_GRAD_MIN = 60;
static const int BACK_GRAD_MAX = 140;

//----------
// Audio Wave Cycling
//----------
static const int BUFF_SIZE = 256;
static const int WAVE_SIZE = 3000;

// 読み込む曲
static const string MUSICS[] = {
    "sample.wav",
    "techno.wav",
    "se.wav"
};
static const int MUSIC_NUM = sizeof(MUSICS) / sizeof(MUSICS[0]); //読み込んだ曲数

// 横切るもの
static const float OBJ_WAVE_FORM_NUM = 10 ; //一度に見えるWaveFormの数


//----------
// Muvie Frame Cycling
//----------
static const int FILM_INTERVAL = 16;
static const int FILM_SIZE = 3000;

// 読み込む曲
static const string MOVIES[] = {
    "LumiereFilms.mov",
//    "sample.mov",
};
static const int MOVIE_NUM = sizeof(MOVIES) / sizeof(MOVIES[0]); //読み込んだ動画数



//------------------------------
// my utils
//------------------------------
float mouseNX(float max = 1){ return ((float)ofGetMouseX() / (float)ofGetWidth()) * max; };
float mouseNY(float max = 1){ return ((float)ofGetMouseY() / (float)ofGetHeight()) * max; };

#endif
