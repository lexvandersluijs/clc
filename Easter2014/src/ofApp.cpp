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

	// -------------- initialize Kinect --------------
	kinect.initSensor( 0 );
	
	kinect.initColorStream(640, 480);
	kinect.initDepthStream(320, 240, true);
	kinect.initSkeletonStream(true);

	kinect.start();

	kinect.getNuiSensor()->NuiCameraElevationSetAngle(10);

	hasSkeleton = false;
	jointDistance = 1.f;
}

void testApp::updateKinectInput()
{
	kinect.update();

	if(kinect.isNewSkeleton()) 
	{

		//cout << "kinect.getSkeletons().size() = " << kinect.getSkeletons().size() << endl;

		for( int i = 0; i < kinect.getSkeletons().size(); i++) 
		{

			if(kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD) != kinect.getSkeletons().at(i).end())
			{
				//cout << "head was found" << endl;


				// just get the first one
				SkeletonBone headBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second;
				SkeletonBone leftHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
				SkeletonBone rightHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;

				ofVec3f hb( headBone.getScreenPosition().x, headBone.getScreenPosition().y, 0 );
				head = head.getInterpolated(hb, 0.5);
				head.z =  ofInterpolateCosine( head.z, headBone.getStartPosition().x, 0.5) + 0.1;

				leftHandPrev = leftHand;
				ofVec3f lhb(leftHandBone.getScreenPosition().x, leftHandBone.getScreenPosition().y, 0);
				leftHand = leftHand.getInterpolated( lhb, 0.5);
				leftHand.z = ofInterpolateCosine( leftHand.z, leftHandBone.getStartPosition().x, 0.5);

				rightHandPrev = rightHand;
				ofVec3f rhb(rightHandBone.getScreenPosition().x, rightHandBone.getScreenPosition().y, 0);
				rightHand = rightHand.getInterpolated( rhb, 0.5);
				rightHand.z = ofInterpolateCosine( rightHand.z, rightHandBone.getStartPosition().x, 0.5);

				cout << headBone.getScreenPosition()  << endl;
				cout << rightHandBone.getScreenPosition() << endl;
				cout << leftHandBone.getScreenPosition() << endl;

				//cout << kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second.getScreenPosition() << endl;
				//cout << kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second.getScreenPosition() << endl;
				//cout << kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second.getScreenPosition() << endl;

				jointDistance = head.distance(rightHand);
				jointDistance += leftHand.distance(rightHand);
				jointDistance += leftHand.distance(head);

				hasSkeleton = true;

				return;
			}
		}
	}
	//else
	//	cout << "no new skeleton"  << endl;
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

	if(hasSkeleton)
	{
		// ------- compute movement direction of left hand ---------------
		ofPoint leftHandDirection = (leftHand - leftHandPrev) * 10.0;

		// determine color
		ofPoint leftHandC = ofPoint(640*0.5, 480*0.5) - m;
		leftHandC.normalize();
		ofFloatColor leftHandColor = ofFloatColor(leftHandC.x,leftHandC.y,0.5)*sin(ofGetElapsedTimef());

		// scale input to window size (temporary)
		ofVec3f leftHandPos = leftHand;
		leftHandPos.x = (leftHand.x / 640.0f) * (float)ofGetWindowWidth();
		leftHandPos.y = (leftHand.y / 480.0f) * (float)ofGetWindowHeight();
		fluid.addTemporalForce(leftHandPos, leftHandDirection, leftHandColor,3.0f);

		// ------- compute movement direction of right hand ---------------
		ofPoint rightHandDirection = (rightHand - rightHandPrev) * 10.0;

		// determine color
		ofPoint rightHandC = ofPoint(640*0.5, 480*0.5) - m;
		rightHandC.normalize();
		ofFloatColor rightHandColor = ofFloatColor(rightHandC.x,rightHandC.y,0.5)*sin(ofGetElapsedTimef());

		// scale input to window size (temporary)
		ofVec3f rightHandPos = rightHand;
		rightHandPos.x = (rightHand.x / 640.0f) * (float)ofGetWindowWidth();
		rightHandPos.y = (rightHand.y / 480.0f) * (float)ofGetWindowHeight();
		//cout << "rightHandPos x,y = " << rightHandPos.x << ", " << rightHandPos.y << endl;
		fluid.addTemporalForce(rightHandPos, rightHandDirection, rightHandColor,3.0f);

	}

    //  Update
    //
    fluid.update();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_LINEAR);
    
    fluid.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
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