/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxGui.h"
#include "sj_common.h"

#include "Dmx.h"


/************************************************************
************************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	/********************
	********************/
	int soundStream_DeviceId;
	ofSoundStream soundStream;
	
	ofxPanel gui;
	ofxFloatSlider LPF_Amb_ThreshSec; // 大:緩やか
	ofxFloatSlider LPF_Sound_ThreshSec; // 大:緩やか
	
	ofxToggle b_DownLimit;
	ofxFloatSlider SoundFilter_DownPerSec; // 小:緩やか...つくられた動き
	
	ofxToggle b_NonLinear;
	ofxFloatSlider SoundFilter_Thresh_NonLinear; // 不感帯の設定
	
	ofxToggle b_AGC;
	ofxFloatSlider MonitorGain_AGC_Target;
	ofxFloatSlider MonitorGain;
	
	ofxToggle b_Dmx;
	ofxFloatSlider DmxGain;
	ofxToggle b_AmbCancel;
	
	ofxColorSlider DmxFullColor;
	
	
	/********************
	********************/
	ofVbo Vbo_SoundLevel;
	THREAD__VERTS Thread_Verts_SoundLevel;
	ofFloatColor Color_SoundLevel[NUM_POINTS];
	
	ofVbo Vbo_AmbLevel;
	THREAD__VERTS Thread_Verts_AmbLevel;
	ofFloatColor Color_AmbLevel[NUM_POINTS];
	
	ofColor Dmx_OutputColor;
	THREAD__DOUBLE Thread_SoundLevel_Dmx;
	
	/********************
	********************/
	int png_id;
	bool b_Reset;
	
	/********************
	********************/
	DMX dmx;
	
	
	/****************************************
	****************************************/
	int Limit_ColorVal(int val);
	void clear_VboSetting_gl();
	
public:
	/****************************************
	****************************************/
	ofApp(int _soundStream_DeviceId);
	void audioReceived(float *input, int bufferSize, int nChannels);
	
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
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
