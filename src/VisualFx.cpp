//
//  VisualFx.cpp
//  AudioCycling
//
//  Created by Tatsuya Ogusu on 2013/11/02.
//
//

#include "VisualFx.h"

VisualFx::VisualFx():
mBufferSize(256),
mPan(0),
mPos(0),
mQuick(0)
{
    shader.load("","shaders/basic.frag");
}

VisualFx::~VisualFx()
{
}


/**
 Waveフォームをヴィジュアライズ
 */
void VisualFx::drawWave(const ofPoint& pos, const double vol, const float maxSize, waveObjMode mode)
{
    ofNoFill();
    ofColor colL = ofColor(0, 255, 255);
    ofColor colR = ofColor(255, 255, 0);
    int h = ofGetHeight()/2;
    float vL = mPan > 0 ? (-mPan + 1) : 1.0;
    float vR = mPan < 0 ? -(abs(mPan)) + 1 : 1.0;
    ofPushMatrix();
    ofTranslate(pos);
    for (unsigned i = 0; i < BUFF_SIZE; i++) {
        int x = (int)ofMap(i, 0, BUFF_SIZE, 0, ROAD_WIDTH);
        switch (mode) {
            case CIRCLE:
                ofSetColor(colL, 180 * vL);
                ofCircle(x, (mBuffL[i] * h * vL), vol * maxSize * vL);
                ofSetColor(colR, 180 * vR);
                ofCircle(x, (mBuffR[i] * h * vR), vol * maxSize * vR);
                break;
            case RECT:
                ofSetRectMode(OF_RECTMODE_CENTER);
                ofSetColor(colL, 180 * vL);
                ofRect(x, (mBuffL[i] * h * vL), vol * maxSize * vL, vol * maxSize * vL);
                ofSetColor(colR, 180 * vR);
                ofRect(x, (mBuffR[i] * h * vR), vol * maxSize * vR, vol * maxSize * vR);
                break;
        }
    }
    ofPopMatrix();
}



/**
 背景（エコー効果）
 @param col
 @param echo 0~1
 */
void VisualFx::background(const ofColor& col, const float echo)
{
    if (echo == 0) {
        ofBackground(col);
    } else {
        ofFill();
        ofSetColor(col, (int)((-echo + 1) * 255));
        ofRect(0, 0, ofGetWidth(), ofGetHeight());
    }
}

/**
 背景（エコー効果・グラデーション）
 @param col1, cal2, mode
 @param echo 0~1
 */
void VisualFx::backgroundGradient(const ofColor &col1, const ofColor &col2, ofGradientMode mode, const float echo)
{
    if (echo == 0) {
        ofBackgroundGradient(col1, col2, mode);
    } else {
        int a = (int)((-echo + 1) * 255);
        ofBackgroundGradient(ofColor(col1, a), ofColor(col2, a), mode);
    }
}

