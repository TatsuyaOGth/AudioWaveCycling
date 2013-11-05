#include "testApp.h"

//debug
float debug1 = 0;

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetBackgroundAuto(false);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    
    //OSC settings
    reseiver.setup(OSC_RECEIVE_PORT);
    sender.setup(OSC_SEND_HOST, OSC_SEND_PORT);
    
    
    //----------
    // Camera setup
    //----------
//    mCamera.setNearClip(0.01f);
//	mCamera.setFarClip(1000.0f);
    mCamera.resetTransform();
    mCamera.setFov(60);
    mCamera.clearParent();
    
    //----------
    // 値の初期化
    //----------
    mPan = 0;
    mPanVec = 0;
    mPosition = 0;
    bDrawDebug = true;
    bDebugMode = true;
    mBackWhite = 80;
    mBackwaver = 2;
    mMusicID = 0;
    mMovieID = 0;
    
    //最初のデータを読み込む
    mCyclingMode = MOVIE; // 初期化のためあえてAUDIOにしない
    changeCyclingMode(AUDIO);
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    //----------
    // OSCメッセージを受信, 自転車の状態を更新
    // デバッグモード時はマウスポインタで操作
    //----------
    if (bDebugMode) {
        mPanVec = ofMap(mouseX, 0, ofGetWidth(), -1, 1, true);
        mPan += mPanVec;
        mPan = ofClamp(mPan, -1, 1);
        mPosition = -ofMap(mouseY, 0, ofGetHeight(), 0, 1, true) + 1.0f;
    } else {
        ofxOscMessage m;
        while (reseiver.getNextMessage(&m)) {
            if (m.getAddress() == "/pan") mPan = m.getArgAsFloat(0);
            if (m.getAddress() == "/panvec") mPanVec = m.getArgAsFloat(0);
            if (m.getAddress() == "/pos") mPosition = (double)m.getArgAsFloat(0);
            if (m.getAddress() == "/quick") mQuick = m.getArgAsFloat(0);
        }
    }
    
    mVisual.updateValues(mPan, mPosition, mQuick);
    
    switch (mCyclingMode) {
        case AUDIO:
            //----------
            // 波形を更新
            //----------
            // バッファサイズ分のサンプルを取得して最大値を計算→Buff配列を更新
            mSample.setPosition(mPosition);
            for (unsigned i = 0; i < BUFF_SIZE; i++) {
                if (mSample.getPosition() == 1) break;
                mBuffL[i] = mSample.update();
                mBuffR[i] = mSample.update();
            }
            mVisual.setAudioBufferPts(&mBuffL[0], &mBuffR[0]);
            break;
            
        case MOVIE:
            //----------
            // 動画を更新
            //----------
            mMovie.setPosition(mPosition);
            mMovie.update();
            break;
    }

}

