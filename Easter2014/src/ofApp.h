#pragma once

#include <cmath>

#include "ofMain.h"
#include "ofxFluid.h"
#include "ofxGaussianBlur.h"
#include "ofxKinectCommonBridge.h"
#include "ofxTimeline.h"
#include "ofxGui.h"
#include "ofxGlow.h"

#include "ParticleSystem/Particle.h"
#include "ParticleSystem/ParticleManager.h"
#include "ParticleSystem/ParticleGenerator.h"
#include "ParticleSystem/ParticleRenderer.h"
#include "ParticleSystem/ParticleAnimator.h"
#include "ParticleSystem/ForceField.h"
#include "ParticleSystem/SimulationAnimator.h"
#include "KinectForProjection.h"
#include "FluidEffect.h"
#include "ParticleEffect.h"

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
	void exit();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);


	FluidEffect fluidEffect;
    ParticleEffect particleEffect;

    ofVec2f oldM;

	// Kinect
	int nrOfKinects;
	KinectForProjection* kinectForProjection[2];

	// Control
	ofxTimeline timeline;

	ofxPanel gui;
	ofxFloatSlider kinectVerticalOffset0;
	ofxFloatSlider kinectForwardOffset0;
	ofxFloatSlider toPresentationSpaceFocalLength0;
	ofxFloatSlider toPresentationSpacePrincipalX0;
	ofxFloatSlider toPresentationSpacePrincipalY0;
	//ofxColorSlider color;
	//ofxVec2Slider center;
	//ofxIntSlider circleResolution;
	//ofxToggle filled;
	//ofxButton twoCircles;
	//ofxButton ringButton;
	//ofxLabel screenSize;


	// Visual effects
	ofxGaussianBlur		blur;

private:
	void updateKinectInput();
	void updateFromTimelineAndDraw();
};
