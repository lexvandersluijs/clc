#pragma once


class KinectForProjection
{
public:
	ofxKinectCommonBridge kinect;
	ofVec3f head;
	ofVec3f leftHand, leftHandPrev, leftHandDirection;
	ofVec3f rightHand, rightHandPrev, rightHandDirection;

	bool hasSkeleton;
	float jointDistance;


	void setup(int index);
	void update();

	ofVec2f projectPoint(ofVec3f p);
	ofVec2f transformTrackedPointToProjectorScreen(ofVec3f point);

	ofVec3f kinectOffsetFromProjector;

	ofVec2f toPresentationSpacePrincipalPoint;
	float toPresentationSpaceFocalLength;
};