/************************************************************
************************************************************/
#include "ofApp.h"



/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _soundStream_DeviceId)
: soundStream_DeviceId(_soundStream_DeviceId)
, b_Reset(false)
{
}

//--------------------------------------------------------------
void ofApp::setup(){
	/********************
	********************/
	ofSetVerticalSync(false);
	ofSetFrameRate(30);
	ofSetWindowShape(WIDTH, HEIGHT);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableSmoothing();
	
	/********************
	********************/
	soundStream.listDevices();
	if(soundStream_DeviceId == -1){
		ofExit();
	}
	soundStream.setDeviceID(soundStream_DeviceId);
	soundStream.setup(this, 0, 1, 44100, 1024, 2);
	
	for(int i = 0; i < NUM_POINTS; i++){
		Color_SoundLevel[i].set(0.1, 0.8, 1.0, 0.8);
		Color_AmbLevel[i].set(1.0, 1.0, 1.0, 0.8);
	}
	Vbo_SoundLevel.setVertexData(&Thread_Verts_SoundLevel[0], NUM_POINTS, GL_DYNAMIC_DRAW);
	Vbo_SoundLevel.setColorData(Color_SoundLevel, NUM_POINTS, GL_DYNAMIC_DRAW);
	
	Vbo_AmbLevel.setVertexData(&Thread_Verts_AmbLevel[0], NUM_POINTS, GL_DYNAMIC_DRAW);
	Vbo_AmbLevel.setColorData(Color_AmbLevel, NUM_POINTS, GL_DYNAMIC_DRAW);
	
	/********************
	********************/
	gui.setup();
	gui.add(LPF_Amb_ThreshSec.setup("LPF Amb Th_Sec", 10.0, 0, 25.0));
	gui.add(LPF_Sound_ThreshSec.setup("LPF Snd Th_Sec", 0.3, 0, 1.5));
	
	gui.add(b_DownLimit.setup("b_DownLimit", false));
	gui.add(SoundFilter_DownPerSec.setup("DownPerSec", 40, 10, 50));
	
	gui.add(b_NonLinear.setup("b_NonLinear", false));
	gui.add(SoundFilter_Thresh_NonLinear.setup("NonLinear", 0.01, 0, 0.3));
	
	gui.add(b_AGC.setup("b_AGC", false));
	gui.add(MonitorGain_AGC_Target.setup("AGC Target", 0.1, 0.01, 1));
	
	gui.add(MonitorGain.setup("MonitorGain", 8, 1, 20));
	gui.add(DmxGain.setup("DmxGain", 2, 1, 10));
	gui.add(b_AmbCancel.setup("b_AmbCancel", false));
	
	/* */
	ofColor initColor = ofColor(0, 127, 255, 255);
	ofColor minColor = ofColor(0, 0, 0, 0);
	ofColor maxColor = ofColor(255, 255, 255, 255);
	
	gui.add(DmxFullColor.setup("DmxFullColor", initColor, minColor, maxColor));
	
	/********************
	********************/
	Thread_Verts_SoundLevel.startThread(true, false); // blocking, non verbose
	Thread_Verts_AmbLevel.startThread(true, false); // blocking, non verbose
	Thread_SoundLevel_Dmx.startThread(true, false); // blocking, non verbose
	
	/********************
	********************/
	png_id = 0;
	
	/********************
	********************/
	dmx.setup();
}

