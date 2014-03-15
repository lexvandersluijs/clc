#pragma once

struct PresentationSpaceJoint
{
public:

	void setNewPosition(ofVec2f newPos)
	{
		float currentTime = ofGetElapsedTimef();
		float deltaTime = currentTime - prevTime;
		if(deltaTime > 1.0f) // if more than 1 second has elapsed between frames, we cannot reliably compute the velocity. Better luck next frame.
		{
			velocity = ofVec2f::zero();
		}
		else
		{
			velocity = newPos - position;
			float movementAmount = velocity.length();
			float frameRateIndependentSpeed = movementAmount / deltaTime; // if we moved 5 pixels in 0.1 seconds, then our velocity is 50 pixels/second
			velocity.normalize();
			velocity *= frameRateIndependentSpeed;
		}
		prevTime = currentTime;

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
	PresentationSpaceJoint()
	{
		prevTime = 0;
	}

private:
	ofVec2f position;
	ofVec2f velocity;
	float prevTime;
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