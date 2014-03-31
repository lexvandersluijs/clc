#include "ofApp.h"

// we need to declare this static variable in a CPP file (C# is better with this..)
appSettings* appSettings::inst;

//--------------------------------------------------------------

HWND GetWindowHandle()
{
	return WindowFromDC(wglGetCurrentDC());
}

void testApp::configureWindowBorder(bool showWindowBorder)
{
	if (showWindowBorder) 
	{
		HWND hWnd = GetWindowHandle();
		long style = ::GetWindowLong(hWnd, GWL_STYLE);
		style |= WS_DLGFRAME;
		style |= WS_CAPTION;
		style |= WS_BORDER;
		style |= WS_THICKFRAME;
//		style &= ~WS_POPUP;
		::SetWindowLong(hWnd, GWL_STYLE, style);

		long exstyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
		exstyle |= WS_EX_DLGMODALFRAME;
		exstyle |= WS_EX_CLIENTEDGE;
		exstyle |= WS_EX_STATICEDGE;
		::SetWindowLong(hWnd, GWL_EXSTYLE, exstyle);

		// call SetWindowPos to _apply_ the frame change, see http://msdn.microsoft.com/en-us/library/windows/desktop/ms633545(v=vs.85).aspx
		::SetWindowPos(hWnd, HWND_TOPMOST, 0,0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		HWND hWnd = GetWindowHandle();
		long style = ::GetWindowLong(hWnd, GWL_STYLE);
		style &= ~WS_DLGFRAME;
		style &= ~WS_CAPTION;
		style &= ~WS_BORDER;
		style &= ~WS_THICKFRAME;
//		style &= WS_POPUP;
		::SetWindowLong(hWnd, GWL_STYLE, style);

		long exstyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
		exstyle &= ~WS_EX_DLGMODALFRAME;
		exstyle &= ~WS_EX_CLIENTEDGE ;
		exstyle &= ~WS_EX_STATICEDGE ;
		::SetWindowLong(hWnd, GWL_EXSTYLE, exstyle);

		// call SetWindowPos to _apply_ the frame change, see http://msdn.microsoft.com/en-us/library/windows/desktop/ms633545(v=vs.85).aspx
		::SetWindowPos(hWnd, HWND_TOPMOST, 0,0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
	}
}
void testApp::setup()
{
	_prevTime = 0;

	presentationWidth = 1280; //2400;
	presentationHeight = 320; //600;
	float screenToFluidScale = 0.4f;

	bool testWithTwnklsBeamer = true;
	if(testWithTwnklsBeamer)
	{
		presentationWidth = 1024; 
		presentationHeight = 768; 
		screenToFluidScale = 0.25f;
	}

	_presentationOffset = ofVec2f::zero();

	_showGUI = true;
	_productionMode = false; // start with presentation on main monitor
	_visible = true;
	_colorMode = 2;

    ofEnableAlphaBlending();
    ofSetCircleResolution(100);

    //ofSetWindowShape(width, height);

	// -------------- initialize Kinect(s) --------------
	nrOfKinects = 2;
	kinectForProjection[0] = new KinectForProjection();
	kinectForProjection[0]->kinectOffsetFromProjector.x = 0;
	kinectForProjection[0]->kinectOffsetFromProjector.y = -300; // cm
	kinectForProjection[0]->kinectOffsetFromProjector.z = 300; // assuming z points forward
	kinectForProjection[0]->toPresentationSpacePrincipalPoint.x = (presentationWidth/3) / 2;
	kinectForProjection[0]->toPresentationSpacePrincipalPoint.y = presentationHeight / 2;
	kinectForProjection[0]->toPresentationSpaceFocalLength = 400; // pixels
	kinectForProjection[0]->setup(0);

	kinectForProjection[1] = new KinectForProjection();
	kinectForProjection[1]->kinectOffsetFromProjector.x = 0;
	kinectForProjection[1]->kinectOffsetFromProjector.y = -300; // cm
	kinectForProjection[1]->kinectOffsetFromProjector.z = 300; // assuming z points forward
	kinectForProjection[1]->toPresentationSpacePrincipalPoint.x = (presentationWidth/3) / 2;
	kinectForProjection[1]->toPresentationSpacePrincipalPoint.y = presentationHeight / 2;
	kinectForProjection[1]->toPresentationSpaceFocalLength = 400; // pixels
	kinectForProjection[1]->setup(1);

	// ---------------- initialize base settings object --------------
	appSettings::instance().setup();

	// set up parameters for Kinect 0
	for(int i=0; i<nrOfKinects; i++)
		kinectForProjection[i]->setupGUI();

	// also restore the previously saved settings
	appSettings::instance().loadFromFile();

	// ------------------------ init fluid simulation and particles-----------------------
	// note: we make the size of the simulation independent of the screen size, so that we can test
	// everything in the right proportions and at the right resolution, even before we have the 
	// triple-beamer, double-kinect setup..
	fluidEffect.setup(presentationWidth, presentationHeight, screenToFluidScale);
	particleEffect.setup(presentationWidth, presentationHeight, fluidEffect.getVelocityMap(), screenToFluidScale);


	// finally, start playing the timeline
	//appSettings::instance().timeline.play();

	// --------------------- for OSC --------------------------
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	current_msg_string = 0;
}

void testApp::updateFromOSC()
{
	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msg_strings[i] = "";
		}
	}

	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);

		// check for mouse moved message
		if(m.getAddress() == "/mouse/position"){
			// both the arguments are int32's
			oscMouseX = m.getArgAsInt32(0);
			oscMouseY = m.getArgAsInt32(1);
		}
		// check for mouse button message
		else if(m.getAddress() == "/mouse/button"){
			// the single argument is a string
			oscMouseButtonState = m.getArgAsString(0);
		}
		else if(m.getAddress() == "/start"){
			// the single argument is a string
			appSettings::instance().timeline.setCurrentTimeSeconds(0.f);
			appSettings::instance().timeline.play();
		}
		else if(m.getAddress() == "/stop"){
			appSettings::instance().timeline.stop();
		}
		else if(m.getAddress() == "/visible"){
			_visible = true;
		}
		else if(m.getAddress() == "/invisible"){
			_visible = false;
		}
		else if(m.getAddress() == "/color1"){
			_colorMode = 1; // Red / yellow
		}
		else if(m.getAddress() == "/color2"){
			_colorMode = 2; // happy colors
		}

		else{
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for(int i = 0; i < m.getNumArgs(); i++){
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
				}
				else{
					msg_string += "unknown";
				}
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}
	}
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

