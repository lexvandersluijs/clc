#include "ofApp.h"

void KinectForProjection::setup(int index)
{
	_index = index;

	kinect.initSensor( _index );
	
	//kinect.initColorStream(640, 480);
	kinect.initDepthStream(320, 240, true);
	kinect.initSkeletonStream(true);

	kinect.start();

	if(kinect.getNuiSensor() != nullptr)
		kinect.getNuiSensor()->NuiCameraElevationSetAngle(18);

	hasSkeleton = false;

}

KinectForProjection::~KinectForProjection()
{
	kinect.stop();
}

void KinectForProjection::setupGUI()
{
	stringstream nameSS = stringstream();
	nameSS << "kinect " << _index << " vertical offset";
	appSettings::instance().gui.add(kinectVerticalOffset.setup( nameSS.str(), kinectOffsetFromProjector.y, -300, 300, 400 ));

	nameSS = stringstream();
	nameSS << "kinect " << _index << " horizontal offset";
	appSettings::instance().gui.add(kinectHorizontalOffset.setup( nameSS.str(), kinectOffsetFromProjector.x, -300, 300, 400 ));

	nameSS = stringstream();
	nameSS << "kinect " << _index << " forward offset";
	appSettings::instance().gui.add(kinectForwardOffset.setup( nameSS.str(), kinectOffsetFromProjector.z, 0, 400, 400 ));

	nameSS = stringstream();
	nameSS << "pres " << _index << " FL";
	appSettings::instance().gui.add(toPresentationSpaceFocalLengthSlider.setup( nameSS.str(), toPresentationSpaceFocalLength, 300, 1000, 400 ));

	nameSS = stringstream();
	nameSS << "pres " << _index << " PP X";
	appSettings::instance().gui.add(toPresentationSpacePrincipalX.setup( nameSS.str(), toPresentationSpacePrincipalPoint.x, -800, 2400, 400 ));

	nameSS = stringstream();
	nameSS << "pres " << _index << " PP Y";
	appSettings::instance().gui.add(toPresentationSpacePrincipalY.setup( nameSS.str(), toPresentationSpacePrincipalPoint.y, -600, 600, 400 ));

}

void KinectForProjection::updateFromGUI()
{
	kinectOffsetFromProjector.y = kinectVerticalOffset;
	kinectOffsetFromProjector.x = kinectHorizontalOffset;
	kinectOffsetFromProjector.z = kinectForwardOffset;
	toPresentationSpaceFocalLength = toPresentationSpaceFocalLengthSlider;
	toPresentationSpacePrincipalPoint.x = toPresentationSpacePrincipalX;
	toPresentationSpacePrincipalPoint.y = toPresentationSpacePrincipalY;

}

void KinectForProjection::update()
{
	kinect.update();

	hasSkeleton = false;

	if(kinect.isNewSkeleton()) 
	{

		//cout << "kinect.getSkeletons().size() = " << kinect.getSkeletons().size() << endl;

		for( int i = 0; i < kinect.getSkeletons().size(); i++) 
		{

			if( // kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD) != kinect.getSkeletons().at(i).end() && 
				kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT) != kinect.getSkeletons().at(i).end() &&
				kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT) != kinect.getSkeletons().at(i).end())
			{
				//cout << "head was found" << endl;


				// just get the first one
				//SkeletonBone headBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HEAD)->second;
				SkeletonBone leftHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
				SkeletonBone rightHandBone = kinect.getSkeletons().at(i).find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;

				// transform the 3D tracked values from Kinect to our 2D presentation/simulation space
				//presentationSpaceJoints[Head].setNewPosition(transformTrackedPointToProjectorScreen(headBone.getStartPosition()));
				presentationSpaceJoints[LeftHand].setNewPosition(transformTrackedPointToProjectorScreen(leftHandBone.getStartPosition()));
				presentationSpaceJoints[RightHand].setNewPosition(transformTrackedPointToProjectorScreen(rightHandBone.getStartPosition()));
/*
				static int counter = 0;
				// dump to console to check results
				if(counter%20 == 0)
				{
					cout << "Head 3D: " << headBone.getStartPosition() << ", 2D: " << presentationSpaceJoints[Head].getPosition() << endl;
					cout << "LeftHand 3D: " << leftHandBone.getStartPosition() << ", 2D: " << presentationSpaceJoints[LeftHand].getPosition() << endl;
					cout << "RightHand 3D: " << rightHandBone.getStartPosition() << ", 2D: " << presentationSpaceJoints[RightHand].getPosition() << endl;
				}
				counter++;
*/


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
	projected.x = -p.x / p.z;	// invert x: Kinect's convention is based on that it is looking away from the screen
	projected.y =  p.y / p.z;	// but in our case it is looking toward the screen
			
	projected.x *=  toPresentationSpaceFocalLength;
	projected.y *= -toPresentationSpaceFocalLength; // invert y: in screen-space +y is down
			
	ofVec2f projectedPoint = projected + toPresentationSpacePrincipalPoint;
	return projectedPoint;
}

ofVec2f KinectForProjection::transformTrackedPointToProjectorScreen(ofVec3f point)
{
	// we assume that the axes of the projector and the kinect (upright) scanning volume are aligned
	// multiply with 100 to convert from meters to centimeters
	ofVec3f pointInProjectorSpace = (point * 100.f)+ kinectOffsetFromProjector;

	// project the point to the projector to get normalized device coordinates
	return projectPoint(pointInProjectorSpace);
}