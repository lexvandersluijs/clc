#pragma once

#include <cmath>

#include "ofMain.h"
#include "ofxFluid.h"
#include "ofxGaussianBlur.h"
#include "ofxKinectCommonBridge.h"
#include "ofxTimeline.h"
#include "ofxGui.h"
#include "ofxGlow.h"
#include "ofxFastFboReader.h" // for getting the velocity vector field from the GPU

#include "appSettings.h"

#include "ParticleSystem/Particle.h"
#include "ParticleSystem/ParticleManager.h"
#include "ParticleSystem/ParticleGenerator.h"
#include "ParticleSystem/ParticleRenderer.h"
#include "ParticleSystem/ParticleAnimator.h"
#include "ParticleSystem/ForceField.h"
#include "ParticleSystem/SimulationAnimator.h"
#include "KinectForProjection.h"
#include "FluidEffect.h"
#include "ParticleSystem/FluidSimulationAnimator.h"
#include "ParticleEffect.h"


class testApp : public ofBaseApp{
public:
	testApp()
	{
	}

	void removeWindowBorder();
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

	appSettings settings;


	// Visual effects
	//ofxGaussianBlur		blur;


private:
	float _prevTime;

	ofVec2f _presentationOffset; // 0,0 in test mode, screenWidth,0 in production mode
	bool _productionMode;

	int presentationWidth;
	int presentationHeight;

	bool showGUI;

	void updateKinectInput();
	void updateFromSettings();
};