void testApp::changeProductionModeSetting(bool productionMode)
{
	if(productionMode != _productionMode)
	{
		HWND hWnd = GetWindowHandle();
		int primaryScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int primaryScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		if(productionMode)
		{
			// switch to production mode:
			// remove window border, change offset for presentation, set window position
			configureWindowBorder(false);

			// move the presentation to exactly to the right of the main window so that it lies in the extended desktop area
			_presentationOffset = ofVec2f(primaryScreenWidth, 0.f);

			int windowWidth = primaryScreenWidth + presentationWidth;
			int windowHeight = 1000;

			//::SetWindowPos(hWnd, HWND_TOPMOST, 0,0, windowWidth, windowHeight, 0);
		}
		else
		{
			// switch to test mode:
			// put back window border, change offset for presentation back to 0,0, set window position
			// in develop/test-mode, the presentation is underneath the timeline and GUI
			configureWindowBorder(true);

			_presentationOffset = ofVec2f::zero();

			int windowWidth = primaryScreenWidth;
			int windowHeight = 1000;

			//::SetWindowPos(hWnd, HWND_TOPMOST, 0,0, windowWidth, windowHeight, 0);
		}
		_productionMode = productionMode;
	}
}

bool testApp::pointInsidePresentationArea(ofVec2f p)
{
	if((p.x > 10.f) && (p.y > 10.f) && (p.x < presentationWidth-10.f) && (p.y < presentationHeight-10.f))
		return true;

	return false;
}

// limb index:
// 0: left hand
// 1: right hand
// 2: left foot (?)
// 3: right foot (?)

ofFloatColor testApp::generateColor(ofVec2f position, ofVec2f dir, float currentTime, int limbIndex)
{
	ofFloatColor c;
	switch(_colorMode)
	{

	// original mode from ofxFluid
	case 0:
		{
		ofPoint cn = ofPoint(640*0.5, 480*0.5) - position;
		cn.normalize();
		c = ofFloatColor(cn.x,cn.y,0.5) * sin(currentTime);
		}
		break;

	// constant color
	case 1:
		if(limbIndex%2 == 0)
			c = ofFloatColor(1.0f, 0.f, 0.f);
		else
			c = ofFloatColor(1.0f, 1.f, 0.f);
		break;

	case 2:
		if(limbIndex%2 == 0)
			c = ofFloatColor::fromHsb(fmod(currentTime / 3.f, 1.f), 1.f, 0.7f);
		else
			c = ofFloatColor::fromHsb(fmod((currentTime / 3.f) + 1.5f, 1.f), 1.f, 0.7f);
		break;

	// based on velocity (pixels per second)
	case 3: 
		{
			float speed = dir.length();
			cout << "speed: " << speed << endl;

			c = ofFloatColor::red;
			c = c.getLerped(ofFloatColor::yellow, ofClamp(speed / 250.f, 0, 1.f));
		}
		break;
	}


	//cout << "color: " << color << "c.x: " << c.x << "c.y: " << c.y << endl;
	return c;
}