/******************************
******************************/
void ofApp::exit()
{
	/********************
	********************/
	dmx.exit();
	
	/********************
	********************/
	try{
		/********************
		stop済みのthreadをさらにstopすると、Errorが出るようだ。
		********************/
		if(Thread_Verts_SoundLevel.isThreadRunning()){
			Thread_Verts_SoundLevel.stopThread();
			while(Thread_Verts_SoundLevel.isThreadRunning()){
				Thread_Verts_SoundLevel.waitForThread(true);
			}
		}
		
		if(Thread_Verts_AmbLevel.isThreadRunning()){
			Thread_Verts_AmbLevel.stopThread();
			while(Thread_Verts_AmbLevel.isThreadRunning()){
				Thread_Verts_AmbLevel.waitForThread(true);
			}
		}
		
		if(Thread_SoundLevel_Dmx.isThreadRunning()){
			Thread_SoundLevel_Dmx.stopThread();
			while(Thread_SoundLevel_Dmx.isThreadRunning()){
				Thread_SoundLevel_Dmx.waitForThread(true);
			}
		}
		
	}catch(...){
		printf("Thread exiting Error\n");
	}
	
	/********************
	********************/
	printf("Good bye\n");
	
	std::exit(1);
}

//--------------------------------------------------------------
void ofApp::update(){
	/* */
	Thread_Verts_SoundLevel.lock();
	Vbo_SoundLevel.updateVertexData(&Thread_Verts_SoundLevel[0], NUM_POINTS);
	Thread_Verts_SoundLevel.unlock();

	Thread_Verts_AmbLevel.lock();
	Vbo_AmbLevel.updateVertexData(&Thread_Verts_AmbLevel[0], NUM_POINTS);
	Thread_Verts_AmbLevel.unlock();
	
	/* */
	Thread_SoundLevel_Dmx.lock();
	double SoundLevel_Dmx = Thread_SoundLevel_Dmx.getVal();
	Thread_SoundLevel_Dmx.unlock();
	
	ofColor currentColor = DmxFullColor;
	Dmx_OutputColor.a = currentColor.a;
	Dmx_OutputColor.r = Limit_ColorVal( int(currentColor.r * SoundLevel_Dmx) );
	Dmx_OutputColor.g = Limit_ColorVal( int(currentColor.g * SoundLevel_Dmx) );
	Dmx_OutputColor.b = Limit_ColorVal( int(currentColor.b * SoundLevel_Dmx) );
	printf("%5.2f\r", SoundLevel_Dmx);
	fflush(stdout);
	
	dmx.update(Dmx_OutputColor);
}

/******************************
******************************/
int ofApp::Limit_ColorVal(int val)
{
	if(val < 0)			val = 0;
	else if(255 < val)	val = 255;
	
	return val;
}

//--------------------------------------------------------------
void ofApp::draw(){
	/********************
	********************/
	ofBackground(20);

	/********************
	********************/
	dmx.draw();
	
	/********************
	********************/
	ofPushMatrix();
	
	ofTranslate(0, ofGetHeight() - MARGIN);
	ofScale(1, -1, 1);
	
	glPointSize(1.5);
	Vbo_SoundLevel.draw(GL_LINE_STRIP, 0, NUM_POINTS);
	Vbo_AmbLevel.draw(GL_LINE_STRIP, 0, NUM_POINTS);
	
	ofPopMatrix();
	
	/********************
	********************/
	gui.draw();
	
	/*
	printf("> FrameRate = %5.1f\r", ofGetFrameRate());
	fflush(stdout);
	*/
}

