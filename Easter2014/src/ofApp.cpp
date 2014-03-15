#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	// ------------------------ init fluid simulation -----------------------
    ofEnableAlphaBlending();
    ofSetCircleResolution(100);

	// note: we make the size of the simulation independent of the screen size, so that we can test
	// everything in the right proportions and at the right resolution, even before we have the 
	// triple-beamer, double-kinect setup..
	fluidEffect.setup(2400, 600);

	particleEffect.setup(2400, 600);

    //ofSetWindowShape(width, height);

	// -------------- initialize Kinect(s) --------------
	nrOfKinects = 1;
	kinectForProjection[0] = new KinectForProjection();
	kinectForProjection[0]->kinectOffsetFromProjector.x = 0;
	kinectForProjection[0]->kinectOffsetFromProjector.y = -300; // cm
	kinectForProjection[0]->kinectOffsetFromProjector.z = 300; // assuming z points forward
	kinectForProjection[0]->toPresentationSpacePrincipalPoint.x = 800 / 2;
	kinectForProjection[0]->toPresentationSpacePrincipalPoint.y = 600 / 2;
	kinectForProjection[0]->toPresentationSpaceFocalLength = 400; // pixels
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
	gui.add(kinectVerticalOffset0.setup( "kinect 0 vertical offset", kinectForProjection[0]->kinectOffsetFromProjector.y, -300, 300, 400 ));
	gui.add(kinectForwardOffset0.setup( "kinect 0 forward offset", kinectForProjection[0]->kinectOffsetFromProjector.z, 100, 400, 400 ));
	gui.add(toPresentationSpaceFocalLength0.setup( "pres 0 FL", kinectForProjection[0]->toPresentationSpaceFocalLength, 300, 1000, 400 ));
	gui.add(toPresentationSpacePrincipalX0.setup( "pres 0 PP X", kinectForProjection[0]->toPresentationSpacePrincipalPoint.x, -800, 2400, 400 ));
	gui.add(toPresentationSpacePrincipalY0.setup( "pres 0 PP Y", kinectForProjection[0]->toPresentationSpacePrincipalPoint.y, -600, 600, 400 ));

	// TODO: same for Kinect 1


	// ------------------ initialize visual effects -------
	blur.passes = 2;
	blur.allocate(fluidEffect.getWidth(), fluidEffect.getHeight()); // same size as fluid
}

void testApp::updateKinectInput()
{
	for(int i=0; i<nrOfKinects; i++)
	{
		// get values from GUI for Kinect
		kinectForProjection[i]->kinectOffsetFromProjector.y = kinectVerticalOffset0;
		kinectForProjection[i]->kinectOffsetFromProjector.z = kinectForwardOffset0;
		kinectForProjection[i]->toPresentationSpaceFocalLength = toPresentationSpaceFocalLength0;
		kinectForProjection[i]->toPresentationSpacePrincipalPoint.x = toPresentationSpacePrincipalX0;
		kinectForProjection[i]->toPresentationSpacePrincipalPoint.y = toPresentationSpacePrincipalY0;

		// TODO: same for Kinect 1

		kinectForProjection[i]->update();
		//kinectForProjection[1].update();
	}
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
	fluidEffect.getFluid().addTemporalForce(m, d, ofFloatColor(c.x,c.y,0.5)*sin(ofGetElapsedTimef()),3.0f);

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
			//ofVec3f leftHand = kinectForProjection[0]->leftHand;
			//ofVec3f leftHandPos = leftHand;
			//leftHandPos.x = leftHand.x * 512 + 512;
			//leftHandPos.y = -leftHand.y * 384 + 512;
			fluidEffect.getFluid().addTemporalForce(kinectForProjection[i]->presentationSpaceJoints[LeftHand].getPosition(),
								   kinectForProjection[i]->presentationSpaceJoints[LeftHand].getVelocity(), 
								   leftHandColor, 3.0f);

			// ------- compute movement direction of right hand ---------------
		

			// determine color
			ofPoint rightHandC = ofPoint(640*0.5, 480*0.5) - m;
			rightHandC.normalize();
			ofFloatColor rightHandColor = ofFloatColor(1.f, 1.f, 0.f); //rightHandC.x,rightHandC.y,0.5)*sin(ofGetElapsedTimef());

			// scale input to window size (temporary)
			//ofVec3f rightHand = kinectForProjection[0]->rightHand;
			//ofVec3f rightHandPos = rightHand;
			//rightHandPos.x = rightHand.x * 512 + 512;
			//rightHandPos.y = -rightHand.y * 384 + 512;
			//cout << "rightHandPos x,y = " << rightHandPos.x << ", " << rightHandPos.y << endl;
			//fluid.addTemporalForce(rightHandPos, kinectForProjection[0]->rightHandDirection, rightHandColor,3.0f);
			fluidEffect.getFluid().addTemporalForce(kinectForProjection[i]->presentationSpaceJoints[RightHand].getPosition(),
								   kinectForProjection[i]->presentationSpaceJoints[RightHand].getVelocity(), 
								   rightHandColor, 3.0f);
		}
	}

    //  Update
    //
    fluidEffect.update();
    
	particleEffect.update();

    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_LINEAR);
    
	// ------ fluid sim ----------------
    fluidEffect.draw(0, 0, 2400, 600);

	// --------- particles -------------
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	particleEffect.draw(0, 0, 2400, 600);
	ofDisableBlendMode();

	// ------------ Timeline -----------
	updateFromTimelineAndDraw();

	kinectForProjection[0]->kinect.getDepthTexture().draw(0, 0);
	//fluid.getVelocityTexture().draw(0, 0);

	//blur << fluid.getVelocityTexture();
	//blur.update();

	// stretch blurred bitmap while rendering
	//blur.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	// ------------- inputs from Kinect(s) -------------
	for(int i=0; i<nrOfKinects; i++)
	{
		ofSetColor(255, 255, 0);
		ofCircle(kinectForProjection[i]->presentationSpaceJoints[LeftHand].getPosition(), 10.f);
		ofSetColor(255, 0, 255);
		ofCircle(kinectForProjection[i]->presentationSpaceJoints[RightHand].getPosition(), 10.f);
	}

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
	fluidEffect.keyPressed(key);
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