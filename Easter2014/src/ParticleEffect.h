using namespace ParticleSystem;

class ParticleEffect
{

private:
    ofxGaussianBlur glow;
	int     width,height;

	ParticleManager* _particleManager;
	ParticleGenerator* _particleGenerator;
	ParticleAnimator* _particleAnimator;
	ParticleRenderer* _particleRenderer;


public:
	void setup(int w, int h, ofFloatPixels& velocityMap, float screenToFluidScale)
	{
		_particleManager = new ParticleManager(50000);
		_particleGenerator = new ParticleGenerator(_particleManager);
		_particleGenerator->SetGenerationRate(1000);
		_particleGenerator->SetParticleLifeTime(3.f);
		//SimulationAnimator* simAnim = new SimulationAnimator(_particleManager);
		//simAnim->AddForceField(new VortexForceField(ofVec2f(200.f, 200.f), 30.f));
		//_particleAnimator = simAnim;
		FluidSimulationAnimator* fluidSimAnim = new FluidSimulationAnimator(_particleManager, velocityMap, screenToFluidScale);
		_particleAnimator = fluidSimAnim;
		_particleRenderer = new ParticleRenderer(_particleManager);

		width=w;
		height=h;

		glow.allocate(width, height); 
		glow.setRadius(5.f);
		glow.setPasses(2);

   	}

	void exit()
	{
		delete _particleRenderer;
		delete _particleAnimator;
		delete _particleGenerator;
		delete _particleManager;
	}

	void generate(float currentTime, float timeStep, ofVec2f mousePos, ofVec2f direction, float quotaPercentage)
	{
		if(!appSettings::instance().speedBasedGeneration || direction.length() > 4.0f)
			_particleGenerator->Generate(timeStep, mousePos, direction, quotaPercentage); 

	}

	// note: generate has been called before this
	void update(float currentTime, float timeStep)
	{
		if(timeStep < 1.0f) // ignore timesteps that are too large
		{
			_particleManager->Update(timeStep);
			_particleAnimator->Update(currentTime, timeStep); // compute new properties (position, color, age, etc) all particles

			glow.begin();

			// first fill with black to make sure we don't have
			// any transparent pixels anymore, this would mess
			// up our additive blending later on
			ofSetColor(0, 0, 0, 255);
			ofRectangle(0, 0, width, height);

			//some shapes for testing our glow effect
			//ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			// ofSetColor(255, 255, 255, 255);
			//for(int i=0; i<10; i++)
			//	ofCircle(50*i, 300, 20.f);

			// and render our particles
			_particleRenderer->Draw(ofVec2f::zero(), 4.f);

			//ofDisableBlendMode();
			glow.end();

			glow.update();
		}
	}
	void draw(int x, int y, float width, float height)
	{
		// draw the glows of the lines
		glow.draw_dontEnableAlphaBlending(x, y, width, height);

		// draw the lines themselves
		_particleRenderer->Draw(ofVec2f(x, y), 0.5f);
	}

	int getWidth() { return width; }

	int getHeight() { return height; }

	ofxGaussianBlur& getGlow() { return glow; }


};