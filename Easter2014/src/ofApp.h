#pragma once

#include "ofMain.h"
#include "ofxFluid.h"
#include "ofxGaussianBlur.h"
#include "ofxKinectCommonBridge.h"
#include "ofxTimeline.h"
#include "ofxGui.h"

#include "KinectForProjection.h"

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
		
    ofxFluid fluid;
    
    ofVec2f oldM;
    int     width,height;
    bool    bPaint, bObstacle, bBounding, bClear;

	// Kinect
	KinectForProjection* kinectForProjection[2];

	// Control
	ofxTimeline timeline;

	ofxPanel gui;
	ofxFloatSlider kinectVerticalOffset;
	ofxFloatSlider kinectForwardOffset;
	ofxFloatSlider toPresentationSpaceFocalLength;
	ofxFloatSlider toPresentationSpacePrincipalX;
	ofxFloatSlider toPresentationSpacePrincipalY;
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
