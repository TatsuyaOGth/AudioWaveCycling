#pragma once

#include "ofMain.h"
#include "ofxSamplePlayer.h"
#include "ofxOsc.h"
#include "VisualFx.h"
#include "constants.h"

enum CyclingMode {
    AUDIO = 0,
    MOVIE,
};

class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    bool loadSample(const int music_id);
    bool loadMovie(const int movie_id);
    void changeMusic(const bool up);
    void drawDebug();
    void sendMusicId(const int ID);
    void changeCyclingMode(const CyclingMode mode); //setup as each mode
    
private:
    
    // Bicyble Parameters
    float mPan; //0.0 - 1.0, 0.5=center
    float mPanVec; //左右方向への力
    double mPosition;
    float mQuick;
    
    // Audio
    ofxSamplePlayer mSample;
    vector<double> mLWave;
    vector<double> mRWave;
    double mBuffL[BUFF_SIZE];
    double mBuffR[BUFF_SIZE];
    int mMusicID;
    
    // Movie
    ofVideoPlayer mMovie;
    int mMovieID;
    
    ofCamera mCamera;
    int mBackWhite;
    int mBackwaver;
    
    bool bDrawDebug;
    bool bDebugMode;
    
    CyclingMode mCyclingMode;
        
    ofFbo fbo;
    ofxOscReceiver reseiver;
    ofxOscSender sender;
    
    VisualFx mVisual;

};
