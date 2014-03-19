class FluidEffect
{
public:
	void setup(int w, int h, float screenToFluidScale)
	{
		width=w;
		height=h;


	   // Initial Allocation
		//
		// note: we make the size of the simulation independent of the screen size, so that we can test
		// everything in the right proportions and at the right resolution, even before we have the 
		// triple-beamer, double-kinect setup..
		fluid.allocate(width, height, screenToFluidScale); 
    
		_velocityMap.allocate(fluid.getVelocityFbo().getWidth(), fluid.getVelocityFbo().getHeight(), ofImageType::OF_IMAGE_COLOR);
		_fboReader.setAsync(true);

		// Seting the gravity set up & injecting the background image
		//
		fluid.dissipation = 0.99;
		fluid.velocityDissipation = 0.99;
    
		fluid.setGravity(ofVec2f(0.0,-0.005));
	//    fluid.setGravity(ofVec2f(0.0,0.0098));
    
		//  Set obstacle
		//
		//fluid.begin();
		//ofSetColor(0,0);
		//ofSetColor(255);
		//ofCircle(width*0.5, height*0.35, 40);
		//fluid.end();
		//fluid.setUseObstacles(true);
    
		// Adding constant forces
		//
		fluid.addConstantForce(ofPoint(width*0.5,height*0.85), ofPoint(0,-2), ofFloatColor(0.5,0.1,0.0), 3.f);
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
			fluid.addConstantForce(ofPoint(width*0.5,height*0.5), ofPoint(-10,0), ofFloatColor(0.5,0.1,0.0), 3.f);
			fluid.addConstantForce(ofPoint(width*0.5,height*0.5), ofPoint(10, 0), ofFloatColor(0.5,0.1,0.0), 3.f);
			fluid.addConstantForce(ofPoint(width*0.5,height*0.5), ofPoint(0,-10), ofFloatColor(0.5,0.1,0.0), 3.f);
			fluid.addConstantForce(ofPoint(width*0.5,height*0.5), ofPoint(0, 10), ofFloatColor(0.5,0.1,0.0), 3.f);
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