//--------------------------------------------------------------
void testApp::draw(){
//    mVisual.background(ofColor(0), mQuick);
    //背景にゆらぎをつくる
    if (ofGetFrameNum() % BACK_UPDATE_INTERVAL == 0) {
        mBackWhite += mBackwaver;
        if (mBackWhite > BACK_GRAD_MAX || mBackWhite < BACK_GRAD_MIN) {
            mBackwaver *= -1;
        }
    }
    mVisual.backgroundGradient(ofColor(mBackWhite), ofColor(0), OF_GRADIENT_CIRCULAR, mQuick);

    switch (mCyclingMode) {
        case AUDIO:
        {
            //----------
            //道を描画
            //----------
            int W = 400; //ofsc
            int H = WAVE_SIZE; //ofsc
            int Z = -ROAD_LENGTH;
            
            const int pos = (int)((mLWave.size()-1) * mPosition);
            double * wl = &mLWave[pos];
            double * wr = &mRWave[pos];
            
            // Off-screen rendering
            fbo.begin();
            ofBackground(0,0,0,0);
            // ground
            ofFill();
            ofSetColor(0, 90);
            ofRect(0, 0, W, H);
            // Wave form
            ofNoFill();
            ofSetColor(255,255,255,255);
            for (int i = 0; i < H; i++) {
                if (pos + i >= mLWave.size()) break;
                float x = W/4;
                float y = H - i;
                ofLine(x-(*wl)*W/2, y, x+(*wl)*W/2, y);
                x = x * 3;
                ofLine(x-(*wr)*W/2, y, x+(*wr)*W/2, y);
                wl++;
                wr++;
            }
            fbo.end();
            
            //==========
            
            // カメラ
            float camX = mPan * (ROAD_WIDTH / 2);
            mCamera.setPosition(CAM_POS_X + camX, CAM_POS_Y, CAM_POS_Z);
            mCamera.lookAt(ofVec3f(mPanVec * CAM_TARGET_WIDTH, CAM_POS_Y, Z));
            
            mCamera.begin();
            
            ofPushMatrix();
            ofTranslate(-(ROAD_WIDTH / 2), 0);
            
            ofPushStyle();
            if (!mLWave.empty() && !mRWave.empty()) {
                for (int i = 1; i <= OBJ_WAVE_FORM_NUM; i++) {
                    float offset = pos % (int)(ROAD_LENGTH / OBJ_WAVE_FORM_NUM);
                    float z = -(i * (ROAD_LENGTH / OBJ_WAVE_FORM_NUM)) + offset;
                    mVisual.drawWave(ofPoint(0, 0, z), (mLWave[pos] + mRWave[pos]) * 0.5, 100, RECT);
                }
            }
            ofPopStyle();
            
            // FBO draw
            ofTexture & texture = fbo.getTextureReference();
            texture.bind();
            glBegin( GL_TRIANGLE_STRIP );
            glTexCoord2f( 0, 0 );
            glVertex3f( 0, 0, Z );
            glTexCoord2f( fbo.getWidth(), 0 );
            glVertex3f( ROAD_WIDTH, 0, Z );
            glTexCoord2f( 0, fbo.getHeight() );
            glVertex3f( 0, 0, 0 );
            glTexCoord2f( fbo.getWidth(), fbo.getHeight() );
            glVertex3f( ROAD_WIDTH, 0, 0 );
            glEnd();
            texture.unbind();
            ofPopMatrix();
            mCamera.end();
            break;
        }
        case MOVIE:
        {
            //----------
            //道を描画
            //----------
            int W = 160; //ofsc
            int movW = W;
            int movH = 320;
            int H = FILM_SIZE; //ofsc
            int Z = -9000;
            
            // Off-screen rendering
            fbo.begin();
            ofBackground(0,0,0,0);
            // ground
            ofFill();
            ofSetColor(0, 0, 0, 255);
            ofRect(0, 0, W, H);
            // frames
            ofSetColor(255, 255, 255, 255);
            for (int i = 0; i < H; i += (movH + 10)) {
                float tmp = mPosition * movH * FILM_INTERVAL;
                int offset = (int)tmp % (int)movH;
                mMovie.draw(5, i + offset, movW-10, movH);
            }
            fbo.end();
            
            //==========
            
            // カメラ
            float camX = mPan * (ROAD_WIDTH / 2);
            mCamera.setPosition(CAM_POS_X + camX, CAM_POS_Y, CAM_POS_Z);
            mCamera.lookAt(ofVec3f(mPanVec * CAM_TARGET_WIDTH, CAM_POS_Y, Z));
            
            // 奥の動画
            ofSetColor(255, 255, 255, 255);
//            ofPushMatrix();
//            ofTranslate(-(movW/2), -(movH/2), Z);
//            mMovie.draw(0, 0, movW, movH);
//            ofPopMatrix();
            mMovie.draw(0, 0, ofGetWidth(), ofGetHeight());
            
            mCamera.begin();
            
            ofPushMatrix();
            ofTranslate(-(ROAD_WIDTH / 2), 0);
            
            // FBO draw
            ofTexture & texture = fbo.getTextureReference();
            texture.bind();
            glBegin( GL_TRIANGLE_STRIP );
            glTexCoord2f( 0, 0 );
            glVertex3f( 0, 0, Z );
            glTexCoord2f( fbo.getWidth(), 0 );
            glVertex3f( ROAD_WIDTH, 0, Z );
            glTexCoord2f( 0, fbo.getHeight() );
            glVertex3f( 0, 0, 0 );
            glTexCoord2f( fbo.getWidth(), fbo.getHeight() );
            glVertex3f( ROAD_WIDTH, 0, 0 );
            glEnd();
            texture.unbind();
            ofPopMatrix();
            mCamera.end();
            break;
        }
    }
    
    
//    fbo.draw(0,0,ofGetWidth(), ofGetHeight());
    if (bDrawDebug) drawDebug();
}

/**
 デバッグ表示.
 */
