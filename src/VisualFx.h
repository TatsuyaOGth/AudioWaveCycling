//
//  VisualFx.h
//  AudioCycling
//
//  Created by Tatsuya Ogusu on 2013/11/02.
//
//

#ifndef __AudioCycling__VisualFx__
#define __AudioCycling__VisualFx__

#include "ofMain.h"
#include "constants.h"

struct myObj {
    ofPoint pos;
    ofColor col;
};

enum waveObjMode {
    CIRCLE = 0,
    RECT,
};

class VisualFx {
public:
    
    VisualFx();
    virtual ~VisualFx();
    
    void updateValues(const float pan, const double pos, const float quick)
    {
        mPan = pan;
        mPos = pos;
        mQuick = quick;
    }

    void setAudioBufferPts(double * L, double * R)
    {
        mBuffL = L;
        mBuffR = R;
    }
    
    void drawWave(const ofPoint& pos, const double vol, const float maxSize, waveObjMode mode);

    
    void background(const ofColor& col, const float echo);
    void backgroundGradient(const ofColor& col1, const ofColor& col2, ofGradientMode mode, const float echo);
    
private:
    
    float mPan;
    double mPos;
    float mQuick;
    
    // VFX Object
    vector<myObj> mGrandLines;
    
    // Audio
    int mBufferSize;
    double * mBuffL;
    double * mBuffR;
    
    // Background
    
    
    ofShader shader;
    
};

#endif /* defined(__AudioCycling__VisualFx__) */
