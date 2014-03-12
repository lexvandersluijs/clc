#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	// ------------------------ init fluid simulation -----------------------
    ofEnableAlphaBlending();
    ofSetCircleResolution(100);
    
	width = ofGetWindowWidth(); //300;
    height = ofGetWindowHeight(); // 600;

    // Initial Allocation
    //
    fluid.allocate(width, height, 0.2);
    
    // Seting the gravity set up & injecting the background image
    //
    fluid.dissipation = 0.99;
    fluid.velocityDissipation = 0.99;
    
    fluid.setGravity(ofVec2f(0.0,0.0));
//    fluid.setGravity(ofVec2f(0.0,0.0098));
    
    //  Set obstacle
    //
    fluid.begin();
    ofSetColor(0,0);
    ofSetColor(255);
    ofCircle(width*0.5, height*0.35, 40);
    fluid.end();
    fluid.setUseObstacles(false);
    
    // Adding constant forces
    //
    //fluid.addConstantForce(ofPoint(width*0.5,height*0.85), ofPoint(0,-2), ofFloatColor(0.5,0.1,0.0), 10.f);

    //ofSetWindowShape(width, height);

	// -------------- initialize Kinect(s) --------------
	kinectForProjection[0] = new KinectForProjection();
	kinectForProjection[0]->kinectOffsetFromProjector.x = 0;
	kinectForProjection[0]->kinectOffsetFromProjector.y = -300; // cm
	kinectForProjection[0]->kinectOffsetFromProjector.z = 300; // assuming z points forward
	kinectForProjection[0]->toPresentationSpacePrincipalPoint.x = 1024 / 2;
	kinectForProjection[0]->toPresentationSpacePrincipalPoint.y = 0;
	kinectForProjection[0]->toPresentationSpaceFocalLength = 256; // pixels
	kinectForProjection[0]->setup(0);
	//kinectForProjection[1] = new KinectForProjection();
	//kinectForProjection[1].setup(1);

	// ---------------- initialize timeline --------------
	timeline.setup(); //registers events
	timeline.setDurationInSeconds(10); //sets time
	timeline.setLoopType(OF_LOOP_NORMAL); //turns the timeline to loop

	//add a tracks, etc
	timeline.addCurves("MyCircleRadius", ofRange(0, 200));
	timeline.play();

	// ------------------ intialize GUI -------------------
	gui.setup(); // most of the time you don't need a name
	gui.setDefaultWidth(400);

	// set up parameters for Kinect 0
	gui.add(kinectVerticalOffset.setup( "kinect 0 vertical offset", kinectForProjection[0]->kinectOffsetFromProjector.y, -300, 300, 400 ));
	gui.add(kinectForwardOffset.setup( "kinect 0 forward offset", kinectForProjection[0]->kinectOffsetFromProjector.z, 100, 400, 400 ));
	gui.add(toPresentationSpaceFocalLength.setup( "pres 0 FL", kinectForProjection[0]->toPresentationSpaceFocalLength, 100, 500, 400 ));
	gui.add(toPresentationSpacePrincipalX.setup( "pres 0 PP X", kinectForProjection[0]->toPresentationSpacePrincipalPoint.x, -512, 3072, 400 ));
	gui.add(toPresentationSpacePrincipalY.setup( "pres 0 PP Y", kinectForProjection[0]->toPresentationSpacePrincipalPoint.y, -384, 384, 400 ));

	// TODO: same for Kinect 1


	// ------------------ initialize visual effects -------
	blur.passes = 2;
	blur.allocate(width/2, height/2);
}

