class FluidEffect
{
public:
	void setup(int w, int h, float screenToFluidScale);

	// event handler for catching events from 'Flags' timeline
	void receivedBang(ofxTLBangEventArgs& bang)
	{
	  ofLogNotice("Bang fired from track " + bang.track->getName());
	  if(bang.track->getName() == "Events")
	  {
		  if(bang.flag == "F_A")
		  {
			  setConstantForcesPattern(0);
		  }
		  if(bang.flag == "F_B")
		  {
			  setConstantForcesPattern(1);
		  }
	  }
	}

	void setConstantForcesPattern(int patternIndex)
	{
		fluid.clearConstantForces();
		switch(patternIndex)
		{
		case 0:
			fluid.addConstantForce(ofPoint(width*0.25,height*0.9), ofPoint(0,-2), ofFloatColor(0.5,0.1,0.0), 3.f);
			fluid.addConstantForce(ofPoint(width*0.5, height*0.9), ofPoint(0,-2), ofFloatColor(0.5,0.1,0.0), 3.f);
			fluid.addConstantForce(ofPoint(width*0.75,height*0.9), ofPoint(0,-2), ofFloatColor(0.5,0.1,0.0), 3.f);
			break;
		case 1:
			//fluid.addConstantForce(ofPoint(width*0.5,height*0.5), ofPoint(-10,0), ofFloatColor(0.5,0.1,0.0), 3.f);
			//fluid.addConstantForce(ofPoint(width*0.5,height*0.5), ofPoint(10, 0), ofFloatColor(0.5,0.1,0.0), 3.f);
			//fluid.addConstantForce(ofPoint(width*0.5,height*0.5), ofPoint(0,-10), ofFloatColor(0.5,0.1,0.0), 3.f);
			//fluid.addConstantForce(ofPoint(width*0.5,height*0.5), ofPoint(0, 10), ofFloatColor(0.5,0.1,0.0), 3.f);
			break;
		}
	}

	void update()
	{
		fluid.update();

		// SLOOOOW, framerate drops to 30 fps, from 60 (without Kinect attached)
		//fluid.getVelocityFbo().readToPixels(_velocityMap);

		// instead, get contents from buffer asynchronously without stalling everything
		_fboReader.readToFloatPixels(fluid.getVelocityFbo(), _velocityMap, OF_IMAGE_COLOR);
	}
	void draw(int x, int y, float width, float height)
	{
		fluid.draw(x, y, width, height);
	}

	int getWidth() { return width; }

	int getHeight() { return height; }

	void keyPressed(int key)
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
	ofxFluid& getFluid()
	{
		return fluid;
	}
	ofFloatPixels& getVelocityMap() { return _velocityMap; }

private:
    ofxFluid fluid;
	ofFloatPixels _velocityMap;
	ofxFastFboReader _fboReader;

	int     width,height;
    bool    bPaint, bObstacle, bBounding, bClear;
};