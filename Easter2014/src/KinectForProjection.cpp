#include "ofApp.h"

void KinectForProjection::setup(int index)
{
	kinect.initSensor( index );
	
	kinect.initColorStream(640, 480);
	kinect.initDepthStream(320, 240, true);
	kinect.initSkeletonStream(true);

	kinect.start();

	if(kinect.getNuiSensor() != nullptr)
		kinect.getNuiSensor()->NuiCameraElevationSetAngle(10);

	hasSkeleton = false;
	jointDistance = 1.f;
}

void KinectForProjection::update()
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

				leftHandDirection = (leftHand - leftHandPrev) * 10.0;

				rightHandPrev = rightHand;
				ofVec3f rhb(rightHandBone.getScreenPosition().x, rightHandBone.getScreenPosition().y, 0);
				rightHand = rightHand.getInterpolated( rhb, 0.5);
				rightHand.z = ofInterpolateCosine( rightHand.z, rightHandBone.getStartPosition().x, 0.5);

				rightHandDirection = (rightHand - rightHandPrev) * 10.0;

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

ofVec2f KinectForProjection::projectPoint(ofVec3f p)
{
	ofVec2f projected;
	projected.x = p.x / p.z;
	projected.y = p.y / p.z;
			
	projected.x *= toPresentationSpaceFocalLength;
	projected.y *= toPresentationSpaceFocalLength; // invert y: in screen-space +y is down
			
	ofVec2f projectedPoint = p + toPresentationSpacePrincipalPoint;
	return projectedPoint;
}

ofVec2f KinectForProjection::transformTrackedPointToProjectorScreen(ofVec3f point)
{
	// we assume that the axes of the projector and the kinect (upright) scanning volume are aligned
	ofVec3f pointInProjectorSpace = point + kinectOffsetFromProjector;

	// project the point to the projector to get normalized device coordinates
	return projectPoint(pointInProjectorSpace);
}