void testApp::updateKinectInput()
{
	// get values from GUI for Kinect 0
	kinectForProjection[0]->kinectOffsetFromProjector.y = kinectVerticalOffset;
	kinectForProjection[0]->kinectOffsetFromProjector.z = kinectForwardOffset;
	kinectForProjection[0]->toPresentationSpaceFocalLength = toPresentationSpaceFocalLength;
	kinectForProjection[0]->toPresentationSpacePrincipalPoint.x = toPresentationSpacePrincipalX;
	kinectForProjection[0]->toPresentationSpacePrincipalPoint.y = toPresentationSpacePrincipalY;

	// TODO: same for Kinect 1

	kinectForProjection[0]->update();
	//kinectForProjection[1].update();

}

void testApp::updateFromTimelineAndDraw()
{
	//the value of changingRadius will be different depending on the timeline
	float changingRadius = timeline.getValue("MyCircleRadius");
	//use the value for something amazing!
	ofCircle(mouseX, mouseY, changingRadius);
	//don't forget to draw your timeline so you can edit it.
	timeline.draw();
}

//--------------------------------------------------------------
void testApp::update()
{
    updateKinectInput();

    // Adding temporal Force
    //
    ofPoint m = ofPoint(mouseX,mouseY);
    ofPoint d = (m - oldM)*10.0;
    oldM = m;
    ofPoint c = ofPoint(640*0.5, 480*0.5) - m;
    c.normalize();
    //fluid.addTemporalForce(m, d, ofFloatColor(c.x,c.y,0.5)*sin(ofGetElapsedTimef()),3.0f);

	if(kinectForProjection[0]->hasSkeleton)
	{
		// ------- compute movement direction of left hand ---------------
		

		// determine color
		ofPoint leftHandC = ofPoint(640*0.5, 480*0.5) - m;
		leftHandC.normalize();
		ofFloatColor leftHandColor = ofFloatColor(leftHandC.x,leftHandC.y,0.5)*sin(ofGetElapsedTimef());

		// scale input to window size (temporary)
		ofVec3f leftHand = kinectForProjection[0]->leftHand;
		ofVec3f leftHandPos = leftHand;
		leftHandPos.x = (leftHand.x / 640.0f) * (float)ofGetWindowWidth();
		leftHandPos.y = (leftHand.y / 480.0f) * (float)ofGetWindowHeight();
		fluid.addTemporalForce(leftHandPos, kinectForProjection[0]->leftHandDirection, leftHandColor,3.0f);

		// ------- compute movement direction of right hand ---------------
		

		// determine color
		ofPoint rightHandC = ofPoint(640*0.5, 480*0.5) - m;
		rightHandC.normalize();
		ofFloatColor rightHandColor = ofFloatColor(rightHandC.x,rightHandC.y,0.5)*sin(ofGetElapsedTimef());

		// scale input to window size (temporary)
		ofVec3f rightHand = kinectForProjection[0]->rightHand;
		ofVec3f rightHandPos = rightHand;
		rightHandPos.x = (rightHand.x / 640.0f) * (float)ofGetWindowWidth();
		rightHandPos.y = (rightHand.y / 480.0f) * (float)ofGetWindowHeight();
		//cout << "rightHandPos x,y = " << rightHandPos.x << ", " << rightHandPos.y << endl;
		fluid.addTemporalForce(rightHandPos, kinectForProjection[0]->rightHandDirection, rightHandColor,3.0f);

	}

    //  Update
    //
    fluid.update();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_LINEAR);
    
	// ------ fluid sim ----------------
    fluid.draw();

	// ------------ Timeline -----------
	//updateFromTimelineAndDraw();

	//kinect.getDepthTexture().draw(0, 0);
	//fluid.getVelocityTexture().draw(0, 0);

	//blur << fluid.getVelocityTexture();
	//blur.update();

	// stretch blurred bitmap while rendering
	//blur.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	// ---------------- GUI -----------------
	gui.draw();
}

void testApp::exit()
{
	delete kinectForProjection[0];
	//delete kinectForProjection[1];
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    if( key == 'p')
        bPaint = !bPaint;
    if( key == 'o')
        bObstacle = !bObstacle;
    if( key == 'b')
        bBounding = !bBounding;
    if( key == 'c')
        bClear = !bClear;
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