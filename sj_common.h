/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"


/************************************************************
************************************************************/
enum{
	WIDTH = 1280,
	HEIGHT = 720,
	
	MARGIN = 1,
};
enum{
	NUM_POINTS = WIDTH,
};
enum{
	THREAD_SLEEP_MS = 1,
	// THREAD_SLEEP_MS = 1,
};

/************************************************************
class
************************************************************/

/**************************************************
**************************************************/
class THREAD__DOUBLE : public ofThread{
private:
	double val;
	
	bool b_set;
	double NewVal;

public:
	THREAD__DOUBLE()
	{
		val = 0;
	}

	/******************************
	description
		need lock()/unlock()
	******************************/
	double getVal(){
		return val;
	}
	
	/******************************
	description
		need lock()/unlock()
	******************************/
	void set(double _val){
		if(isThreadRunning()){
			NewVal = _val;
			b_set = true;
		}
	}
	
	void threadedFunction(){
		while(isThreadRunning()) {
			bool b_set_Copy;
			
			lock();
			if(b_set){
				val = NewVal;
				b_set = false;
			}
			b_set_Copy = b_set;
			unlock();
			
			if(!b_set_Copy){
				if(THREAD_SLEEP_MS == 0)	yield();
				else						sleep(THREAD_SLEEP_MS);
			}
		}
    }
};

/**************************************************
**************************************************/
class THREAD__VERTS : public ofThread{
private:
	
	ofVec3f Verts[NUM_POINTS];
	
	bool b_set;
	double NewVal;
	
	bool b_TimeReverse;
	
public:
	THREAD__VERTS()
	: b_TimeReverse(false)
	{
		for(int i = 0; i < NUM_POINTS; i++){
			Verts[i].set(i, 0);
		}
	}

	/******************************
	description
		need lock()/unlock()
	******************************/
	const ofVec3f& operator[] (const int i){
		return Verts[i];
	}
	
	/******************************
	description
		need lock()/unlock()
	******************************/
	void set(double _val){
		if(isThreadRunning()){
			NewVal = _val;
			b_set = true;
		}
	}
	
	void threadedFunction(){
		while(isThreadRunning()) {
			bool b_set_Copy;
			
			lock();
			b_set_Copy = b_set;
			unlock();
			
			if(b_set_Copy){
				lock();
				if(b_TimeReverse){
					for(int i = NUM_POINTS - 1; 0 < i; i--){
						Verts[i].set(i, Verts[i-1].y);
					}
					Verts[0].set(0, NewVal);
				}else{
					for(int i = 0; i < NUM_POINTS - 1; i++){
						Verts[i].set(i, Verts[i + 1].y);
					}
					Verts[NUM_POINTS - 1].set(NUM_POINTS - 1, NewVal);
				}
				
				b_set = false;
				unlock();
				
			}else{
				if(THREAD_SLEEP_MS == 0)	yield();
				else						sleep(THREAD_SLEEP_MS);
			}
		}
    }
};