/******************************
******************************/
void ofApp::audioReceived(float *input, int bufferSize, int nChannels)
{
	/********************
	********************/
	double v = 0;
	for(int i = 0; i < bufferSize * nChannels; i++){
		v += input[i] * input[i];
	}
	v = sqrt(v / bufferSize);
	
	/********************
	********************/
	const int thresh_count_start = 5;
	static int c_INT = 0;
	
	static double Last_v_Sound = 0;
	static double Last_v_Amb = 0;
	
	if(b_Reset){
		c_INT = 0;
		b_Reset = false;
	}
	
	static float t_LastINT = 0;
	
	float now = ofGetElapsedTimef();
	
	if(c_INT < thresh_count_start){
		c_INT++;
		
		static double v_sum = 0;
		v_sum += v;
		
		if(thresh_count_start <= c_INT){
			Last_v_Sound = v_sum / thresh_count_start;
			Last_v_Amb = v_sum / thresh_count_start;
			
			v_sum = 0;
		}
		
	}else{
		double dt = now - t_LastINT;
		double v_Amb = v;
		double v_Sound = v;
		
		/********************
		********************/
		double alpha;
		if( (LPF_Amb_ThreshSec == 0) || (LPF_Amb_ThreshSec < dt) ){
			alpha = 1;
		}else{
			alpha = 1/LPF_Amb_ThreshSec * dt;
		}
		v_Amb = alpha * v_Amb + (1 - alpha) * Last_v_Amb;
		Last_v_Amb = v_Amb;
	
		/********************
		********************/
		if( (LPF_Sound_ThreshSec == 0) || (LPF_Sound_ThreshSec < dt) ){
			alpha = 1;
		}else{
			alpha = 1/LPF_Sound_ThreshSec * dt;
		}
		v_Sound = alpha * v_Sound + (1 - alpha) * Last_v_Sound;
		
		/********************
		********************/
		if(b_DownLimit){
			double v_DownLine = Last_v_Sound * (1 - SoundFilter_DownPerSec * dt);
			v_Sound = max( v_Sound, v_DownLine );
		}
		
		/********************
		********************/
		if(b_NonLinear){
			double diff = v_Sound - Last_v_Sound;
			if(SoundFilter_Thresh_NonLinear != 0){
				if( (0 < diff) && (diff < SoundFilter_Thresh_NonLinear) ){
					diff = pow(diff, 2) / SoundFilter_Thresh_NonLinear;
				}else if( (-SoundFilter_Thresh_NonLinear < diff) && (diff < 0) ){
					diff = -pow(diff, 2) / SoundFilter_Thresh_NonLinear;
				}
			}
			v_Sound = Last_v_Sound + diff;
		}
		
		Last_v_Sound = v_Sound;
		
		/********************
		********************/
		if(b_AGC){
			const double thresh = 0.01;
			if(v_Amb < thresh){
				MonitorGain = MonitorGain_AGC_Target / thresh;
			}else{
				MonitorGain = MonitorGain_AGC_Target / v_Amb;
			}
		}
	
		/********************
		********************/
		double Disp_SoundLevel = ofMap(v_Sound * MonitorGain, 0, 1, 0, HEIGHT - MARGIN);
		double Disp_AmbLevel = ofMap(v_Amb * MonitorGain, 0, 1, 0, HEIGHT - MARGIN);
		
		Thread_Verts_SoundLevel.lock();
		Thread_Verts_SoundLevel.set(Disp_SoundLevel);
		Thread_Verts_SoundLevel.unlock();
		
		Thread_Verts_AmbLevel.lock();
		Thread_Verts_AmbLevel.set(Disp_AmbLevel);
		Thread_Verts_AmbLevel.unlock();
		
		/********************
		********************/
		// double Dmx_SoundLevel = ofMap(v_Sound * MonitorGain * DmxGain, 0, 1, 0, 255, true);
		// double Dmx_SoundLevel = v_Sound * MonitorGain * DmxGain;
		double Dmx_SoundLevel;
		if(b_AmbCancel)	Dmx_SoundLevel = max((v_Sound - v_Amb), 0.0) * MonitorGain * DmxGain + v_Amb * MonitorGain;
		else			Dmx_SoundLevel = v_Sound * MonitorGain * DmxGain;
		
		Thread_SoundLevel_Dmx.lock();
		Thread_SoundLevel_Dmx.set(Dmx_SoundLevel);
		Thread_SoundLevel_Dmx.unlock();
	}
	
	t_LastINT = now;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case ' ':
		{
			enum{
				BUF_SIZE = 512,
			};
			char buf[BUF_SIZE];
			
			sprintf(buf, "image_%d.png", png_id);
			ofSaveScreen(buf);
			printf("%s saved\n");
			
			png_id++;
		}
			break;
			
		case 'r':
			b_Reset = true;
			printf("\nReset\n");
			break;
			
		case 's':
			gui.saveToFile("gui.xml");
			printf("save setting\n");
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
