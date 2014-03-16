using namespace ParticleSystem;

class ParticleEffect
{

private:
    ofxGaussianBlur glow;
	int     width,height;

	float _prevTime;
	ParticleManager* _particleManager;
	ParticleGenerator* _particleGenerator;
	ParticleAnimator* _particleAnimator;
	ParticleRenderer* _particleRenderer;


public:
	void setup(int w, int h)
	{
		_prevTime = 0;
		_particleManager = new ParticleManager(1000);
		_particleGenerator = new ParticleGenerator(_particleManager);
		SimulationAnimator* simAnim = new SimulationAnimator(_particleManager);
		simAnim->AddForceField(new VortexForceField(ofVec2f(200.f, 200.f), 30.f));
		_particleAnimator = simAnim;
		_particleRenderer = new ParticleRenderer(_particleManager);

		width=w;
		height=h;

		glow.allocate(width, height); 
		glow.setRadius(5.f);
		glow.setPasses(2);

   	}

	void update(ofVec2f mousePos)
	{
		float currentTime = ofGetElapsedTimef();
		float timeStep = currentTime - _prevTime;

		if(timeStep < 1.0f) // ignore timesteps that are too large
		{
			_particleManager->Update(timeStep);
			_particleGenerator->Generate(timeStep, mousePos); // TODO: specify a position and velocity for the creation
			_particleAnimator->Update(currentTime, timeStep); // compute new properties (position, color, age, etc) all particles

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

			_particleRenderer->Draw(4.f);

			//ofDisableBlendMode();
			glow.end();

			glow.update();
		}

		_prevTime = currentTime;
	}
	void draw(int x, int y, float width, float height)
	{
		// draw the glows of the lines
		glow.draw_dontEnableAlphaBlending(x, y, width, height);

		// draw the lines themselves
		_particleRenderer->Draw(1.f);
	}

	int getWidth() { return width; }

	int getHeight() { return height; }

	ofxGaussianBlur& getGlow() { return glow; }


};