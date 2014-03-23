#include "ofApp.h"

// we need to declare this static variable in a CPP file (C# is better with this..)
appSettings* appSettings::inst;

//--------------------------------------------------------------

void testApp::removeWindowBorder()
{
	bool showWindowBorder = false;
	if (!showWindowBorder) {
	  HWND m_hWnd = WindowFromDC(wglGetCurrentDC());
	  long style = ::GetWindowLong(m_hWnd, GWL_STYLE);
	  style &= ~WS_DLGFRAME;
	  style &= ~WS_CAPTION;
	  style &= ~WS_BORDER;
	  style &= WS_POPUP;
	  //_originalWindowStyle = style;
	  ::SetWindowLong(m_hWnd, GWL_STYLE, style);

	  long exstyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	  exstyle &= ~WS_EX_DLGMODALFRAME;
	  //_originalWindowStyleEx = exstyle;
	  ::SetWindowLong(m_hWnd, GWL_EXSTYLE, exstyle);

	  SetWindowPos(m_hWnd, HWND_TOPMOST, 1920,0,0,0, SWP_NOSIZE);
	}
}
void testApp::setup()
{
	_prevTime = 0;

	presentationWidth = 1280; //2400;
	presentationHeight = 320; //600;

	//removeWindowBorder();

	_presentationOffset.x = 100.f;
	_presentationOffset.y = 100.f;

	showGUI = true;

	// ------------------------ init fluid simulation -----------------------
    ofEnableAlphaBlending();
    ofSetCircleResolution(100);

    //ofSetWindowShape(width, height);

	// -------------- initialize Kinect(s) --------------
	nrOfKinects = 1;
	kinectForProjection[0] = new KinectForProjection();
	kinectForProjection[0]->kinectOffsetFromProjector.x = 0;
	kinectForProjection[0]->kinectOffsetFromProjector.y = -300; // cm
	kinectForProjection[0]->kinectOffsetFromProjector.z = 300; // assuming z points forward
	kinectForProjection[0]->toPresentationSpacePrincipalPoint.x = (presentationWidth/3) / 2;
	kinectForProjection[0]->toPresentationSpacePrincipalPoint.y = presentationHeight / 2;
	kinectForProjection[0]->toPresentationSpaceFocalLength = 400; // pixels
	kinectForProjection[0]->setup(0);
	//kinectForProjection[1] = new KinectForProjection();
	//kinectForProjection[1].setup(1);

	// ---------------- initialize base settings object --------------
	appSettings::instance().setup();

	// set up parameters for Kinect 0
	for(int i=0; i<nrOfKinects; i++)
		kinectForProjection[i]->setupGUI();

	// also restore the previously saved settings
	appSettings::instance().loadFromFile();


	// note: we make the size of the simulation independent of the screen size, so that we can test
	// everything in the right proportions and at the right resolution, even before we have the 
	// triple-beamer, double-kinect setup..
	float screenToFluidScale = 0.4f;
	fluidEffect.setup(presentationWidth, presentationHeight, screenToFluidScale);

	particleEffect.setup(presentationWidth, presentationHeight, fluidEffect.getVelocityMap(), screenToFluidScale);


	// finally, start playing the timeline
	appSettings::instance().timeline.play();

	// ------------------ initialize visual effects -------
	//blur.passes = 2;
	//blur.allocate(fluidEffect.getWidth(), fluidEffect.getHeight()); // same size as fluid
}

void testApp::updateKinectInput()
{
	for(int i=0; i<nrOfKinects; i++)
	{
		// get values from GUI for Kinect
		kinectForProjection[i]->updateFromGUI();

		// get input from tracking and project to presentation (pixel) space
		kinectForProjection[i]->update();
	}
}

void testApp::updateFromSettings()
{
	appSettings::instance().update();

	//the value of changingRadius will be different depending on the timeline
	float changingRadius = appSettings::instance().timeline.getValue("MyCircleRadius");
	//use the value for something amazing!
	//ofCircle(mouseX, mouseY, changingRadius);

}

//--------------------------------------------------------------