void testApp::drawDebug()
{
    ofPushStyle();
    ofSetColor(255, 255, 255, 255);
    stringstream s;
    s << "show/hide debug draw = key 'd'" << endl;
    s << "controller mode: " << (bDebugMode ? "MOUSE" : "BICYCLE") << endl;
    s << "fps: " << ofGetFrameRate() << endl;
    s << "position: " << mPosition << endl;
    s << "quick turn: " << mQuick << endl;
    s << "|";
    for (int i = 0; i < 20; i++)
        s << (mPosition*20 > i ? "*" : "-");
    s << "|" << endl;
    s << "pan: " << mPan << endl;
    s << "|";
    for (int i = 0; i < 20; i++)
        s << (round((mPan+1)*9) == i ? "|" : "-");
    s << "|" << endl;
    s << "camera:" << endl;
    s << "x." << mCamera.getPosition().x << endl;
    s << "y." << mCamera.getPosition().y << endl;
    s << "z." << mCamera.getPosition().z << endl;
    s << "cycling mode: ";
    switch (mCyclingMode) {
        case AUDIO:
            s << "AUDIO" << endl;
            s << "music[" << mMusicID << "] " << MUSICS[mMusicID] << endl;
            break;
        case MOVIE:
            s << "MOVIE" << endl;
            break;
    }
//    s << "debug1: " << debug1 << endl;
    ofDrawBitmapString(s.str(), 20, 20);
    
    ofPopStyle();
}

void testApp::exit()
{
    mSample.clear();
}

//--------------------------------------------------------------
#pragma mark - keyPressed
void testApp::keyPressed(int key){
    switch (key) {
        case 'd': bDrawDebug = !bDrawDebug; break;
        case 'D': bDebugMode = !bDebugMode; break;
        case 'f': ofToggleFullscreen(); break;
        case '.': case '>':
            changeMusic(true);
            break;
        case ',': case '<':
            changeMusic(false);
            break;
        case '1': changeCyclingMode(AUDIO); break;
        case '2': changeCyclingMode(MOVIE); break;

        case OF_KEY_UP: debug1++; break;
        case OF_KEY_DOWN: debug1--; break;
    }
}
#pragma mark -
//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::changeCyclingMode(CyclingMode mode)
{
    if (mode == mCyclingMode) return;
    switch (mode) {
        case AUDIO:
        {
            // FBO settings
            ofFbo::Settings fboSettings = ofFbo::Settings();
            fboSettings.width = 400; //FBOサイズ
            fboSettings.height = WAVE_SIZE;
            fboSettings.internalformat = GL_RGBA; //画像フォーマット
            fbo.allocate(fboSettings);
            // load default file
            loadSample(mMusicID);
            break;
        }
        case MOVIE:
        {
            // FBO settings
            ofFbo::Settings fboSettings = ofFbo::Settings();
            fboSettings.width = 160; //FBOサイズ
            fboSettings.height = FILM_SIZE;
            fboSettings.internalformat = GL_RGBA; //画像フォーマット
            fbo.allocate(fboSettings);
            // load default file
            loadMovie(0);
            break;
        }
    }
    mCyclingMode = mode;
}

/**
 曲チェンジ
 */
void testApp::changeMusic(const bool up)
{
    up ? mMusicID++ : mMusicID--;
    mMusicID = mMusicID % MUSIC_NUM; //loop
    loadSample(mMusicID);
}
/**
 曲を読み込む
 */
bool testApp::loadSample(const int music_id)
{
    if (music_id >= MUSIC_NUM) return false;
    if (mSample.getIsLoaded()) {
        mLWave.clear();
        mRWave.clear();
        mSample.clear();
    }
    assert(mSample.load(MUSICS[music_id]));
    assert(mSample.getChannels() == 2); //とりあえずステレオ限定
    mSample.setPosition(0);
    mSample.play();
    const float second = (mSample.getLength() / (float)mSample.getSampleRate()) * 0.5;
    while (mSample.getPosition() != 1.0) {
        double maxL = 0;
        double maxR = 0;
        int i = 0;
        while (i < BUFF_SIZE) {
            if (mSample.getPosition() == 1.0) break;
            const double l = abs(mSample.update());
            const double r = abs(mSample.update());
            if (maxL < l) maxL = l;
            if (maxR < r) maxR = r;
            i++;
        }
        mLWave.push_back(maxL);
        mRWave.push_back(maxR);
    }
    assert(mLWave.size() == mRWave.size());
    ofLogVerbose("load music: " + MUSICS[music_id]);
    sendMusicId(mMusicID);
    return true;
}
/**
 */
void testApp::sendMusicId(const int ID)
{
    ofxOscMessage m;
    m.setAddress("/music_id");
    m.addIntArg(ID);
    sender.sendMessage(m);
}


/**
 動画を読み込む
 */
bool testApp::loadMovie(const int movie_id)
{
    if (movie_id >= MOVIE_NUM) return false;
    if (mMovie.isLoaded()) return true;
    assert(mMovie.loadMovie(MOVIES[movie_id]));
//    mMovie.play();
    ofxOscMessage m;
    m.setAddress("/movie_id");
    m.addIntArg(movie_id);
    sender.sendMessage(m);

    return true;
}


