#pragma once

struct PresentationSpaceJoint
{
public:

	void setNewPosition(ofVec2f newPos)
	{
		velocity = newPos - position;
		position = newPos;
	}

	ofVec2f getPosition()
	{
		return position;
	}
	ofVec2f getVelocity()
	{
		return velocity;
	}

private:
	ofVec2f position;
	ofVec2f velocity;
};

enum JointIndex { Head=0, LeftHand=1, RightHand=2 };

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

	PresentationSpaceJoint presentationSpaceJoints[3];

};