void testApp::update()
{
	updateFromOSC();

	// first, some fundamentals
	float currentTime = ofGetElapsedTimef();
	float timeStep = currentTime - _prevTime;
	_prevTime = currentTime;

	// get inputs from input devices
    updateKinectInput();
	updateFromSettings();

	float quotaPercentage = 0.5f;

    // Adding temporal Force
    //
    ofPoint m = ofPoint(mouseX,mouseY) - _presentationOffset;

	// only apply forces to fluids, generate particles if the mouse cursor is inside the presentation area
	if(pointInsidePresentationArea(m))
	{
		ofVec2f d = (m - oldM)*10.0;
		oldM = m;


		// if not speed-based then we always want to add a force, independent of movement. Otherwise
		// we require a minimum movement distance
		if(!appSettings::instance().speedBasedGeneration || d.length() > 3)
		{
			ofFloatColor color = generateColor(m, d, currentTime, 0); 
			fluidEffect.getFluid().addTemporalForce(m, d, color ,3.0f);
			particleEffect.generate(currentTime, timeStep, m, d, quotaPercentage);
		}
	}

	// get the inputs from all Kinects and insert the relevant forces into the fluid simulation
	for(int i=0; i<nrOfKinects; i++)
	{
		if(kinectForProjection[i]->hasSkeleton)
		{
			// ------- compute movement direction of left hand ---------------
		

			// determine color
			//ofPoint leftHandC = ofPoint(640*0.5, 480*0.5) - m;
			//leftHandC.normalize();
		

			// scale input to window size (temporary)
			ofVec2f leftHandPos = kinectForProjection[i]->presentationSpaceJoints[LeftHand].getPosition();
			ofVec2f leftHandDir = kinectForProjection[i]->presentationSpaceJoints[LeftHand].getVelocity();

			ofFloatColor leftHandColor = generateColor(leftHandPos, leftHandDir, currentTime, 0); //ofFloatColor(1.0f, 0.f, 0.f); //leftHandC.x,leftHandC.y,0.5)*sin(ofGetElapsedTimef());

			if(pointInsidePresentationArea(leftHandPos))
			{
				fluidEffect.getFluid().addTemporalForce(leftHandPos,
										leftHandDir, 
										leftHandColor, 3.0f);
				particleEffect.generate(currentTime, timeStep, leftHandPos, leftHandDir, quotaPercentage);
			}
			// ------- compute movement direction of right hand ---------------
		

			// determine color
			//ofPoint rightHandC = ofPoint(640*0.5, 480*0.5) - m;
			//rightHandC.normalize();
			//ofFloatColor rightHandColor = ofFloatColor(1.f, 1.f, 0.f); //rightHandC.x,rightHandC.y,0.5)*sin(ofGetElapsedTimef());

			// scale input to window size (temporary)
			ofVec2f rightHandPos = kinectForProjection[i]->presentationSpaceJoints[RightHand].getPosition();
			ofVec2f rightHandDir = kinectForProjection[i]->presentationSpaceJoints[RightHand].getVelocity();
			
			ofFloatColor rightHandColor = generateColor(rightHandPos, rightHandDir, currentTime, 1); 

			if(pointInsidePresentationArea(rightHandPos))
			{
				fluidEffect.getFluid().addTemporalForce(rightHandPos,
										rightHandDir, 
										rightHandColor, 3.0f);
				particleEffect.generate(currentTime, timeStep, rightHandPos, rightHandDir, quotaPercentage);
			}
		}
	}
	
    //  Update
    //
    fluidEffect.update();
    
	particleEffect.update(currentTime, timeStep);

    ofSetWindowTitle(ofToString(ofGetFrameRate()));

	appSettings::instance().update();
}

void testApp::drawOSC()
{
	string buf;
	buf = "listening for osc messages on port" + ofToString(PORT);
	ofDrawBitmapString(buf, 10, 20);

	// draw mouse state
	buf = "mouse: " + ofToString(oscMouseX, 4) +  " " + ofToString(oscMouseY, 4);
	ofDrawBitmapString(buf, 430, 20);
	ofDrawBitmapString(oscMouseButtonState, 580, 20);

	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		ofDrawBitmapString(msg_strings[i], 10, 40 + 15 * i);
	}	
}

//--------------------------------------------------------------
void testApp::draw()
{
	//ofBackgroundGradient(ofColor::darkGray, ofColor::black, OF_GRADIENT_LINEAR);
	ofBackground(ofColor::black);

	// ---------------- horizontal line below presentation area for testing ------------------
	if(!_productionMode)
	{
		ofSetLineWidth(1.0f);
		ofSetColor(255);
		ofLine(_presentationOffset.x, _presentationOffset.y + presentationHeight+1, 
				_presentationOffset.x + presentationWidth, _presentationOffset.y + presentationHeight+1);
	}

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
	appSettings::instance().draw(_showGUI);

	if(_showGUI)
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

		// draw debug output from OSC
		drawOSC();

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

	switch(key)
	{
		case OF_KEY_TAB:
			_showGUI = !_showGUI;
			break;
		case 'v':
			_visible = !_visible;
			break;
		case 'p':
			changeProductionModeSetting(!_productionMode);
			break;
		case '0':
			_colorMode = 0;
			break;
		case '1':
			_colorMode = 1;
			break;
		case '2':
			_colorMode = 2;
			break;
		case '3':
			_colorMode = 3;
			break;
		case '4':
			_colorMode = 4;
			break;
		case '5':
			_colorMode = 5;
			break;
		case '6':
			_colorMode = 6;
			break;
		case '7':
			_colorMode = 7;
			break;
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