void testApp::update()
{
	// first, some fundamentals
	float currentTime = ofGetElapsedTimef();
	float timeStep = currentTime - _prevTime;
	_prevTime = currentTime;

	// get inputs from input devices
    updateKinectInput();
	updateFromSettings();

    // Adding temporal Force
    //
    ofPoint m = ofPoint(mouseX,mouseY) - _presentationOffset;
    ofVec2f d = (m - oldM)*10.0;
    oldM = m;
    ofPoint c = ofPoint(640*0.5, 480*0.5) - m;
    c.normalize();

	float quotaPercentage = 0.5f;

	// if not speed-based then we always want to add a force, independent of movement. Otherwise
	// we require a minimum movement distance
	if(!appSettings::instance().speedBasedGeneration || d.length() > 3)
	{
		fluidEffect.getFluid().addTemporalForce(m, d, ofFloatColor(c.x,c.y,0.5)*sin(ofGetElapsedTimef()),3.0f);
		particleEffect.generate(currentTime, timeStep, m, d, quotaPercentage);
	}

	// get the inputs from all Kinects and insert the relevant forces into the fluid simulation
	for(int i=0; i<nrOfKinects; i++)
	{
		if(kinectForProjection[i]->hasSkeleton)
		{
			// ------- compute movement direction of left hand ---------------
		

			// determine color
			ofPoint leftHandC = ofPoint(640*0.5, 480*0.5) - m;
			leftHandC.normalize();
			ofFloatColor leftHandColor = ofFloatColor(1.0f, 0.f, 0.f); //leftHandC.x,leftHandC.y,0.5)*sin(ofGetElapsedTimef());

			// scale input to window size (temporary)
			ofVec2f leftHandPos = kinectForProjection[i]->presentationSpaceJoints[LeftHand].getPosition();
			ofVec2f leftHandDir = kinectForProjection[i]->presentationSpaceJoints[LeftHand].getVelocity();
			fluidEffect.getFluid().addTemporalForce(leftHandPos,
								   leftHandDir, 
								   leftHandColor, 3.0f);
			particleEffect.generate(currentTime, timeStep, leftHandPos, leftHandDir, quotaPercentage);
			// ------- compute movement direction of right hand ---------------
		

			// determine color
			ofPoint rightHandC = ofPoint(640*0.5, 480*0.5) - m;
			rightHandC.normalize();
			ofFloatColor rightHandColor = ofFloatColor(1.f, 1.f, 0.f); //rightHandC.x,rightHandC.y,0.5)*sin(ofGetElapsedTimef());

			// scale input to window size (temporary)
			ofVec2f rightHandPos = kinectForProjection[i]->presentationSpaceJoints[RightHand].getPosition();
			ofVec2f rightHandDir = kinectForProjection[i]->presentationSpaceJoints[RightHand].getVelocity();
			fluidEffect.getFluid().addTemporalForce(rightHandPos,
								   rightHandDir, 
								   rightHandColor, 3.0f);
			particleEffect.generate(currentTime, timeStep, rightHandPos, rightHandDir, quotaPercentage);
		}
	}

    //  Update
    //
    fluidEffect.update();
    
	particleEffect.update(currentTime, timeStep);

    ofSetWindowTitle(ofToString(ofGetFrameRate()));

	appSettings::instance().update();
}

//--------------------------------------------------------------
void testApp::draw()
{
	//ofBackgroundGradient(ofColor::darkGray, ofColor::black, OF_GRADIENT_LINEAR);
	ofBackground(ofColor::black);

	// ---------------- horizontal line ------------------
	ofSetLineWidth(1.0f);
	ofSetColor(255);
	ofLine(_presentationOffset.x, _presentationOffset.y + presentationHeight+1, 
			_presentationOffset.x + presentationWidth, _presentationOffset.y + presentationHeight+1);

	// ------ fluid sim ----------------
    fluidEffect.draw(_presentationOffset.x, _presentationOffset.y, presentationWidth, presentationHeight);

	// --------- particles -------------
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	particleEffect.draw(_presentationOffset.x, _presentationOffset.y, presentationWidth, presentationHeight);
	ofDisableBlendMode();

	ofSetLineWidth(1.0f);


	//fluid.getVelocityTexture().draw(0, 0);

	//blur << fluid.getVelocityTexture();
	//blur.update();

	// stretch blurred bitmap while rendering
	//blur.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());


	// ---------------- GUI -----------------
	appSettings::instance().draw(showGUI);

	if(showGUI)
	{
		float kinectDepthStartX = 500;
		float kinectDepthStartY = 400;

		// ------------- show circles and depth images to debug inputs from Kinect(s) -------------
		for(int i=0; i<nrOfKinects; i++)
		{
			ofSetColor(255, 255, 0);
			ofCircle(_presentationOffset + kinectForProjection[i]->presentationSpaceJoints[LeftHand].getPosition(), 10.f);
			ofSetColor(255, 0, 255);
			ofCircle(_presentationOffset + kinectForProjection[i]->presentationSpaceJoints[RightHand].getPosition(), 10.f);


			kinectForProjection[i]->kinect.getDepthTexture().draw(kinectDepthStartX + float(i)*320.0f, kinectDepthStartY);
		}

	}
}


void testApp::exit()
{
	delete kinectForProjection[0];
	//delete kinectForProjection[1];

	particleEffect.exit();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	fluidEffect.keyPressed(key);

	if(key == ' ')
	{
		showGUI = !showGUI;
	}
}

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