#include "ofApp.h"


void FluidEffect::setup(int w, int h, float screenToFluidScale)
{
	width=w;
	height=h;


	// Initial Allocation
	//
	// note: we make the size of the simulation independent of the screen size, so that we can test
	// everything in the right proportions and at the right resolution, even before we have the 
	// triple-beamer, double-kinect setup..
	fluid.allocate(width, height, screenToFluidScale); 
    
	_velocityMap.allocate(fluid.getVelocityFbo().getWidth(), fluid.getVelocityFbo().getHeight(), ofImageType::OF_IMAGE_COLOR);
	_fboReader.setAsync(true);

	// Seting the gravity set up & injecting the background image
	//
	fluid.dissipation = 0.99;
	fluid.velocityDissipation = 0.99;
    
	fluid.setGravity(ofVec2f(0.0,-0.005));
//    fluid.setGravity(ofVec2f(0.0,0.0098));
    
	//  Set obstacle
	//
	//fluid.begin();
	//ofSetColor(0,0);
	//ofSetColor(255);
	//ofCircle(width*0.5, height*0.35, 40);
	//fluid.end();
	//fluid.setUseObstacles(true);
    
	// Adding constant forces
	//
	fluid.addConstantForce(ofPoint(width*0.5,height*0.85), ofPoint(0,-2), ofFloatColor(0.5,0.1,0.0), 3.f);

	// register event listeners
	ofAddListener(appSettings::instance().timeline.events().bangFired, this, &FluidEffect::receivedBang);

}