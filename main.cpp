#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( int argc, char** argv ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	
	int soundStream_DeviceId;
	if(argc < 2){
		soundStream_DeviceId = -1;
	}else{
		soundStream_DeviceId = atoi(argv[1]);
	}
	ofRunApp(new ofApp(soundStream_DeviceId));

}
