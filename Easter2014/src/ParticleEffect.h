class ParticleEffect
{
public:
	void setup(int w, int h)
	{
		width=w;
		height=h;

		glow.allocate(width, height); 
		glow.setRadius(10.f);
		glow.setPasses(2);
   	}

	void update()
	{
		glow.begin();

		// first fill with black to make sure we don't have
		// any transparent pixels anymore, this would mess
		// up our additive blending later on
		ofSetColor(0, 0, 0, 255);
		ofRectangle(0, 0, width, height);

		//ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofSetColor(255, 255, 255, 255);
		for(int i=0; i<10; i++)
			ofCircle(50*i, 300, 20.f);
		//ofDisableBlendMode();
		glow.end();

		glow.update();
	}
	void draw(int x, int y, float width, float height)
	{
		glow.draw_dontEnableAlphaBlending(x, y, width, height);
	}

	int getWidth() { return width; }

	int getHeight() { return height; }

	ofxGaussianBlur& getGlow() { return glow; }

private:
    ofxGaussianBlur glow;
	int     width,height;

};