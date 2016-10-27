/************************************************************
************************************************************/
#include "Dmx.h"

/************************************************************
************************************************************/
static char targetIP[] = "10.7.157.106";

/************************************************************
************************************************************/

/******************************
******************************/
void DMX::setup(){
	/********************
	********************/
	//at first you must specify the Ip address of this machine
	artnet.setup("10.0.0.2"); //make sure the firewall is deactivated at this point
	
	/********************
	********************/
	for(int i = 0; i < DMX_SIZE; i++){
		data[i] = 0;
	}
}

/******************************
******************************/
void DMX::update(const ofColor& color){

	data[0] = color.a;
	data[1] = color.r;
	data[2] = color.g;
	data[3] = color.b;
}

/******************************
******************************/
void DMX::exit(){
	for(int i = 0; i < DMX_SIZE; i++){
		data[i] = 0;
	}
    artnet.sendDmx(targetIP, data, DMX_SIZE);
}

/******************************
******************************/
void DMX::draw(){
	/********************
	********************/
    // list nodes for sending
    // with subnet / universe
	// artnet.sendDmx("10.0.0.149", 0xf, 0xf, testImage.getPixels(), 512);
    artnet.sendDmx(targetIP, data, DMX_SIZE);
}



